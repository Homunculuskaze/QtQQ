#pragma once

#include "BasicWindow.h"
#include "ui_UserLogin.h"

class UserLogin : public BasicWindow
{
	Q_OBJECT

public:
	UserLogin(QWidget *parent = nullptr);
	~UserLogin();

private slots:
	void onLoginBtnClicked();

private:
	void initControl();
	bool connectMySql();		//�������ݿ⣬�Ƿ�ɹ�

	//��֤�˺�������
	//strAccount,��һ�����˺ţ��п�����QQ��
	//��ΪҪ��ȡ����������ֵ�����������÷�ʽ����
	//bool veryfyAccountCode(bool& isAccountLogin, QString& strAccount);
	bool veryfyAccountCode();
private:
	Ui::UserLogin ui;
};
