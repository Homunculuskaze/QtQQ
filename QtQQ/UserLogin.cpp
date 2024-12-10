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
	setAttribute(Qt::WA_DeleteOnClose);
	iniTitleBar();			//初始化标题
	setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");

	loadStyleSheet("UserLogin");
	initControl();
}

UserLogin::~UserLogin()
{}

void UserLogin::initControl()
{
	QLabel* headLabel = new QLabel(this);
	headLabel->setFixedSize(68, 68);

	QPixmap pix(":/Resources/MainWindow/head_mask.png");
	headLabel->setPixmap(getRoundImage(QPixmap(":/Resources/MainWindow/girl.png"), pix, headLabel->size()));
	headLabel->move(width() / 2 - 34, ui.titleWidget->height() - 34);

	//因为是空的，所以不能直接设置，要先调用函数转换
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


	if (db.open())
	{
		return true;
	}
	else
	{
		return false;
	}



	//// 设置插件路径，以确保 Qt 找到 MySQL 驱动
	//QCoreApplication::addLibraryPath("C:/Qt/6.6.3/msvc2019_64/plugins");

	//// 打印支持的数据库驱动
	//qDebug() << "Available database drivers:" << QSqlDatabase::drivers();
	//qDebug() << QSqlDatabase::drivers();

	//if (!QSqlDatabase::isDriverAvailable("QMYSQL")) {
	//	qDebug() << "MySQL driver not available!";
	//	// 可以尝试加载该驱动
	//	QSqlDatabase::addDatabase("QMYSQL");
	//}
	//QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	//db.setDatabaseName("qtqq");    // 设置数据库名称
	//db.setHostName("localhost");   // 主机名
	//db.setUserName("root");        // 数据库用户名
	//db.setPassword("wcl62426327"); // 数据库密码
	//db.setPort(3306);              // 数据库端口

	//// 打开数据库连接并调试错误信息
	//if (db.open())
	//{
	//	qDebug() << "Database connection succeeded.";
	//	return true;
	//}
	//else
	//{
	//	qDebug() << "Database connection failed: " << db.lastError().text();
	//	return false;
	//}
}

//bool UserLogin::veryfyAccountCode(bool& isAccountLogin, QString& strAccount)

bool UserLogin::veryfyAccountCode()			//原代码基础修改可使用版本
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
			//gLoginEmployeeID = strAccountInput;

			//isAccountLogin = false;				//QQ号登录成功，这里变false
			//strAccount = strAccountInput;		//QQ号赋值
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
			//gLoginEmployeeID = queryAccount.value(1).toString();

			//strAccount = strAccountInput;			//账号赋值
			//isAccountLogin = true;						//账号登录，就是ture
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

//bool UserLogin::veryfyAccountCode()		// 正确的验证方法2
//{
//	// 去除多余空格，提高用户输入的可靠性
//	QString strAccountInput = ui.editUserAccount->text().trimmed();
//	QString strCodeInput = ui.editPassword->text().trimmed();
//
//	// 查询员工号（QQ号）对应的密码
//	QString strSqlCode = QString("SELECT code FROM tab_accounts WHERE employeeID = %1").arg(strAccountInput.toInt());
//	QSqlQuery queryEmployeeID;
//
//	// 调试：打印 SQL 查询语句
//	qDebug() << "Executing SQL Query for employeeID: " << strSqlCode;
//
//	// 执行 SQL 查询
//	if (!queryEmployeeID.exec(strSqlCode)) {
//		// 查询失败，输出错误信息
//		qDebug() << "Query failed: " << queryEmployeeID.lastError().text();
//		return false;
//	}
//
//	// 判断是否有返回结果
//	if (queryEmployeeID.first()) {
//		// 获取数据库中查询到的密码
//		QString strCode = queryEmployeeID.value(0).toString();
//
//		// 调试：输出数据库密码和用户输入密码
//		qDebug() << "Database password: " << strCode;
//		qDebug() << "Input password: " << strCodeInput;
//
//		// 检查密码是否匹配
//		if (strCode == strCodeInput) {
//			qDebug() << "Password matched!";
//			return true;
//		}
//		else {
//			qDebug() << "Password mismatched!";
//			return false;
//		}
//	}
//	else {
//		qDebug() << "No matching record found for employeeID.";
//	}
//
//	// 查询账号（字符串）对应的密码
//	strSqlCode = QString("SELECT code, employeeID FROM tab_accounts WHERE account = '%1'").arg(strAccountInput);
//	QSqlQuery queryAccount;
//
//	// 调试：打印 SQL 查询语句
//	qDebug() << "Executing SQL Query for account: " << strSqlCode;
//
//	// 执行 SQL 查询
//	if (!queryAccount.exec(strSqlCode)) {
//		// 查询失败，输出错误信息
//		qDebug() << "Query failed: " << queryAccount.lastError().text();
//		return false;
//	}
//
//	// 判断是否有返回结果
//	if (queryAccount.first()) {
//		// 获取数据库中查询到的密码
//		QString strCode = queryAccount.value(0).toString();
//
//		// 调试：输出数据库密码和用户输入密码
//		qDebug() << "Database password: " << strCode;
//		qDebug() << "Input password: " << strCodeInput;
//
//		// 检查密码是否匹配
//		if (strCode == strCodeInput) {
//			qDebug() << "Password matched!";
//			return true;
//		}
//		else {
//			qDebug() << "Password mismatched!";
//			return false;
//		}
//	}
//	else {
//		qDebug() << "No matching record found for account.";
//	}
//
//	// 所有验证均未通过
//	return false;
//}


//bool UserLogin::veryfyAccountCode()					//能正常使用版本1
//{
//	// 去除多余空格，提高用户输入的可靠性
//	QString strAccountInput = ui.editUserAccount->text().trimmed();
//	QString strCodeInput = ui.editPassword->text().trimmed();
//
//	// 查询员工号（QQ号）对应的密码
//	QString strSqlCode = QString("SELECT code FROM tab_accounts WHERE employeeID = %1").arg(strAccountInput.toInt());
//	QSqlQuery queryEmployeeID;
//
//	// 调试：打印 SQL 查询语句
//	qDebug() << "Executing SQL Query for employeeID: " << strSqlCode;
//
//	// 执行 SQL 查询
//	if (!queryEmployeeID.exec(strSqlCode)) {
//		// 查询失败，输出错误信息
//		qDebug() << "Query failed: " << queryEmployeeID.lastError().text();
//		return false;
//	}
//
//	// 判断是否有返回结果
//	if (queryEmployeeID.first()) {
//		// 获取数据库中查询到的密码
//		QString strCode = queryEmployeeID.value(0).toString();
//
//		// 调试：输出数据库密码和用户输入密码
//		qDebug() << "Database password: " << strCode;
//		qDebug() << "Input password: " << strCodeInput;
//
//		// 检查密码是否匹配
//		if (strCode == strCodeInput) {
//			qDebug() << "Password matched!";
//			return true;
//		}
//		else {
//			qDebug() << "Password mismatched!";
//			return false;
//		}
//	}
//	else {
//		// 无匹配记录
//		qDebug() << "No matching record found for employeeID.";
//	}
//
//	// 查询账号（字符串）对应的密码
//	strSqlCode = QString("SELECT code, employeeID FROM tab_accounts WHERE account = '%1'").arg(strAccountInput);
//	QSqlQuery queryAccount;
//
//	// 调试：打印 SQL 查询语句
//	qDebug() << "Executing SQL Query for account: " << strSqlCode;
//
//	// 执行 SQL 查询
//	if (!queryAccount.exec(strSqlCode)) {
//		// 查询失败，输出错误信息
//		qDebug() << "Query failed: " << queryAccount.lastError().text();
//		return false;
//	}
//
//	// 判断是否有返回结果
//	if (queryAccount.first()) {
//		// 获取数据库中查询到的密码
//		QString strCode = queryAccount.value(0).toString();
//
//		// 调试：输出数据库密码和用户输入密码
//		qDebug() << "Database password: " << strCode;
//		qDebug() << "Input password: " << strCodeInput;
//
//		// 检查密码是否匹配
//		if (strCode == strCodeInput) {
//			qDebug() << "Password matched!";
//			return true;
//		}
//		else {
//			qDebug() << "Password mismatched!";
//			return false;
//		}
//	}
//	else {
//		// 无匹配记录
//		qDebug() << "No matching record found for account.";
//	}
//
//	// 所有验证均未通过
//	return false;
//}


void UserLogin::onLoginBtnClicked()
{
	//bool isAccountLogin;
	////有可能是账号，或者QQ号
	////isAccountLogin,是true，就是账号
	////isAccountLogin,是false，就是QQ号
	//QString strAccount;

	//验证
	//if (!veryfyAccountCode(isAccountLogin, strAccount))
	if (!veryfyAccountCode())
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
	//CCMainWindow* mainWindow = new CCMainWindow(strAccount, isAccountLogin);
	CCMainWindow* mainWindow = new CCMainWindow;
	mainWindow->show();
}