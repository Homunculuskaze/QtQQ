#include "UserLogin.h"
#include "CCMainWindow.h"

#include<QMessageBox>
#include<qdebug.h>
#include<QtSql\qsqlquery.h>

//��¼�ߵ�QQ��(Ա����)
QString gLoginEmployeeID;

UserLogin::UserLogin(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);

	//�����캯������һ��
	setAttribute(Qt::WA_DeleteOnClose);
	iniTitleBar();			//��ʼ������
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

	//��Ϊ�ǿյģ����Բ���ֱ�����ã�Ҫ�ȵ��ú���ת��
	connect(ui.loginBtn, &QPushButton::clicked, this, &UserLogin::onLoginBtnClicked);

	////�ж��������ݿ⣬�Ƿ�ɹ�
	//if (!connectMySql())				//���ʧ��
	//{
	//	QMessageBox::information(NULL, QString::fromLocal8Bit("��ʾ"),
	//		QString::fromLocal8Bit("�������ݿ�ʧ��"));

	//	close();			//ֱ�ӹر�
	//}

}

//bool UserLogin::connectMySql()
//{
//	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
//	db.setDatabaseName("QTQQ");		//�������ݿ�����
//	db.setHostName("QTQQ");			//������
//	db.setUserName("long");			//���ݿ⣬�û���
//	db.setPassword("qq421962");			//���ݿ⣬����
//	db.setPort(3306);							//���ݿ⣬�˿�
//
//
//	if (db.open())
//	{
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//}

//bool UserLogin::veryfyAccountCode(bool& isAccountLogin, QString& strAccount)
//{
//	QString strAccountInput = ui.editUserAccount->text();
//	QString strCodeInput = ui.editPassword->text();
//
//	//����Ա���ţ�QQ��
//	//�ؼ����ô�д���ɶ��Ժ�һЩ
//	//�����ݿ����ѯ��ͨ��employeeID��ѯ
//	//[SELECT code FROM tab_accounts WHERE employeeID = 10001]
//	QString strSqlCode = QString("SELECT code FROM tab_accounts WHERE employeeID = %1").arg(strAccountInput);
//	QSqlQuery queryEmployeeID(strSqlCode);
//	queryEmployeeID.exec();						//ִ��
//
//	//����first����������ָ�������ĵ�һ��
//	//�ж��Ƿ��н��
//	if (queryEmployeeID.first())
//	{
//		//���ݿ��У�QQ�Ŷ�Ӧ������
//		//��ѯ���ݿ��ֶΣ��ӵ�0�п�ʼ
//		//�����ݿ��е�����ֵ�ó�����ת���ַ���
//		QString strCode = queryEmployeeID.value(0).toString();
//
//		if (strCode == strCodeInput)
//		{
//			//���˺ŵ�¼�Ļ���������ȷ�������˺Ÿ�ȫ�ֱ���
//			gLoginEmployeeID = strAccountInput;
//
//			isAccountLogin = false;				//QQ�ŵ�¼�ɹ��������false
//			strAccount = strAccountInput;		//QQ�Ÿ�ֵ
//			return  true;
//		}
//		else
//		{
//			return false;		//�������
//		}
//	}
//
//	//�˺ŵ�¼
//	//�û�������˺�
//	//�����ݿ�������Ҫ��ѯ�ַ������͵üӡ�''��������
//	//ͨ��account��ѯ
//	//��SELECT code,employeeID FROM tab_accounts WHERE account = 'ren1'��
//	strSqlCode = QString("SELECT code,employeeID FROM tab_accounts WHERE account = '%1'").arg(strAccountInput);
//	QSqlQuery queryAccount(strSqlCode);
//	queryAccount.exec();
//	if (queryAccount.first())
//	{
//		//value(0),������
//		QString strCode = queryAccount.value(0).toString();
//
//		if (strCode == strCodeInput)
//		{
//			//���˺ŵ�¼�Ļ���������ȷ�������˺Ÿ�ȫ�ֱ���
//			//value(1),������Ҫ��QQ��
//			gLoginEmployeeID = queryAccount.value(1).toString();
//
//			strAccount = strAccountInput;			//�˺Ÿ�ֵ
//			isAccountLogin = true;						//�˺ŵ�¼������ture
//			return true;
//		}
//		else
//		{
//			return false;
//		}
//	}
//
//	//������붼�����⣬��ֱ�ӷ���false
//	return false;
//}

void UserLogin::onLoginBtnClicked()
{
	//bool isAccountLogin;
	////�п������˺ţ�����QQ��
	////isAccountLogin,��true�������˺�
	////isAccountLogin,��false������QQ��
	//QString strAccount;

	////��֤
	//if (!veryfyAccountCode(isAccountLogin, strAccount))
	//{
	//	QMessageBox::information(NULL, QString::fromLocal8Bit("��ʾ"),
	//		QString::fromLocal8Bit("�˺Ż���������"));

	//	//������󣬾Ͱ��˺ź��������
	//	ui.editUserAccount->setText("");
	//	ui.editPassword->setText("");
	//	return;
	//}

	////���µ�¼״̬Ϊ"��¼"
	//QString strSqlStatus = QString("UPDATE tab_employee SET online = 2 WHERE employeeID = %1").arg(gLoginEmployeeID);
	//QSqlQuery queryStatus(strSqlStatus);
	//queryStatus.exec();

	//�˺���ȷ������ʾ����
	close();
	//CCMainWindow* mainWindow = new CCMainWindow(strAccount, isAccountLogin);
	CCMainWindow* mainWindow = new CCMainWindow;
	mainWindow->show();
}