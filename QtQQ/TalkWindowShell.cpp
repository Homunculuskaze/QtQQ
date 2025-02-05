//原版代码
#include "TalkWindowShell.h"
#include "EmotionWindow.h"
#include "CommonUtils.h"
#include "TalkWindow.h"
#include "TalkWindowItem.h"
#include "WindowManager.h"
#include "QMsgTextEdit.h"
#include "ReceiveFile.h"
#include <QListWidget>
//#include <QPoint>
#include <QFile>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QDir>
#include <QRegularExpression>
#include <QSqlQuery>

QString gfileName;				//文件名称。在这里定义，在ReceiveFile.cpp里面声明外部链接extern QString gfileName;
QString gfileData;				//文件内容。在这里定义，在ReceiveFile.cpp里面声明外部链接extern QString gfileData;


// 简易函数：将 "01005" -> "1005"，"00000" -> "0"，"10005" -> "10005"
static QString removeLeadingZeros(const QString& str)
{
	bool ok = false;
	int val = str.toInt(&ok);    // 尝试转整数
	if (ok) {
		// 转回字符串，就去掉了前导零
		return QString::number(val);
	}
	// 如果非数字(比如空、包含字母)，就原样返回
	return str;
}

extern QString gLoginEmployeeID;
const int gUdpPort = 6666;			//+g表示global全局变量

TalkWindowShell::TalkWindowShell(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initControl();
	initTcpSocket();
	initUdpSocket();

	//JS文件，没有内容得时候，才开始写入数据
	QFile file("C:/Users/12054/source/repos/QtQQ/QtQQ/Resources/MainWindow/MsgHtml/msgtmpl.js");


	if (!file.exists()) {
		QMessageBox::information(this, "提示", "文件不存在！");
		return;
	}

	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		QMessageBox::information(this, "提示", "打开文件失败，错误信息：" + file.errorString());
		return;
	}




	if (!file.size())
	{
		QStringList employeesIDList;
		getEmployeesID(employeesIDList);
		if (!createJSFile(employeesIDList))
			{
			QMessageBox::information(this, QString::fromUtf8("提示"),
				QString::fromUtf8("更新js文件数据失败！"));
			}
	}
	
}



TalkWindowShell::~TalkWindowShell()
{
	delete m_emotionWindow;
	m_emotionWindow=nullptr;
}

//void TalkWindowShell::addTalkWindow(TalkWindow* talkWindow, TalkWindowItem* talkWindowItem,const QString& uid/*, GroupType grouptype*/)						//原版可用
//{
//	ui.rightStackedWidget->addWidget(talkWindow);
//
//	//发送者：表情窗口，发射表情窗口隐藏信号
//	//接收者：聊天窗口，设置表情按钮的状态
//	connect(m_emotionWindow, SIGNAL(signalEmotionWindowHide()),
//		talkWindow, SLOT(onSetEmotionBtnStatus()));
//
//	//创建新聊天项
//	QListWidgetItem* aItem = new QListWidgetItem(ui.listWidget);
//
//	//把新聊天项，插入到映射链表中
//	//一项 对应 一个聊天窗口
//	m_talkwindowItemMap.insert(aItem, talkWindow);
//
//	aItem->setSelected(true);			//被选中
//
//	//判断群聊还是单聊
//	QSqlQueryModel sqlDepModel;
//	QString strQuery = QString("SELECT picture FROM tab_department WHERE departmentID = %1").arg(uid);
//	sqlDepModel.setQuery(strQuery);
//	int rows = sqlDepModel.rowCount();
//
//	if (rows == 0)		//单聊
//	{
//		strQuery = QString("SELECT picture FROM tab_employees WHERE employeeID = %1").arg(uid);
//		sqlDepModel.setQuery(strQuery);
//	}
//
//	QModelIndex index;
//	index = sqlDepModel.index(0, 0);			//0行0列
//	QImage img;
//	img.load(sqlDepModel.data(index).toString());
//
//	talkWindowItem->setHeadPixmap(QPixmap::fromImage(img));			//设置头像
//	ui.listWidget->addItem(aItem);
//	ui.listWidget->setItemWidget(aItem, talkWindowItem);
//
//	onTalkWindowItemClicked(aItem);
//
//	connect(talkWindowItem, &TalkWindowItem::signalCloseClicked, [talkWindowItem, talkWindow, aItem, this]() {
//		m_talkwindowItemMap.remove(aItem);			//先处理左边部分,删除项
//		talkWindow->close();								//先处理左边部分,聊天窗口关闭
//		ui.listWidget->takeItem(ui.listWidget->row(aItem));
//		delete talkWindowItem;
//		// 通过 takeItem方法 移除 ，参数是行号，
//		// 调用row方法，将 aItem 项，传进去
//		ui.rightStackedWidget->removeWidget(talkWindow);
//		if (ui.rightStackedWidget->count() < 1)
//			close();
//		});
//	//
//	////判断是 群聊 还是单聊
//	//QSqlQueryModel sqlDepModel;
//	//QString strQuery = QString("SELECT picture FROM tab_department WHERE departmentID = %1").arg(uid);
//	//sqlDepModel.setQuery(strQuery);
//	//int rows = sqlDepMedel.rowCount();				//获取行号
//
//	////判断是单聊
//	//if (rows == 0)
//	//{
//	//	strQuery = QString("SELECT picture FROM tab_employee WHERE employeeID=%1").arg(uid);
//	//	sqlDepModel.setQuery(strQuery);
//	//}
//
//	//QModelIndex index;
//	//index = sqlDepModel.index(0, 0);					//0行,0列
//
//	//QImage img;
//	////获取图片路径，加载
//	//img.load(sqlDepModel.data(index).toString());
//
//	////设置头像
//	//talkWindowItem->setHeadPixmap(QPixmap::fromImage(img));
//	////添加项
//	//ui.listWidget->addItem(aItem);
//	////设置列表部件
//	//ui.listWidget->setItemWidget(aItem, talkWindowItem);
//
//	////被点击
//	//onTalkWindowItemClicked(aItem);
//
//	////需要捕获的信号非常多，窗口关闭之后，要进行资源回收
//	////（1）聊天窗口要关闭 ，	（2）当前聊天项要移除    （3）talkWindow资源要释放	（4）右边多页面的显示，也要删除
//	//connect(talkWindowItem, &TalkWindowItem::signalCloseClicked, [talkWindowItem, talkWindow, aItem, this]()
//	//	{
//	//		//先处理左边部分
//	//		m_talkwindowItemMap.remove(aItem);			//删除项
//	//		talkWindow->close();										//聊天窗口关闭
//	//		//通过takeItem方法移除，参数是行号，
//	//		//调用row方法，将aItem项，传进去
//	//		ui.listWidget->talkItem(ui.listWidget->row(aItem));
//	//		delete talkWindowItem;
//
//	//		//再处理右边部分
//	//		ui.rightStackedWidget->removeWidget(talkWindow);
//
//	//		//做一个判断，如果删除的是最后一个 窗口就将窗口关闭
//	//		if (ui.rightStackedWidget->count() < 1)
//	//		{
//	//			close();
//	//		}
//	//	});
//	//
//}

void TalkWindowShell::addTalkWindow(TalkWindow* talkWindow,
	TalkWindowItem* talkWindowItem,
	const QString& uid)
{
	ui.rightStackedWidget->addWidget(talkWindow);

	connect(m_emotionWindow, SIGNAL(signalEmotionWindowHide()),
		talkWindow, SLOT(onSetEmotionBtnStatus()));

	QListWidgetItem* aItem = new QListWidgetItem(ui.listWidget);
	m_talkwindowItemMap.insert(aItem, talkWindow);
	aItem->setSelected(true);

	// ========== 新增：去掉前导零，然后设置给 TalkWindow ==========
	// （若你想直接用 uid，也可以省略去前导零，只要与解包时一致即可）
	QString finalId = removeLeadingZeros(uid);
	talkWindow->setTalkId(finalId);
	// =======================================================

	// 后面保持原逻辑 (查头像、设置列表等)
	QSqlQueryModel sqlDepModel;
	QString strQuery = QString("SELECT picture FROM tab_department WHERE departmentID = %1").arg(uid);
	sqlDepModel.setQuery(strQuery);
	int rows = sqlDepModel.rowCount();

	if (rows == 0) //单聊
	{
		strQuery = QString("SELECT picture FROM tab_employees WHERE employeeID = %1").arg(uid);
		sqlDepModel.setQuery(strQuery);
	}

	QModelIndex index = sqlDepModel.index(0, 0);
	QImage img;
	img.load(sqlDepModel.data(index).toString());
	talkWindowItem->setHeadPixmap(QPixmap::fromImage(img));

	ui.listWidget->addItem(aItem);
	ui.listWidget->setItemWidget(aItem, talkWindowItem);
	onTalkWindowItemClicked(aItem);

	connect(talkWindowItem, &TalkWindowItem::signalCloseClicked,
		[talkWindowItem, talkWindow, aItem, this]() {
			m_talkwindowItemMap.remove(aItem);
			talkWindow->close();
			ui.listWidget->takeItem(ui.listWidget->row(aItem));
			delete talkWindowItem;
			ui.rightStackedWidget->removeWidget(talkWindow);
			if (ui.rightStackedWidget->count() < 1)
				close();
		});
}



void TalkWindowShell::setCurrentWidget(QWidget* widget)
{
	ui.rightStackedWidget->setCurrentWidget(widget);
}

const QMap<QListWidgetItem*, QWidget*>& TalkWindowShell::getTalkWindowItemMap() const
{
	return m_talkwindowItemMap;
}

void TalkWindowShell::initControl()
{
	loadStyleSheet("TalkWindow");
	setWindowTitle(QString::fromLocal8Bit("奇奇-聊天窗口"));

	m_emotionWindow = new EmotionWindow;
	m_emotionWindow->hide();		//隐藏表情窗口

	QList<int> leftWidgetSize;
	leftWidgetSize << 154 << width() - 154;
	ui.splitter->setSizes(leftWidgetSize);				//分裂器设置尺寸
	
	ui.listWidget->setStyle(new CustomProxyStyle(this));

	connect(ui.listWidget, &QListWidget::itemClicked, this, &TalkWindowShell::onTalkWindowItemClicked);
	connect(m_emotionWindow, SIGNAL(signalEmotionItemClicked(int)), this, SLOT(onEmotionItemClicked(int)));
}

void TalkWindowShell::initTcpSocket()			//tcp绑定ip地址
{
	m_tcpClientSocket = new QTcpSocket(this);
	m_tcpClientSocket->connectToHost("127.0.0.1", gtcpPort);			//IP,端口
}

//void TalkWindowShell::initUdpSocket()			//udp绑定端口		原版
//{
//	m_udpReceiver = new QUdpSocket(this);
//	for (quint16 port = gUdpPort; port < gUdpPort + 200; ++port)
//	{
//		if (m_udpReceiver->bind(port, QUdpSocket::ShareAddress))			//一旦成功绑定一个端口，就不会继续尝试绑定后续的端口
//			break;
//	}
//	connect(m_udpReceiver, &QUdpSocket::readyRead, this, &TalkWindowShell::processPendingData);
//}

//void TalkWindowShell::initUdpSocket()
//{
//	m_udpReceiver = new QUdpSocket(this);
//	bool success = false;
//
//	for (quint16 port = gUdpPort; port < gUdpPort + 200; ++port) {
//		if (m_udpReceiver->bind(QHostAddress::AnyIPv4, port, QUdpSocket::ShareAddress)) {
//			qDebug() << "客户端成功绑定 UDP 端口：" << port;
//			success = true;
//			break;
//		}
//	}
//
//	if (!success) {
//		qDebug() << "客户端 UDP 端口绑定失败！";
//	}
//
//	// 连接 UDP 数据接收槽函数
//	connect(m_udpReceiver, &QUdpSocket::readyRead, this, &TalkWindowShell::processPendingData);
//}

//void TalkWindowShell::initUdpSocket()
//{
//	m_udpReceiver = new QUdpSocket(this);
//
//	// 允许多个客户端绑定同一端口
//	if (m_udpReceiver->bind(QHostAddress::AnyIPv4, 6666, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
//		qDebug() << "客户端成功绑定 UDP 端口：6666";
//	}
//	else {
//		qDebug() << "客户端 UDP 端口绑定失败：" << m_udpReceiver->errorString();
//	}
//
//	// 连接 UDP 数据接收槽函数
//	connect(m_udpReceiver, &QUdpSocket::readyRead, this, &TalkWindowShell::processPendingData);
//}

void TalkWindowShell::initUdpSocket()
{
	m_udpReceiver = new QUdpSocket(this);

	// 绑定 6666 端口，并允许端口复用
	if (m_udpReceiver->bind(6666, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
		qDebug() << "客户端成功绑定 UDP 端口：6666";
	}
	else {
		qDebug() << "客户端 UDP 端口绑定失败！";
		return;
	}

	// 监听 UDP 数据
	connect(m_udpReceiver, &QUdpSocket::readyRead, this, &TalkWindowShell::processPendingData);
}


//void TalkWindowShell::getEmployeesID(QStringList& employeesList)				//原版
//{
//	QSqlQueryModel queryModel;
//	queryModel.setQuery("SELECT employeeID FROM tab_employees WHERE status = 1");
//
//	//返回模型的总行数（员工的总数）
//	int employeesNum = queryModel.rowCount();
//
//	QModelIndex index;
//	for (int i = 0; i < employeesNum; ++i)
//	{
//		//返回i行，0列的索引值
//		index = queryModel.index(i, 0);		
//		//通过索引，拿到数据，再转成字符串，最后添加到链表中
//		employeesList<<queryModel.data(index).toString();
//	}
//
//}

void TalkWindowShell::getEmployeesID(QStringList& employeesList)
{
	QSqlQueryModel queryModel;
	queryModel.setQuery("SELECT employeeID FROM tab_employees WHERE status = 1");



	int employeesNum = queryModel.rowCount();
	qDebug() << "获取到的员工数量：" << employeesNum;

	QModelIndex index;
	for (int i = 0; i < employeesNum; ++i)
	{
		index = queryModel.index(i, 0);
		QString empID = queryModel.data(index).toString();
		employeesList << empID;
		qDebug() << "获取到的员工ID：" << empID;
	}
}



//bool TalkWindowShell::createJSFile(QStringList& employeesList)				//原版
//{
//	//读取txt文件数据
//	QString strFileTxt = ":/Resources/MainWindow/MsgHtml/msgtmpl.txt";
//	//QString strFileTxt = "C:/Users/12054/source/repos/QtQQ/QtQQ/Resources/MainWindow/MsgHtml/msgtmpl.txt";  // 更正为文件路径
//	QFile fileRead(strFileTxt);
//
//	QString strFile;			//保存读取到的数据
//
//	//判断以只读模式打开，能否打开成功
//	if (fileRead.open(QIODevice::ReadOnly))
//	{
//		//打开成功,读取全部
//		strFile = fileRead.readAll();
//		fileRead.close();					//读取完了就关闭
//	}
//	else
//	{
//		QMessageBox::information(this, QString::fromUtf8("提示"),
//			QString::fromUtf8("读取msgtmpl.txt失败"));
//			return false;
//	}
//
//	//替换（external0,appendHtml0.用作自己发信息使用）
//	//QFile fileWrite(":/Resources/MainWindow/MsgHtml/msgtmpl.js");
//	QFile fileWrite("C:/Users/12054/source/repos/QtQQ/QtQQ/Resources/MainWindow/MsgHtml/msgtmpl.js");
//
//
//	//写入模式，和覆盖模式
//	if (fileWrite.open(QIODevice::WriteOnly | QIODevice::Truncate))
//	{
//		/*
//			1.初始化对象为null
//			2.通道里面设置对象
//			3.在接受html那里写入数据
//		*/
//
//		//更新空值
//		//原始的，初始化的，空值的代码
//		QString strSourceInitNull = "var external=null;";
//
//		//更新初始化
//		//原始的，初始化的对象的代码
//		//QString strSourceInit = "external=channel.objects.external;";
//		QString strSourceInit = "external = channel.objects.external;";
//
//		//更新newWebChannel
//		//这里吧external0去掉，等到时候再添加上
//		//原始的都用external
//		QString strSourceNew = "new QWebChannel(qt.webChannelTransport,\
//													function(channel){\
//													external=channel.objects.external;\
//		}\
//		);\
//		";
//
//		//更新追加recvHtml
//		/*
//			这一段里面有双引号，会引起冲突
//			所以不能像上面那样直接卸载双引号里，
//			因此，要通过读取字符串的方法，读到strSourceRecvHtml里面
//			把下面这段代码，单独放到一个txt文件里，再进行读取
//
//			function recvHtml(msg){
//					$("#placeholder").append(external.msgLHtmlTmpl.format(msg));
//					window.scrollTo(0,document.body.scrollHright);;
//			};
//		*/
//		QString strSourceRecvHtml;
//		QFile fileRecvHtml(":/Resources/MainWindow/MsgHtml/recvHtml.txt");
//		//QFile fileRecvHtml("C:/Users/12054/source/repos/QtQQ/QtQQ/Resources/MainWindow/MsgHtml/recvHtml.txt");
//		if (fileRecvHtml.open(QIODevice::ReadOnly))
//		{
//			//先读取全部，再赋值，再关闭
//			strSourceRecvHtml = fileRecvHtml.readAll();
//			fileRecvHtml.close();
//		}
//		else
//		{
//			qDebug() << "Failed to open recvHtml.txt:" << fileRecvHtml.errorString();
//			QMessageBox::information(this,
//				QString::fromUtf8("提示"),
//				QString::fromUtf8("读取recvHtml.txt失败"));
//				return false;
//		}
//
//		//保存替换后的脚本，对应上面的4个
//		QString strReplaceInitNull;
//		QString strReplaceInit;
//		QString strReplaceNew;
//		QString strReplaceRecvHtml;
//
//		for (int i = 0; i < employeesList.length(); ++i)
//		{
//			//编辑替换后的空值
//			QString strInitNull = strSourceInitNull;
//			strInitNull.replace("external", QString("external_%1").arg(employeesList.at(i)));
//			strReplaceInitNull += strInitNull;
//			strReplaceInitNull += "\n";
//
//			//编辑替换后的初始值
//			QString strInit = strSourceInit;
//			strInit.replace("external", QString("external_%1").arg(employeesList.at(i)));
//			strReplaceInit += strInit;
//			strReplaceInit += "\n";
//
//			//编辑替换后的newWebchannel
//			QString strNew = strSourceNew;
//			strNew.replace("external", QString("external_%1").arg(employeesList.at(i)));
//			strReplaceNew += strNew;
//			strReplaceNew += "\n";
//
//			//replace,替换修改的，直接就是strRecvHtml
//			//编辑替换后的recvHtml
//			QString strRecvHtml = strSourceRecvHtml;
//			strRecvHtml.replace("external", QString("external_%1").arg(employeesList.at(i)));
//			strRecvHtml.replace("recvHtml", QString("recvHtml_%1").arg(employeesList.at(i)));
//			strReplaceRecvHtml += strRecvHtml;
//			strReplaceRecvHtml += "\n";
//		}
//
//		//上面的for循环走完，有几个员工，就会出现几组
//		//然后再将替换后的字符串
//		//拿来替换到读取数据的【原始文件】的【原字符串】上
//		strFile.replace(strSourceInitNull,strReplaceInitNull);
//		strFile.replace(strSourceInit,strReplaceInit);
//		strFile.replace(strSourceNew,strReplaceNew);
//		strFile.replace(strSourceRecvHtml,strReplaceRecvHtml);
//
//		//strFile就是最终数据可
//		//用一个文件流，写入进去
//		QTextStream stream(&fileWrite);
//		stream << strFile;
//		fileWrite.close();
//
//		return true;
//	}
//	else
//	{
//		QMessageBox::information(this,
//			QString::fromUtf8("提示"),
//			QString::fromUtf8("写入 msgtmpl.js失败"));
//
//			return false;
//	}
//}

//最终版
bool TalkWindowShell::createJSFile(QStringList& employeesList) {
	// 读取txt文件数据
	QString strFileTxt = ":/Resources/MainWindow/MsgHtml/msgtmpl.txt";
	QFile fileRead(strFileTxt);

	QString strFile;  // 保存读取到的数据

	// 判断以只读模式打开，能否打开成功
	if (fileRead.open(QIODevice::ReadOnly)) {
		// 打开成功,读取全部
		strFile = fileRead.readAll();
		fileRead.close();  // 读取完了就关闭
	}
	else {
		QMessageBox::information(this, QString::fromUtf8("提示"),
			QString::fromUtf8("读取msgtmpl.txt失败"));
		return false;
	}

	// 读取替换的初始化部分
	QString strSourceInitNull = "var external = null;";
	QString strSourceInit = "external = channel.objects.external;";
	QString strSourceNew = "new QWebChannel(qt.webChannelTransport, function(channel){ external=channel.objects.external; });";

	// 读取需要插入的函数
	QString strSourceRecvHtml;
	QFile fileRecvHtml(":/Resources/MainWindow/MsgHtml/recvHtml.txt");
	if (fileRecvHtml.open(QIODevice::ReadOnly)) {
		strSourceRecvHtml = fileRecvHtml.readAll();
		fileRecvHtml.close();
	}
	else {
		qDebug() << "Failed to open recvHtml.txt:" << fileRecvHtml.errorString();
		QMessageBox::information(this, QString::fromUtf8("提示"),
			QString::fromUtf8("读取recvHtml.txt失败"));
		return false;
	}

	// 创建替换后的字符串
	QString strReplaceInitNull;
	QString strReplaceInit;
	QString strReplaceNew;
	QString strReplaceRecvHtml;

	for (int i = 0; i < employeesList.length(); ++i) {
		// 生成替换后的初始化部分
		QString strInitNull = strSourceInitNull;
		strInitNull.replace("external", QString("external_%1").arg(employeesList.at(i)));
		strReplaceInitNull += strInitNull;
		strReplaceInitNull += "\n";

		QString strInit = strSourceInit;
		strInit.replace("external", QString("external_%1").arg(employeesList.at(i)));
		strReplaceInit += strInit;
		strReplaceInit += "\n";

		QString strNew = strSourceNew;
		strNew.replace("external", QString("external_%1").arg(employeesList.at(i)));
		strReplaceNew += strNew;
		strReplaceNew += "\n";

		// 生成替换后的 recvHtml
		QString strRecvHtmlTemp = strSourceRecvHtml;
		strRecvHtmlTemp.replace("external", QString("external_%1").arg(employeesList.at(i)));
		strRecvHtmlTemp.replace("recvHtml", QString("recvHtml_%1").arg(employeesList.at(i)));
		strReplaceRecvHtml += strRecvHtmlTemp;
		strReplaceRecvHtml += "\n";
	}

	// 将替换后的字符串应用到文件内容
	//strFile.replace(strSourceInitNull, strReplaceInitNull);			//使用这个会使external显示不完全
	// 
	// 使用正则表达式进行精确替换，忽略空格差异
	QRegularExpression regexInitNull("var\\s+external\\s*=\\s*null;");
	strFile.replace(regexInitNull, strReplaceInitNull);
	
	strFile.replace(strSourceInit, strReplaceInit);
	strFile.replace(strSourceNew, strReplaceNew);
	strFile.replace(strSourceRecvHtml, strReplaceRecvHtml);

	// 写入最终的 JS 文件
	QFile fileWrite("C:/Users/12054/source/repos/QtQQ/QtQQ/Resources/MainWindow/MsgHtml/msgtmpl.js");
	if (fileWrite.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		QTextStream stream(&fileWrite);
		stream << strFile;
		fileWrite.close();
		return true;
	}
	else {
		QMessageBox::information(this, QString::fromUtf8("提示"),
			QString::fromUtf8("写入 msgtmpl.js失败"));
		return false;
	}
}





//void TalkWindowShell::handleReceivedMsg(int senderEmployeeID, int msgType, QString strMsg)					//原版
//{
//	QMsgTextEdit msgTextEdit;
//	msgTextEdit.setText(strMsg);
//
//	if (msgType == 1)			//文本信息
//	{
//		msgTextEdit.document()->toHtml();
//	}
//	else if (msgType == 0)			//表情信息
//	{
//
//		qDebug() << "表情数据：" << strMsg;
//
//		const int emotionWidth = 3;			
//		int emotionNum = strMsg.length() / emotionWidth;				//表情数量 = 表情总长度/单表情宽度
//
//		for (int i = 0; i < emotionNum; ++i)
//		{
//			msgTextEdit.addEmotionUrl(strMsg.mid(i * emotionNum, emotionWidth).toInt());			//一个表情一个表情的拿
//		}
//	}
//
//	QString html = msgTextEdit.document()->toHtml();
//
//	//文本html如果没有字体则添加字体
//	if (!html.contains(".png") && !html.contains("</span>"))
//	{
//		QString fontHtml;
//		QFile file(":/Resources/MainWindow/MsgHtml/msgFont.txt");
//		if (file.open(QIODevice::ReadOnly))
//		{
//			fontHtml = file.readAll();
//			fontHtml.replace("%1", strMsg);
//			file.close();
//		}
//		else
//		{
//			QMessageBox::information(this, QString::fromUtf8("提示"),
//				QString::fromUtf8("文件 msgFont.txt 不存在！"));
//			return;
//		}
//
//		if (!html.contains(fontHtml))
//		{
//			html.replace(strMsg, fontHtml);
//		}
//	}
//
//	TalkWindow* talkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
//	talkWindow->ui.msgWidget->appendMsg(html, QString::number(senderEmployeeID));
//}



void TalkWindowShell::handleReceivedMsg(int senderEmployeeID, int msgType, QString strMsg)			//复制的示例代码
{
	// QMsgTextEdit，这个类的默认参数，是一个部件指针
	// 将 参数默认值 设为 nullptr ，这样构造对象时候，可以不带参数
	QMsgTextEdit msgTextEdit;	// 信息文本编辑器
	msgTextEdit.setText(strMsg);

	// 这里只处理 文本信息，表情信息。
	// 文件类型，不调用这个方法
	if (msgType == 1)	// 文本信息												///////////这里开始可能有问题
	{
		// 将信息，转换为 html
		msgTextEdit.document()->toHtml();
	}
	else if (msgType == 0)	// 表情信息
	{
		// 每个表情所占宽度
		const int emotionWidth = 3;
		// 计算表情数量，数据长度 除以 每个表情宽度
		int emotionNum = strMsg.length() / emotionWidth;

		// 遍历数据中的 表情，添加html里去
		for (int i = 0; i < emotionNum; i++)
		{
			// 截取数据中的字符串，
			// 从哪里截取，从数据最开始的地方截取(在调用这个函数之前，数据已经处理完了，拿到了最后的表情数据部分)
			// 截取宽度：一次截取3个宽度(表情060，005，101)，每个表情占3位宽度
			// 再转成int类型，加载进去
			msgTextEdit.addEmotionUrl(strMsg.mid(i * 3, emotionWidth).toInt());
		}
	}

	// 判断，追加html的文本里面，有没有字体，颜色的设置
	// 如果发送的是表情，则不需要判断
	QString hemlText = msgTextEdit.document()->toHtml();

	// 文本HTML，如果没有字体，则添加字体
	// 首先要保证是 文本类型
	// 如果是表情的话，字符串中 肯定有 .png
	if (!hemlText.contains(".png") && !hemlText.contains("</span>"))
	{
		QString fontHtml;
		QFile file(":/Resources/MainWindow/MsgHtml/msgFont.txt");
		if (file.open(QIODevice::ReadOnly))
		{
			fontHtml = file.readAll();
			// 将html文件里的 %1，用字符串 text 替换
			fontHtml.replace("%1", strMsg);
			file.close();
		}
		else
		{
			// this，当前聊天部件，作为父窗体
			QMessageBox::information(this, QString::fromLocal8Bit("提示"),
				QString::fromLocal8Bit("文件 msgFont.txt 不存在！"));
			return;
		}

		// 判断转换后，有没有包含 fontHtml
		if (!hemlText.contains(fontHtml))
		{
			hemlText.replace(strMsg, fontHtml);
		}
	}

	/*
		将数据添加到"网页"中
		TalkWindowShell中，右侧部件rightStackedWidget，里面有很多页面
		每个页面，就是一个聊天窗口
		所以在将数据添加到"网页"时，要添加到当前的"页面"中
	*/

	// 先获取 窗口地址
	// 右侧部件rightStackedWidget，获取当前的 活动页面
	// 返回的是 QWidget类型指针，要转换成 TalkWindow* 类型指针
	TalkWindow* talkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());

	// 终于最后一步，开始 追加信息
	// 1）：转换类型问题
	// appendMsg()，函数原型，第二个参数是字符串，传的是 发送者的ID
	// 在当前这个函数里，参数 发送者的ID 是int类型，两个类型不匹配，因此要转换一下
	// 2）：这个ui是 TalkWindow 的私有成员，在 TalkWindowShell 里无法访问
	// 因此 设置成友元类
	talkWindow->ui.msgWidget->appendMsg(hemlText, QString::number(senderEmployeeID));

	//qDebug() << "[DEBUG] 正在向目标窗口添加消息：" << strMsg;
	//talkWindow->ui.msgWidget->appendMsg(strMsg, QString::number(senderEmployeeID));



}










/*
	【文本】数据包格式：
	群聊标志（0私聊，1群聊） + 发信息员工QQ号 + 收信息员工QQ号（群QQ号）
		+ 信息类型（文本） + 数据长度 + 文本数据

	【表情】数据包格式：
	群聊标志（0私聊，1群聊） + 发信息员工QQ号 + 收信息员工QQ号（群QQ号）
		+ 信息类型（表情） + 表情个数 + images + 表情数据

	【文件】数据包格式：
	群聊标志（0私聊，1群聊） + 发信息员工QQ号 + 收信息员工QQ号（群QQ号）
		+ 信息类型（2文件） + 文件长度 + "bytes" + 文件名称 + "data_begin" + 文件内容

	msgType，信息类型，0是表情，1是文本，2是文件
*/
void TalkWindowShell::updateSendTcpMsg(QString& strData, int& msgType, QString fileName)
{
	/*
		发送数据包
		里面需要定义一些数据包的格式
		方便其它客户端接收数据

		将数据包发送到服务器之后，服务器再解析数据包
		再由服务器通过广播发送到其它客户端上
	*/

	//获取当前聊天窗口，就是当前活动的窗口
	//ui.rightStackedWidget->currentWidget(),获取当前部件
	TalkWindow* curTalkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());

	//getTalkID(),为了获取TalkWindow私有成员m_talkId (即聊天窗口的ID)
	QString talkId = curTalkWindow->getTalkId();

	QString strGroupFlag;
	if (talkId.length() == 4)			//群QQ的长度
	{
		strGroupFlag = "1";
	}
	else
	{
		strGroupFlag = "0";
	}

	//发送数据的长度
	int nstrDataLength= strData.length();			//例如strData为"helloworld",nstrDataLength为10，

	//将长度转换成宽度
	int dataLength = QString::number(nstrDataLength).length();			//例如strData为"helloworld"，nstrDataLength为10，dataLength为2


	QString strdataLength;
	QString strSend;				//用于发送
	if (msgType == 1)			//发送文本信息
	{
		// 文本信息的长度约定为5位，保证数据包长度，始终为5位宽
		// 例如发送【hello】，5个长度的数据包，"5"
		// 那么 数据长度dataLength "5" ，宽度 就是 【1】了
		// 如果 数据长度dataLength "50" ，宽度 就是 【2】了
		// 如果 数据长度dataLength "500" ，宽度 就是 【3】了，以此类推
		if (dataLength == 1)			//如果发送hello,则长度是5
		{
			strdataLength = "0000" + QString::number(nstrDataLength);
		}
		else if(dataLength == 2)
		{
			strdataLength = "000" + QString::number(nstrDataLength);
		}
		else if (dataLength == 3)
		{
			strdataLength = "00" + QString::number(nstrDataLength);
		}
		else if (dataLength == 4)
		{
			strdataLength = "0" + QString::number(nstrDataLength);
		}
		else if (dataLength == 5)
		{
			strdataLength = QString::number(nstrDataLength);
		}
		else
		{
			QMessageBox::information(this,
				QString::fromUtf8("提示"),
				QString::fromUtf8("不合理的数据长度！"));
		}

		/*
			【文本】数据包格式：
			群聊标志（0私聊，1群聊） + 发信息员工QQ号 + 收信息员工QQ号（群QQ号）
				+ 信息类型（1文本） + 数据长度 + 文本数据  
		*/
		// 开始发送，文本
		strSend = strGroupFlag + gLoginEmployeeID + talkId + "1" + strdataLength + strData;
	}
	else if (msgType == 0)		//表情信息
	{
		/*
			【表情】数据包格式：
			群聊标志（0私聊，1群聊） + 发信息员工QQ号 + 收信息员工QQ号（群QQ号）
				+ 信息类型（0表情） + 表情个数 + images + 表情数据
		*/
		//开始发送表情
		strSend = strGroupFlag + gLoginEmployeeID + talkId + "0" + strData;
	}
	else if (msgType == 2)		//文件信息
	{
		/*
			【文件】数据包格式：
			群聊标志（0私聊，1群聊） + 发信息员工QQ号 + 收信息员工QQ号（群QQ号）
				+ 信息类型（2文件） + 文件长度 + "bytes" + 文件名称 + "data_begin" + 文件内容
		*/

		// 先将文件内容，转换成 字节数组 QByteArray
		// 返回 字节数组类型 对象
		QByteArray bt = strData.toUtf8();
		//获取文件字节长度
		QString strLength = QString::number(bt.length());

		//开始拼接数据包
		strSend = strGroupFlag + gLoginEmployeeID + talkId + "2" + strLength + "bytes" + fileName + "data_begin" + strData;
	}


	//开始向客户端，发送数据
	QByteArray dataBt;
	//设置长度，以要发送数据的长度来设置字节数组的长度
	dataBt.resize(strSend.length());
	//对数据进行赋值，调用toUtf8()转换成字节数组类型
	dataBt = strSend.toUtf8();

	//最终一步
	m_tcpClientSocket->write(dataBt);
}

void TalkWindowShell::onEmotionBtnClicked(bool)
{
	//表情窗口，点击之后，要隐藏起来
	//设置成【不可见】
	m_emotionWindow->setVisible(!m_emotionWindow->isVisible());			//如果   可见   则设置为   不可见，不可见   则设置为   可见

	//将当前控件的相对位置，转换为屏幕的绝对位置
	QPoint emotionPoint = this->mapToGlobal(QPoint(0, 0));

	emotionPoint.setX(emotionPoint.x() + 170);
	emotionPoint.setY(emotionPoint.y() + 220);
	m_emotionWindow->move(emotionPoint);					//移动

}

void TalkWindowShell::onTalkWindowItemClicked(QListWidgetItem* item)
{
	//取得窗体地址
	QWidget* talkWindowWidget = m_talkwindowItemMap.find(item).value();

	//将右边的窗体更新
	ui.rightStackedWidget->setCurrentWidget(talkWindowWidget);
}


void TalkWindowShell::onEmotionItemClicked(int emotionNum)
{
	//获取当前的聊天窗口
	//获取右边当前的部件，rightStackWidget调用currentWidget，
	//返回的指针类型是Widget部件类型，需要转换成TalkWindow* 指针类型
	TalkWindow* curTalkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());

	//判断指针是否有效
	if (curTalkWindow)
	{
		//对聊天窗口添加图片
		//表情序号传进去，进行加载
		curTalkWindow->addEmotionImage(emotionNum);
	}
}

/*****************************************************************************************************************************
	数据包格式
	文本数据包格式：群聊标志+发信息员工QQ号+收信息员工QQ号（群QQ号）+信息类型（1）+数据长度+数据
	表情数据包格式：群聊标志+发信息员工QQ号+收信息员工QQ号（群QQ号）+信息类型（0）+表情个数+images+表情名称
	文件数据包格式：群聊标志+发信息员工QQ号+收信息员工QQ号（群QQ号）+信息类型（2）+文件字节数+bytes+文件名+data_begin+文件数据

	群聊标志占1位，0表示单聊，1表示群聊
	信息类型占1位，0表示表情信息，1表示文本信息，2表示文件信息

	QQ号占5位，群QQ号占4位，数据长度占5位，表情名称占3位
	（注意：当群聊标志为1时，则数据包没有收信息员工QQ号，而是收信息群QQ号
			当群聊标志为0时，则数据包没有收信息群QQ号，而是收信息员工QQ号)

	群聊文本信息如：1100012001100005Hello  表示QQ10001向群2001发送文本信息，长度是5，数据为Hello
	单聊图片信息如：0100011000201images060 表示QQ10001向QQ10002发送1个表情60.png
	群聊文件信息如：1100052000210bytestest.txtdata_beginhelloworld
										   表示QQ10005向群2000发送文件信息，文件是test.txt，文件内容长度10，内容helloworld

	群聊文件信息解析：1 10001 2001 1 00005 Hello
	单聊图片信息解析：0 10001 10002 0 1 images  060
	群聊文件信息解析：1 10005 2000 2 10 bytes test.txt data_begin helloword

******************************************************************************************************************************/
void TalkWindowShell::processPendingData()						//原版   可以成功使用
{
	//端口中有未处理的数据
	//调用方法hasPendingDatagrams(),返回值就会告诉我们，是否有未处理的数据
	while (m_udpReceiver->hasPendingDatagrams())
	{
		// 定义一些变量，用于 解包时定位
		// 这些位置是固定的，不会发生改变，因此用const static
		const static int groupFlagWidth = 1;		//群聊标志占位
		const static int groupWidth = 4;				//群QQ号宽度
		const static int employeeWidth = 5;		//员工QQ号宽度
		const static int msgTypeWidth = 1;		//信息类型宽度
		const static int msgLengthWidth = 5;		//文本信息长度的宽度
		const static int pictureWidth = 3;			//表情图片的宽度

		//读取udp数据
		QByteArray btData;
		// 获取 即将要处理数据的大小
		btData.resize(m_udpReceiver->pendingDatagramSize());			//pendingDatagramSize()未处理数据的大小
		// 读取UDP数据（数据，大小）
		m_udpReceiver->readDatagram(btData.data(), btData.size());

		QString strData = btData.data();			// 将数据保存到 QString类型中
		QString strWindowID;		//聊天窗口ID，群聊则是群号，单聊则是员工QQ号
		QString strSendEmployeeID, strReceiveEmployeeID;		//发送及接收端的QQ号
		QString strMsg;				//数据

		int msgLen;				//数据长度
		int msgType;			//数据类型

		strSendEmployeeID = strData.mid(groupFlagWidth, employeeWidth);		//从groupFlagWidth位置开始，长度为employeeWidth

		//自己发的信息不做处理
		if (strSendEmployeeID == gLoginEmployeeID)
		{
			return;
		}

		if (btData[0] == '1')		//群聊
		{
			//群QQ号
			strWindowID = strData.mid(groupFlagWidth + employeeWidth, groupWidth);			//1（groupFlagWidth） 10001（employeeWidth） 2001 1 00005 Hello   1的宽度+10001的宽度，从2001的2开始截取groupWidth的宽度4

			QChar cMsgType = btData[groupFlagWidth + employeeWidth + groupWidth];
			if (cMsgType == '1')			//文本信息
			{
				msgType = 1;
				msgLen = strData.mid(groupFlagWidth + employeeWidth + groupWidth + msgTypeWidth, msgLengthWidth).toInt();			//提取，文本信息的长度
				strMsg = strData.mid(groupFlagWidth + employeeWidth + groupWidth + msgTypeWidth + msgLengthWidth, msgLen);			// 文本信息

				qDebug() << "[DEBUG] 全部数据：" << strData;
				qDebug() << "[DEBUG] 取文本起始="
					<< (groupFlagWidth + employeeWidth + employeeWidth + msgTypeWidth + msgLengthWidth)
					<< " 长度=" << msgLen;
				qDebug() << "[DEBUG] 解析出来的 strMsg=" << strMsg;
			}
			else if (cMsgType == '0')			//表情信息
			{
				msgType = 0;
				int posImages = strData.indexOf("images");
				strMsg = strData.right(strData.length() - posImages - QString("images").length());		//strData = "0100011000201images060"，并且 "images" 的位置在索引 12，posImages = 12，QString("images").length() = 6
																																				//strData.length() = 21，strData.right(strData.length() - posImages - QString("images").length()) 的计算过程如下：21 - 12 - 6 = 3，strData.right(3) 返回的是 "060"，即表情信息的内容。
			}
			else if (cMsgType == '2')			//文件信息
			{
				msgType = 2;
				int bytesWidth = QString("bytes").length();		// 计算 bytes 的长度
				int posBytes = strData.indexOf("bytes");			//indexOf是从0开始数，而不是1开始数
				int posData_begin = strData.indexOf("data_begin");			// data_begin，第一次出现的位置

				//文件名称
				//1.posBytes 是 strData 中 "bytes" 的起始位置，假设为 13。
				//2.bytesWidth 是 5，即 "bytes" 的长度。
				//3.posData_begin 是 strData 中 "data_begin" 的起始位置，假设为 26。
				//posBytes + bytesWidth：计算的是从 "bytes" 字符串的末尾开始的位置，也就是 posBytes + bytesWidth = 13+ 5 = 18，即 test.txt 的起始位置。
				//posData_begin - posBytes - bytesWidth：计算的是从 "bytes" 后面开始到 "data_begin" 之前的字符数。即 26 - 13 - 5 = 8，表示文件名的长度是 8 个字符。
				QString fileName = strData.mid(posBytes + bytesWidth, posData_begin - posBytes - bytesWidth); //以1 10005 2000 2 10 bytes test.txt data_begin helloword为例，
				gfileName = fileName;

				//文件内容
				//mid() 函数有多个重载版本，其中常见的两个版本是：
				/*1.mid(int position)：
					这个版本只需要一个参数 position，表示从该位置开始提取子字符串，直到字符串的结尾。
					例如：strData.mid(posData) 就是从 posData 开始提取，直到字符串的末尾。

					2.mid(int position, int length)：
					这个版本需要两个参数：position 表示开始位置，length 表示要提取的子字符串的长度。
					例如：strData.mid(5, 10) 表示从位置 5 开始，提取 10 个字符的子字符串。*/
				int dataLengthWidth;
				int posData = posData_begin + QString("data_begin").length();			//1 10005 2000 2 10 bytes test.txt data_begin helloword为例， posData则定位hello的h
				strMsg = strData.mid(posData);		// 提取数据，文件数据
				gfileData = strMsg;				// 将解析出来的 文件内容，赋值给全局变量

				//根据employeeID获取发送者姓名
				QString sender;
				int employeeID = strSendEmployeeID.toInt();
				//QSqlQuery queryGroupName(QString("SELECT employee_name FROM tab_employees WHERE employeeID = %1").arg(employeeID));
				QSqlQuery queryGroupName;
				queryGroupName.prepare("SELECT employee_name FROM tab_employees WHERE employeeID = :employeeID");
				queryGroupName.bindValue(":employeeID", employeeID);
				queryGroupName.exec();

				if (queryGroupName.first())
				{
					sender = queryGroupName.value(0).toString();
				}

				//接收文件的后续操作
				ReceiveFile* recvFile = new ReceiveFile(this);

				//用了点取消，发送返回信号
				connect(recvFile, &ReceiveFile::refuseFile, [this]()
					{
						return;
					});

				//收到xxx的信息
				//fromUtf8 方法将 UTF-8 编码的字节序列转换为 QString,fromLocal8Bit 方法将本地8位编码（通常是操作系统的默认编码）转换为 QString
				QString msgLabel = QString::fromUtf8("收到来自") + sender + QString::fromUtf8("发来的文件，是否接收？");
				//将文本字符串，设置到标签上
				recvFile->setMsg(msgLabel);
				recvFile->show();
			}
		}
		else		//单聊
		{			//索引是从 0 开始，假设 groupFlagWidth 为 4，employeeWidth 为 5，那么：表示从字符串的第 9 个字符（索引从 0 开始）开始截取
			strReceiveEmployeeID = strData.mid(groupFlagWidth + employeeWidth, employeeWidth);			//因为是单聊，所以宽度还是employeeWidth
			strWindowID = strSendEmployeeID;

			//不是发给我的信息不做处理
			// 接收者的ID 和 登陆者的ID ，不是一样的，则直接返回
			if (strReceiveEmployeeID != gLoginEmployeeID)
			{
				return;
			}

			//获取信息的类型
			QChar cMsgType = btData[groupFlagWidth + employeeWidth + employeeWidth];
			if (cMsgType == '1')			//文本信息
			{
				msgType = 1;

				//文本信息长度
				msgLen = strData.mid(groupFlagWidth + employeeWidth + employeeWidth + msgTypeWidth, msgLengthWidth).toInt();

				//文本信息
				strMsg = strData.mid(groupFlagWidth + employeeWidth + employeeWidth + msgTypeWidth + msgLengthWidth, msgLen);
			}
			else if (cMsgType == '0')			//表情信息
			{
				msgType = 0;
				int posImages = strData.indexOf("images");			//索引指向images的i这个位置，如"0100011000201images060"，索引到从第 12 个字符开始（索引从 0 开始）i是第13个位置
				int imagesWidth = QString("images").length();
				strMsg = strData.mid(posImages + imagesWidth);
			}
			else if (cMsgType == '2')			//文件信息
			{
				msgType = 2;

				int bytesWidth = QString("bytes").length();
				int posBytes = strData.indexOf("bytes");
				int data_beginWidth = QString("data_begin").length();
				int posData_begin = strData.indexOf("data_begin");

				//文件名称
				QString fileName = strData.mid(posBytes + bytesWidth, posData_begin - posBytes - bytesWidth);
				gfileName = fileName;

				//文件内容
				strMsg = strData.mid(posData_begin + data_beginWidth);		//如1 10005 2000 2 10 bytes test.txt data_begin helloword，从helloworld的h开始一直到结束
				gfileData = strMsg;

				//根据employeeID获取发送者姓名
				QString sender;
				int empID = strSendEmployeeID.toInt();			//转换成整型
				QSqlQuery querySenderName;
				querySenderName.prepare("SELECT employee_name FROM tab_employees WHERE employeeID = :empID");
				querySenderName.bindValue(":empID", empID);
				querySenderName.exec();			//执行一下SQL语句

				// 判断，数据库里 是否有数据
				if (querySenderName.first())
				{
					sender = querySenderName.value(0).toString();
				}


				//接收文件的后续操作
				ReceiveFile* recvFile = new ReceiveFile(this);

				//用了点取消，发送返回信号
				connect(recvFile, &ReceiveFile::refuseFile, [this]()
					{
						return;
					});

				//收到xxx的信息
				//fromUtf8 方法将 UTF-8 编码的字节序列转换为 QString,fromLocal8Bit 方法将本地8位编码（通常是操作系统的默认编码）转换为 QString
				QString msgLabel = QString::fromUtf8("收到来自") + sender + QString::fromUtf8("发来的文件，是否接收？");

				//将文本字符串，设置到标签上
				recvFile->setMsg(msgLabel);
				recvFile->show();
			}
		}


		//将聊天窗口设为活动得窗口
		QWidget* widget = WindowManager::getInstance()->findWindowName(strWindowID);
		if (widget)			//聊天窗口存在
		{
			qDebug() << "[DEBUG] 找到目标窗口：" << strWindowID;  // 打印目标窗口 ID

			this->setCurrentWidget(widget);

			//同步激活左侧聊天窗口
			QListWidgetItem* item = m_talkwindowItemMap.key(widget);
			item->setSelected(true);
		}
		else     //聊天窗口未打开
		{
			qDebug() << "[DEBUG] 找不到窗口，目标窗口 ID：" << strWindowID;
			return;
		}

		//文件信息另作处理
		if (msgType != 2)
		{
			int sendEmployeeID = strSendEmployeeID.toInt();
			handleReceivedMsg(sendEmployeeID, msgType, strMsg);
		}
	}
}




//void TalkWindowShell::processPendingData()			//可以运行，只不过单字符有问题，最后的一个版本
//{
//	while (m_udpReceiver->hasPendingDatagrams())
//	{
//		// 固定宽度约定
//		const static int groupFlagWidth = 1;
//		const static int groupWidth = 4;
//		const static int employeeWidth = 5;
//		const static int msgTypeWidth = 1;
//		const static int msgLengthWidth = 5;
//		const static int pictureWidth = 3;
//
//		// 读取UDP数据
//		QByteArray btData;
//		btData.resize(m_udpReceiver->pendingDatagramSize());
//		m_udpReceiver->readDatagram(btData.data(), btData.size());
//
//		// === 1) 打印原始字符串、长度 ===
//		QString strData = btData.data();
//		qDebug() << "[DEBUG] 收到原始数据 strData:" << strData
//			<< ", length=" << strData.length();
//
//		// 准备解析用的变量
//		QString strWindowID;      // 聊天窗口ID(群聊=群号,单聊=对方员工ID)
//		QString strSendEmployeeID, strReceiveEmployeeID;
//		QString strMsg;
//
//		int msgLen = 0;
//		int msgType = 0;
//
//		// === 2) 解析发送者ID ===
//		strSendEmployeeID = strData.mid(groupFlagWidth, employeeWidth);
//		// 去前导零
//		QString sendIdNoZero = removeLeadingZeros(strSendEmployeeID);
//		strSendEmployeeID = sendIdNoZero;
//
//		// 在这里打印下“发送者ID”及“自己ID”
//		QString myIdNoZero = removeLeadingZeros(gLoginEmployeeID);
//		qDebug() << "[DEBUG] 解析后发送者ID:" << strSendEmployeeID
//			<< " 本地gLoginEmployeeID:" << gLoginEmployeeID
//			<< " 去零后:" << myIdNoZero;
//
//		// === 3) 如果是自己发的消息就跳过 ===
//		if (strSendEmployeeID == myIdNoZero)
//		{
//			qDebug() << "[DEBUG] 这条消息被判定为自己发的, return";
//			return;
//		}
//
//		// === 4) 判断群聊 or 单聊 ===
//		QChar groupFlag = btData[0];
//		if (groupFlag == '1')
//		{
//			// ---- 群聊 ----
//			strWindowID = strData.mid(groupFlagWidth + employeeWidth, groupWidth);
//
//			// 打印群号(窗口ID)
//			qDebug() << "[DEBUG] [群聊] strWindowID(群号):" << strWindowID;
//
//			// 解析类型
//			QChar cMsgType = btData[groupFlagWidth + employeeWidth + groupWidth];
//			qDebug() << "[DEBUG] 群聊消息类型 cMsgType:" << cMsgType;
//
//			if (cMsgType == '1') // 文本
//			{
//				msgType = 1;
//				msgLen = strData.mid(groupFlagWidth + employeeWidth + groupWidth + msgTypeWidth,
//					msgLengthWidth).toInt();
//				strMsg = strData.mid(groupFlagWidth + employeeWidth + groupWidth + msgTypeWidth + msgLengthWidth,
//					msgLen);
//
//				qDebug() << "[DEBUG] [群聊] 文本, msgLen=" << msgLen
//					<< " strMsg=" << strMsg;
//			}
//			else if (cMsgType == '0') // 表情
//			{
//				msgType = 0;
//				int posImages = strData.indexOf("images");
//				if (posImages >= 0)
//				{
//					strMsg = strData.right(strData.length() - posImages - QString("images").length());
//					qDebug() << "[DEBUG] [群聊] 表情, strMsg=" << strMsg;
//				}
//				else
//				{
//					qDebug() << "[DEBUG] [群聊] 表情, 但没找到 'images' 字符串!";
//				}
//			}
//			else if (cMsgType == '2') // 文件
//			{
//				msgType = 2;
//				int bytesWidth = QString("bytes").length();
//				int posBytes = strData.indexOf("bytes");
//				int posData_begin = strData.indexOf("data_begin");
//				if (posBytes >= 0 && posData_begin > posBytes)
//				{
//					QString fileName = strData.mid(posBytes + bytesWidth,
//						posData_begin - posBytes - bytesWidth);
//					strMsg = strData.mid(posData_begin + QString("data_begin").length());
//					qDebug() << "[DEBUG] [群聊] 文件, fileName=" << fileName
//						<< " fileData=" << strMsg;
//				}
//				else
//				{
//					qDebug() << "[DEBUG] [群聊] 文件, 解析 'bytes'/'data_begin' 失败!";
//				}
//			}
//		}
//		else
//		{
//			// ---- 单聊 ----
//			// 接收者ID(去前导零)
//			strReceiveEmployeeID = strData.mid(groupFlagWidth + employeeWidth, employeeWidth);
//			strReceiveEmployeeID = removeLeadingZeros(strReceiveEmployeeID);
//
//			// 单聊窗口ID = 发送者ID (对方)
//			strWindowID = strSendEmployeeID;
//
//			// 打印单聊发送者/接收者
//			qDebug() << "[DEBUG] [单聊] 发送者ID=" << strSendEmployeeID
//				<< " 接收者ID=" << strReceiveEmployeeID
//				<< " => 窗口ID=" << strWindowID;
//
//			// 消息类型
//			QChar cMsgType = btData[groupFlagWidth + employeeWidth + employeeWidth];
//			qDebug() << "[DEBUG] [单聊] msgType cMsgType:" << cMsgType;
//
//			if (cMsgType == '1') // 文本
//			{
//				msgType = 1;
//				msgLen = strData.mid(groupFlagWidth + employeeWidth + employeeWidth + msgTypeWidth,
//					msgLengthWidth).toInt();
//				strMsg = strData.mid(groupFlagWidth + employeeWidth + employeeWidth + msgTypeWidth + msgLengthWidth,
//					msgLen);
//
//				qDebug() << "[DEBUG] [单聊] 文本, msgLen=" << msgLen
//					<< " strMsg=" << strMsg;
//			}
//			else if (cMsgType == '0') // 表情
//			{
//				msgType = 0;
//				int posImages = strData.indexOf("images");
//				int imagesWidth = QString("images").length();
//				if (posImages >= 0)
//				{
//					strMsg = strData.mid(posImages + imagesWidth);
//					qDebug() << "[DEBUG] [单聊] 表情, strMsg=" << strMsg;
//				}
//				else
//				{
//					qDebug() << "[DEBUG] [单聊] 表情, 但没找到 'images' 字符串!";
//				}
//			}
//			else if (cMsgType == '2') // 文件
//			{
//				msgType = 2;
//				int bytesWidth = QString("bytes").length();
//				int posBytes = strData.indexOf("bytes");
//				int data_beginWidth = QString("data_begin").length();
//				int posData_begin = strData.indexOf("data_begin");
//				if (posBytes >= 0 && posData_begin > posBytes)
//				{
//					QString fileName = strData.mid(posBytes + bytesWidth,
//						posData_begin - posBytes - bytesWidth);
//					strMsg = strData.mid(posData_begin + data_beginWidth);
//					qDebug() << "[DEBUG] [单聊] 文件, fileName=" << fileName
//						<< " fileData=" << strMsg;
//				}
//				else
//				{
//					qDebug() << "[DEBUG] [单聊] 文件, 解析 'bytes'/'data_begin' 失败!";
//				}
//			}
//
//			// 如果接收者不是自己，就返回
//			if (strReceiveEmployeeID != myIdNoZero)
//			{
//				qDebug() << "[DEBUG] [单聊] 这条消息不是发给我(ID=" << myIdNoZero << ") => return";
//				return;
//			}
//		}
//
//		// ============ 找到对应窗口并显示消息 ============
//		qDebug() << "[DEBUG] 最终 窗口ID=" << strWindowID
//			<< " msgType=" << msgType
//			<< " strMsg=" << strMsg;
//
//		QWidget* widget = WindowManager::getInstance()->findWindowName(strWindowID);
//		if (!widget)
//		{
//			qDebug() << "[DEBUG] [processPendingData] 未找到窗口" << strWindowID << ", return";
//			return;
//		}
//		else
//		{
//			// 同步激活
//			setCurrentWidget(widget);
//			QListWidgetItem* item = m_talkwindowItemMap.key(widget);
//			if (item)
//			{
//				item->setSelected(true);
//			}
//		}
//
//		// 文件信息另作处理，这里只处理文本/表情
//		if (msgType != 2)
//		{
//			int sendEmployeeID = strSendEmployeeID.toInt();
//			qDebug() << "[DEBUG] 调用 handleReceivedMsg(...)";
//			handleReceivedMsg(sendEmployeeID, msgType, strMsg);
//		}
//	} // end while
//}






