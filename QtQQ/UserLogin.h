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
	bool connectMySql();		//连接数据库，是否成功

	//验证账号与密码
	//strAccount,不一定是账号，有可能是QQ号
	//因为要获取两个参数的值，所以用引用方式传参
	//bool veryfyAccountCode(bool& isAccountLogin, QString& strAccount);
	bool veryfyAccountCode();
private:
	Ui::UserLogin ui;
};
