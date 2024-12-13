#include "UserLogin.h"
#include "CCMainWindow.h"

#include<QMessageBox>
#include<qdebug.h>
#include<QtSql\qsqlquery.h>
#include<QSqlDatabase>
#include <QSqlError>
//登录者的QQ号(员工号)
QString gLoginEmployeeID;

UserLogin::UserLogin(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	//将构造函数完善一下
	setAttribute(Qt::WA_DeleteOnClose);			//确保窗口关闭时资源被释放。
	iniTitleBar();			//初始化标题
	setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");

	loadStyleSheet("UserLogin");			//初始化样式表
	initControl();
}

UserLogin::~UserLogin()
{}

void UserLogin::initControl()
{
	QLabel* headLabel = new QLabel(this);
	headLabel->setFixedSize(68, 68);

	QPixmap pix(":/Resources/MainWindow/head_mask.png");
	//headLabel->setPixmap(getRoundImage(QPixmap(":/Resources/MainWindow/girl.png"), pix, headLabel->size()));
	headLabel->setPixmap(getRoundImage(QPixmap(":/Resources/MainWindow/app/logo.ico"), pix, headLabel->size()));
	headLabel->move(width() / 2 - 34, ui.titleWidget->height() - 34);

	//因为是空的，所以不能直接设置，要先调用函数转换
	//这行代码只是将登录按钮的 clicked 信号和槽函数 onLoginBtnClicked 关联起来。
	//此时不会执行槽函数，只有用户实际点击登录按钮时才会触发槽函数的执行。
	connect(ui.loginBtn, &QPushButton::clicked, this, &UserLogin::onLoginBtnClicked);

	//判断连接数据库，是否成功
	if (!connectMySql())				//如果失败
	{	
		/*QMessageBox::information(NULL, QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("连接数据库失败"));*/
		QMessageBox::information(NULL, QString::fromUtf8("提示"),
			QString::fromUtf8("连接数据库失败"));
		close();			//直接关闭
	}

}


bool UserLogin::connectMySql()
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
	//QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setDatabaseName("qtqq");		//设置数据库名称
	db.setHostName("localhost");			//主机名
	db.setUserName("root");			//数据库，用户名
	db.setPassword("wcl62426327");			//数据库，密码
	db.setPort(3306);							//数据库，端口


	/*if (db.open())
	{
		return true;
	}
	else
	{
		return false;
	}*/

	// 打开连接
	if (db.open()) {
		qDebug() << "Database connected successfully.";
		return true;
	}
	else {
		qDebug() << "Database connection failed:" << db.lastError().text();
		return false;
	}

}

bool UserLogin::veryfyAccountCode(bool& isAccountLogin, QString& strAccount)	
{
	//QString strAccountInput = ui.editUserAccount->text();
	//QString strCodeInput = ui.editPassword->text();
	QString strAccountInput = ui.editUserAccount->text().trimmed();
	QString strCodeInput = ui.editPassword->text().trimmed();


	//输入员工号，QQ号
	//关键字用大写，可读性好一些
	//在数据库里查询，通过employeeID查询
	//[SELECT code FROM tab_accounts WHERE employeeID = 10001]
	QString strSqlCode = QString("SELECT code FROM tab_accounts WHERE employeeID = %1").arg(strAccountInput.toInt());		//%1用strAccountInput替换

	//下面两句错误
	//QSqlQuery queryEmployeeID(strSqlCode);			//这种写法创建了 QSqlQuery 对象并在构造时直接传入了 SQL 查询字符串 strSqlCode。但这种方式在 Qt 中并不推荐，因为 QSqlQuery 的构造函数不接受查询字符串作为参数。在这种情况下，可能会导致 SQL 查询的执行出现意料之外的行为。
	//queryEmployeeID.exec();						//执行

	QSqlQuery queryEmployeeID;
	// 执行 SQL 查询
	if (!queryEmployeeID.exec(strSqlCode)) {
		// 查询失败，输出错误信息
		qDebug() << "Query failed: " << queryEmployeeID.lastError().text();
		return false;
	}

	//调用first方法，让它指向结果集的第一条
	//判断是否有结果
	if (queryEmployeeID.first())
	{
		//数据库中，QQ号对应的密码
		//查询数据库字段，从第0列（code列）开始。如果查询的为code和employeeID则第0列为code，第一列为employeeID
		//把数据库中的密码值拿出来，转成字符串
		QString strCode = queryEmployeeID.value(0).toString();

		if (strCode == strCodeInput)
		{
			////用账号登录的话，密码正确，保存账号给全局变量
			gLoginEmployeeID = strAccountInput;

			isAccountLogin = false;				//QQ号登录成功，这里变false
			strAccount = strAccountInput;		//QQ号赋值
			return  true;
		}
		else
		{
			return false;		//密码错误
		}
	}

	//账号登录
	//用户输入的账号
	//在数据库里，如果想要查询字符串，就得加【''】单引号
	//通过account查询
	//【SELECT code,employeeID FROM tab_accounts WHERE account = 'ren1'】
	strSqlCode = QString("SELECT code,employeeID FROM tab_accounts WHERE account = '%1'").arg(strAccountInput);			//输入的内容是字符串，所以用了单引号'%1'

	//下面两句错误写法
	//QSqlQuery queryAccount(strSqlCode);
	//queryAccount.exec();				//执行


	QSqlQuery queryAccount;
	// 执行 SQL 查询
	if (!queryAccount.exec(strSqlCode)) {
		// 查询失败，输出错误信息
		qDebug() << "Query failed: " << queryAccount.lastError().text();
		return false;
	}

	if (queryAccount.first())				//指向结果集第一条
	{
		//value(0),是密码code
		QString strCode = queryAccount.value(0).toString();

		if (strCode == strCodeInput)
		{
			//用账号登录的话，密码正确，保存账号给全局变量
			//value(1),才是索要的QQ号employeeID
			gLoginEmployeeID = queryAccount.value(1).toString();

			strAccount = strAccountInput;			//账号赋值
			isAccountLogin = true;						//账号登录，就是ture
			return true;
		}
		else
		{
			return false;
		}
	}

	//如果输入都有问题，就直接返回false
	return false;
}




//bool UserLogin::veryfyAccountCode()			//原代码基础修改可使用版本
//{
//	//QString strAccountInput = ui.editUserAccount->text();
//	//QString strCodeInput = ui.editPassword->text();
//	QString strAccountInput = ui.editUserAccount->text().trimmed();
//	QString strCodeInput = ui.editPassword->text().trimmed();
//
//
//	//输入员工号，QQ号
//	//关键字用大写，可读性好一些
//	//在数据库里查询，通过employeeID查询
//	//[SELECT code FROM tab_accounts WHERE employeeID = 10001]
//	QString strSqlCode = QString("SELECT code FROM tab_accounts WHERE employeeID = %1").arg(strAccountInput.toInt());		//%1用strAccountInput替换
//
//	//下面两句错误
//	//QSqlQuery queryEmployeeID(strSqlCode);			//这种写法创建了 QSqlQuery 对象并在构造时直接传入了 SQL 查询字符串 strSqlCode。但这种方式在 Qt 中并不推荐，因为 QSqlQuery 的构造函数不接受查询字符串作为参数。在这种情况下，可能会导致 SQL 查询的执行出现意料之外的行为。
//	//queryEmployeeID.exec();						//执行
//
//	QSqlQuery queryEmployeeID;
//	// 执行 SQL 查询
//	if (!queryEmployeeID.exec(strSqlCode)) {
//		// 查询失败，输出错误信息
//		qDebug() << "Query failed: " << queryEmployeeID.lastError().text();
//		return false;
//	}
//
//	//调用first方法，让它指向结果集的第一条
//	//判断是否有结果
//	if (queryEmployeeID.first())
//	{
//		//数据库中，QQ号对应的密码
//		//查询数据库字段，从第0列（code列）开始。如果查询的为code和employeeID则第0列为code，第一列为employeeID
//		//把数据库中的密码值拿出来，转成字符串
//		QString strCode = queryEmployeeID.value(0).toString();
//
//		if (strCode == strCodeInput)
//		{
//			////用账号登录的话，密码正确，保存账号给全局变量
//			//gLoginEmployeeID = strAccountInput;
//
//			//isAccountLogin = false;				//QQ号登录成功，这里变false
//			//strAccount = strAccountInput;		//QQ号赋值
//			return  true;
//		}
//		else
//		{
//			return false;		//密码错误
//		}
//	}
//
//	//账号登录
//	//用户输入的账号
//	//在数据库里，如果想要查询字符串，就得加【''】单引号
//	//通过account查询
//	//【SELECT code,employeeID FROM tab_accounts WHERE account = 'ren1'】
//	strSqlCode = QString("SELECT code,employeeID FROM tab_accounts WHERE account = '%1'").arg(strAccountInput);			//输入的内容是字符串，所以用了单引号'%1'
//
//	//下面两句错误写法
//	//QSqlQuery queryAccount(strSqlCode);
//	//queryAccount.exec();				//执行
//
//
//	QSqlQuery queryAccount;
//	// 执行 SQL 查询
//	if (!queryAccount.exec(strSqlCode)) {
//		// 查询失败，输出错误信息
//		qDebug() << "Query failed: " << queryAccount.lastError().text();
//		return false;
//	}
//
//	if (queryAccount.first())				//指向结果集第一条
//	{
//		//value(0),是密码code
//		QString strCode = queryAccount.value(0).toString();				
//
//		if (strCode == strCodeInput)
//		{
//			//用账号登录的话，密码正确，保存账号给全局变量
//			//value(1),才是索要的QQ号employeeID
//			//gLoginEmployeeID = queryAccount.value(1).toString();
//
//			//strAccount = strAccountInput;			//账号赋值
//			//isAccountLogin = true;						//账号登录，就是ture
//			return true;
//		}
//		else
//		{
//			return false;
//		}
//	}
//
//	//如果输入都有问题，就直接返回false
//	return false;
//}



void UserLogin::onLoginBtnClicked()
{
	bool isAccountLogin;
	//有可能是账号，或者QQ号
	//isAccountLogin,是true，就是账号
	//isAccountLogin,是false，就是QQ号
	QString strAccount;

	//验证
	if (!veryfyAccountCode(isAccountLogin, strAccount))
	{
		/*QMessageBox::information(NULL, QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("账号或密码有误！"));*/
		QMessageBox::information(NULL, QString::fromUtf8("提示"),
			QString::fromUtf8("账号或密码有误！"));

		//密码错误，就把账号和密码清空
		ui.editUserAccount->setText("");
		ui.editPassword->setText("");
		return;
	}

	////更新登录状态为"登录"
	//QString strSqlStatus = QString("UPDATE tab_employee SET online = 2 WHERE employeeID = %1").arg(gLoginEmployeeID);
	//QSqlQuery queryStatus(strSqlStatus);
	//queryStatus.exec();

	//账号正确，再显示窗口
	close();
	CCMainWindow* mainWindow = new CCMainWindow(strAccount, isAccountLogin);
	mainWindow->show();
}