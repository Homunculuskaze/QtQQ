#pragma once

#include <QWidget>
#include "BasicWindow.h"
#include "ui_ReceiveFile.h"

class ReceiveFile : public BasicWindow
{
	Q_OBJECT

public:
	ReceiveFile(QWidget *parent = nullptr);				// 默认无父部件.创建 recvFile 时未显式指定父部件（例如 new ReceiveFile()），则 recvFile 的父部件为 nullptr，即没有父窗口
	~ReceiveFile();



	//信息根据传入的字符串进行设置
	void setMsg(QString& msgLabel);

signals:
	void refuseFile();				//拒绝

private slots:
	void on_okBtn_clicked();				//确定
	void on_cancelBtn_clicked();					//取消
private:
	Ui::ReceiveFileClass ui;
};
