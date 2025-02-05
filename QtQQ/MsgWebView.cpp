#include "MsgWebView.h"
#include "TalkWindowShell.h"
#include "WindowManager.h"
#include <QFile>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QWebChannel>
#include <QSqlQueryModel>
#include <QFileInfo>
extern QString gstrLoginHeadPath;
MsgHtmlObj::MsgHtmlObj(QObject* parent,QString msgLPicPath) :QObject(parent)
{
	m_msgLPicPath = msgLPicPath;
	initHtmlTmpl();
}

//void MsgHtmlObj::initHtmlTmpl()				//原版
//{
//	m_msgLHtmlTmpl = getMsgTmplHtml("msgleftTmpl");
//	m_msgLHtmlTmpl.replace("%1", m_msgLPicPath);
//
//	// 确保路径格式正确
//	QUrl imgUrl = QUrl::fromLocalFile(gstrLoginHeadPath);
//	qDebug() << "转换后的头像路径：" << imgUrl.toString();
//
//	m_msgRHtmlTmpl = getMsgTmplHtml("msgrightTmpl");
//	//m_msgRHtmlTmpl.replace("%1", gstrLoginHeadPath);
//	m_msgRHtmlTmpl.replace("%1", imgUrl.toString());
//}




#include <QFile>
#include <QByteArray>
QString encodeImageToBase64(const QString& filePath) {
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << "无法打开头像: " << filePath;
		return "";
	}
	QByteArray imageData = file.readAll();
	file.close();
	return QString("data:image/png;base64,") + imageData.toBase64();
}



void MsgHtmlObj::initHtmlTmpl()					//可用版本，但是单字符不能用
{
	// **1️⃣ 读取 msgleftTmpl.html**
	m_msgLHtmlTmpl = getMsgTmplHtml("msgleftTmpl");

	// **2️⃣ 替换头像**
	QString base64Image = encodeImageToBase64(m_msgLPicPath);
	qDebug() << "左侧头像 Base64: " << base64Image.left(50) << "...";
	m_msgLHtmlTmpl.replace("%1", base64Image);

	// **3️⃣ 右侧头像**
	QString base64RightImage = encodeImageToBase64(gstrLoginHeadPath);
	qDebug() << "右侧头像 Base64: " << base64RightImage.left(50) << "...";
	m_msgRHtmlTmpl = getMsgTmplHtml("msgrightTmpl");
	m_msgRHtmlTmpl.replace("%1", base64RightImage);
}




QString MsgHtmlObj::getMsgTmplHtml(const QString& code)
{
	//将数据全部读取出来，然后再返回
	QFile file(":/Resources/MainWindow/MsgHtml/" + code + ".html");

	file.open(QFile::ReadOnly);
	QString strData;

	if (file.isOpen())
	{
		//全部一次性读取
		strData = QLatin1String(file.readAll());
	}
	else
	{
		//information （父窗体指针，标题，提示内容）
		//因为当前写代码的对象，是从QObject派生来的，不是从某个部件派生来的
		//因此要写空
		QMessageBox::information(nullptr, "Tips", "Failed to init html!");
		return QString("");
	}
	file.close();

	return strData;
}


MsgWebView::MsgWebView(QWidget *parent)				//原版
	: QWebEngineView(parent)
	,m_channel(new QWebChannel(this))
{
	MsgWebPage* page = new MsgWebPage(this);
	setPage(page);

	//QWebChannel* channel = new QWebChannel(this);
	m_msgHtmlObj = new MsgHtmlObj(this);
	m_channel->registerObject("external0", m_msgHtmlObj);

	TalkWindowShell* talkWindowShell = WindowManager::getInstance()->getTalkWindowShell();
	connect(this, &MsgWebView::signalSendMsg, talkWindowShell, &TalkWindowShell::updateSendTcpMsg);

	//当前正构建的聊天窗口的ID(QQ号)
	QString strTalkId = WindowManager::getInstance()->getCreatingTalkId();

	QSqlQueryModel queryEmployeeModel;
	QString strEmployeeID, strPicturePath;
	QString strExternal;
	bool isGroupTalk = false;

	//获取公司群ID
	queryEmployeeModel.setQuery(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'")
		.arg(QStringLiteral("公司群")));
	QModelIndex companyIndex = queryEmployeeModel.index(0, 0);
	QString strCompanyID = queryEmployeeModel.data(companyIndex).toString();

	if (strTalkId == strCompanyID)			//公司群聊
	{
		isGroupTalk = true;
		queryEmployeeModel.setQuery("SELECT employeeID,picture FROM tab_employees WHERE status = 1");
	}
	else
	{
		if (strTalkId.length() == 4)			//其他群聊
		{
			isGroupTalk = true;
			queryEmployeeModel.setQuery(QString("SELECT employeeID,picture FROM tab_employees WHERE status = 1 AND departmentID = %1").arg(strTalkId));
		}
		else			//单独聊天
		{
			//获取对方QQ号
			queryEmployeeModel.setQuery(QString("SELECT picture FROM tab_employees WHERE status = 1 AND employeeID = %1").arg(strTalkId));

			//通过索引，找出图片路径，并转成字符串
			QModelIndex index = queryEmployeeModel.index(0, 0);
			strPicturePath = queryEmployeeModel.data(index).toString();

			strExternal = "external_" + strTalkId;

			//构建网页对象
			MsgHtmlObj* msgHtmlObj = new MsgHtmlObj(this, strPicturePath);
			m_channel->registerObject(strExternal, msgHtmlObj);
		}
	}

	if (isGroupTalk)
	{
		QModelIndex employeeModelIndex, pictureModelIndex;
		int rows = queryEmployeeModel.rowCount();
		for (int i = 0; i < rows; ++i)
		{
			employeeModelIndex = queryEmployeeModel.index(i, 0);
			pictureModelIndex = queryEmployeeModel.index(i, 1);

			strEmployeeID = queryEmployeeModel.data(employeeModelIndex).toString();
			strPicturePath = queryEmployeeModel.data(pictureModelIndex).toString();

			strExternal = "external_" + strEmployeeID;

			MsgHtmlObj* msgHtmlObj = new MsgHtmlObj(this, strPicturePath);
			m_channel->registerObject(strExternal, msgHtmlObj);
		}
	}

	//初始化接收信息页面
	this->load(QUrl("qrc:/Resources/MainWindow/MsgHtml/msgTmpl.html"));
	this->page()->setWebChannel(m_channel);
}

//MsgWebView::MsgWebView(QWidget* parent)
//	: QWebEngineView(parent)
//	, m_channel(new QWebChannel(this))
//{
//	MsgWebPage* page = new MsgWebPage(this);
//	setPage(page);
//
//	// 1) 默认 external0
//	m_msgHtmlObj = new MsgHtmlObj(this);
//	m_channel->registerObject("external0", m_msgHtmlObj);
//
//	 //添加一个通用 external 对象，这样 JS 中 external = channel.objects.external 就不会是 undefined
//	MsgHtmlObj* commonObj = new MsgHtmlObj(this);
//	m_channel->registerObject("external", commonObj);
//
//	// 2) 获取当前聊天ID
//	QString strTalkId = WindowManager::getInstance()->getCreatingTalkId();
//
//	bool isGroupTalk = false;
//	QSqlQueryModel queryEmployeeModel;
//
//	// 3) 判断公司群ID
//	queryEmployeeModel.setQuery(QString("SELECT departmentID FROM tab_department WHERE department_name='公司群' "));
//	QString strCompanyID = queryEmployeeModel.index(0, 0).data().toString();
//
//	if (strTalkId == strCompanyID) {
//		// 公司群
//		isGroupTalk = true;
//		queryEmployeeModel.setQuery("SELECT employeeID, picture FROM tab_employees WHERE status=1");
//	}
//	else if (strTalkId.length() == 4) {
//		// 其他群
//		isGroupTalk = true;
//		queryEmployeeModel.setQuery(
//			QString("SELECT employeeID, picture FROM tab_employees WHERE status=1 AND departmentID=%1")
//			.arg(strTalkId)
//		);
//	}
//	else {
//		// 单聊
//		isGroupTalk = false;
//		queryEmployeeModel.setQuery(
//			QString("SELECT employeeID, picture FROM tab_employees WHERE status=1 AND employeeID=%1")
//			.arg(strTalkId)
//		);
//	}
//
//	// 4) 如果是群聊，就对所有员工循环注册
//	if (isGroupTalk)
//	{
//		int rows = queryEmployeeModel.rowCount();
//		for (int i = 0; i < rows; ++i) {
//			QString empIdStr = queryEmployeeModel.index(i, 0).data().toString();
//			QString picPath = queryEmployeeModel.index(i, 1).data().toString();
//			QString strExternal = "external_" + empIdStr;
//
//			MsgHtmlObj* msgHtmlObj = new MsgHtmlObj(this, picPath);
//			m_channel->registerObject(strExternal, msgHtmlObj);
//			qDebug() << "注册了对象:" << strExternal;
//		}
//	}
//	// 5) 如果单聊，只需要注册1个
//	else
//	{
//		// 一般只返回1行 1个结果
//		if (queryEmployeeModel.rowCount() > 0)
//		{
//			QString empIdStr = queryEmployeeModel.index(0, 0).data().toString();
//			QString picPath = queryEmployeeModel.index(0, 1).data().toString();
//			QString strExternal = "external_" + empIdStr;
//
//			MsgHtmlObj* msgHtmlObj = new MsgHtmlObj(this, picPath);
//			m_channel->registerObject(strExternal, msgHtmlObj);
//			qDebug() << "注册了对象(单聊):" << strExternal;
//		}
//	}
//
//	// 6) 绑定 WebChannel, 加载页面
//	page->setWebChannel(m_channel);
//	load(QUrl("qrc:/Resources/MainWindow/MsgHtml/msgTmpl.html"));
//}



MsgWebView::~MsgWebView()
{}

void MsgWebView::appendMsg(const QString& html, QString strObj)				//原版
{
	QJsonObject msgObj;
	QString qsMsg;
	const QList<QStringList>msgList = parseHtml(html);			//解析html

	qDebug() << "[C++ debug] appendMsg called: strObj=" << strObj;

	//信息类型，默认给1
	//0是表情，1是文本，2是文件
	int msgType = 1;
	bool isImageMsg = false;			//默认的图片信息
	int imageNum = 0;				//表情数量，默认给0
	QString strData;					//055，008，表情名称，宽度不够用0补全

	for (int i = 0; i < msgList.size(); i++)
	{
		//如果msgList当权循环的链表的第一个元素 是 img文件
		if (msgList.at(i).at(0) == "img")
		{
			//获取图片路径
			//这里写1，0是img，1是图片的真实内容
			QString imagePath = msgList.at(i).at(1);
			QPixmap pixmap;			//用于加载图片

			//获取表情名称的位置
			//表情路径
			QString strEmotionPath = "qrc:/Resources/MainWindow/emotion/";
			int pos = strEmotionPath.size();			//表情路径长度
			isImageMsg = true;

			//获取表情名称
			//通过mid()方法，截取整个字符串中的部分字符串
			QString strEmotionName = imagePath.mid(pos);
			//把【.png】用【空】替换，直接改变的就是strEmotionName本身
			strEmotionName.replace(".png", "");

			//根据表情名称的长度，进行设置表情数据
			//不足 3位，则补足3位，如23.png,则补足为023.png
			int emotionNameLength = strEmotionName.length();				//视频里面用的不是emotionNameLength而是emotionNameL

			//判断长度
			if (emotionNameLength == 1)
			{
				strData = strData + "00" + strEmotionName;
			}
			else if (emotionNameLength == 2)
			{
				strData = strData + "0" + strEmotionName;
			}
			else if (emotionNameLength == 3)
			{
				strData = strData + strEmotionName;
			}

			msgType = 0;			//表情信息
			imageNum++;

			//判断最左边的3个字符，是否为qrc
			if (imagePath.left(3) == "qrc")				//假设路径为 qrc:/MainWindow/xxx
			{
				//mid()方法，从中间位置开始截取
				//从第三个字符开始截取，就是":"冒号开始
				//去掉表情路径中qrc
				pixmap.load(imagePath.mid(3));		//从位置3开始截取（位置0是q），即		:/MainWindow/xxx
			}
			else
			{
				//不是qrc的，就直接加载
				pixmap.load(imagePath);
			}

			//表情图片html格式，文本组合（路径，宽度，高度）
			QString imgPath=QString("<img src=\"%1\"width=\"%2\"heigh=\"%3\"/>")
				.arg(imagePath).arg(pixmap.width()).arg(pixmap.height());			//\" 表示在字符串中插入一个双引号字符（"）

			//存到字符串中
			qsMsg += imgPath;
		}
		else if (msgList.at(i).at(0) == "text")
		{
			//这里写1，0是text，1是文本的真实内容
			qsMsg += msgList.at(i).at(1);
			strData = qsMsg;				//赋值
		}
	}

	//插入到Json对象中，是键值对，
	//“MSG”是键，qsMsg是值
	//体现在资源文件中，以msgleftTmpl.html为例
	//里面的内容中的【MSG】<div class="msg">{{MSG}}<span class>="trigon"
	msgObj.insert("MSG", qsMsg);


	//一切的目的，都是为了转成QString字符串类型
	//再转换成Json文档，并且要转成UTF-8的文档
	//QJsonDocument::Compact，紧凑的意思
	const QString& Msg = QJsonDocument(msgObj).toJson(QJsonDocument::Compact);
	qDebug() << "[C++ debug] => Call appendHtml(...) in JS, param=" << Msg;
	this->page()->runJavaScript(QString("appendHtml(%1)").arg(Msg));

	//发信息
	if (strObj == "0")
	{
		//属于Java脚本语言
		qDebug() << "[C++ debug] => strObj=0, call appendHtml0(...) in JS";
		this->page()->runJavaScript(QString("appendHtml0(%1)").arg(Msg));

		//如果发送的是表情，拿就对发送的数据进行处理
		if (isImageMsg)
		{
			//当前strData保存的只是表情的名称，占位是3个宽度
			//这里加上表情的数量
			strData = QString::number(imageNum) + "images" + strData;
		}

		//发送信号，发送信息
		emit signalSendMsg(strData, msgType);
	}
	else    //来信
	{
		//追加数据，这里就是recvHtml
		qDebug() << "[C++ debug] => strObj=" << strObj << ", call recvHtml_" << strObj << "(...) in JS";
		this->page()->runJavaScript(QString("recvHtml_%1(%2)").arg(strObj).arg(Msg));
	}
}






QList<QStringList> MsgWebView::parseHtml(const QString & html)				//原版
{
	//因为传进来的是html文件，将它转换成Qt节点文件
	QDomDocument doc;
	doc.setContent(html);				//转换

	//想要解析的节点，就是html里面的body
	//想拿到body，需要先获取元素
	//节点元素
	const QDomElement& root = doc.documentElement();
	//获取第一个元素，node是节点类型，还需要对节点进行解析
	const QDomNode& node = root.firstChildElement("body");

	return parseDocNode(node);
}


bool MsgWebPage::acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame)
{
	//仅接收 qrc:/*.html
	//判断URL类型，是否为qrc类型
	if (url.scheme() == QString("qrc"))
	{
		return true;
	}
	else
	{
		return false;
	}
}
