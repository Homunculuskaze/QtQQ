#include "MsgWebView.h"

#include <QFile>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QWebChannel>
MsgHtmlObj::MsgHtmlObj(QObject* parent) :QObject(parent)
{
	initHtmlTmpl();
}

void MsgHtmlObj::initHtmlTmpl()
{
	m_msgLHtmlTmpl = getMsgTmplHtml("msgleftTmpl");
	m_msgRHtmlTmpl = getMsgTmplHtml("msgrightTmpl");
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

MsgWebView::MsgWebView(QWidget *parent)
	: QWebEngineView(parent)
{
	MsgWebPage* page = new MsgWebPage(this);
	setPage(page);

	QWebChannel* channel = new QWebChannel(this);
	m_msgHtmlObj = new MsgHtmlObj(this);
	channel->registerObject("external", m_msgHtmlObj);
	this->page()->setWebChannel(channel);

	//初始化接收信息页面
	this->load(QUrl("qrc:/Resources/MainWindow/MsgHtml/msgTmpl.html"));
}

MsgWebView::~MsgWebView()
{}

void MsgWebView::appendMsg(const QString& html)
{
	QJsonObject msgObj;
	QString qsMsg;
	const QList<QStringList>msgList = parseHtml(html);			//解析html


	//信息类型，默认给1
	//0是表情，1是文本，2是文件
	//int msgType = 1;
	//bool isImageMsg = false;			//默认的图片信息
	//int imageNum = 0;				//表情数量，默认给0
	//QString strData;					//055，008，表情名称，宽度不够用0补全

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
			//QString strEmotionPath = "qrc:/Resources/MainWindow/emotion/";
			//int pos = strEmotionPath.size();			//表情路径长度
			//isImageMsg = true;

			//获取表情名称
			//通过mid()方法，截取整个字符串中的部分字符串
			//QString strEmotionName = imagePath.mid(pos);
			//吧【.png】用【空】替换，直接改变的就是strEmotionName本身
			//strEmotionName.replace(".png", "");

			//根据表情名称的长度，进行设置表情数据
			//不足 3位，则不足3位，如23.png,则不足为023.png
			//int emotionNameLength = strEmotionName.length();

			//判断长度
			//if (emotionNameLength == 1)
			//{
			//	strData = strData + "00" + strEmotionName;
			//}
			//else if (emotionNameLength == 2)
			//{
			//	strData = strData + "0" + strEmotionName;
			//}
			//else if (emotionNameLength == 3)
			//{
			//	strData = strData + strEmotionName;
			//}

			//msgType = 0;			//表情信息
			//imageNum++;

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
			//strData = qsMsg;				//赋值
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
	this->page()->runJavaScript(QString("appendHtml(%1)").arg(Msg));
	//发信息
	//if (strObj == "0")
	//{
	//	//属于Java脚本语言
	//	this->page()->runJavaScript(QString("appendHtml0(%1)").arg(Msg));

	//	//如果发送的是表情，拿就对发送的数据进行处理
	//	if (isImageMsg)
	//	{
	//		//当前strData保存的只是表情的名称，占位是3个宽度
	//		//这里加上表情的数量
	//		strData = QString::number(imageNum) + "images" + strData;
	//	}

	//	//发送信号，发送信息
	//	emit signalSendMsg(strData, msgType);
	//}
	//else    //来信
	//{
	//	//追加数据，这里就是recvHtml
	//	this->page()->runJavaScript(QString("recvHtml_%1(%2)").arg(strObj).arg(Msg));
	//}
}

QList<QStringList> MsgWebView::parseHtml(const QString & html)
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
