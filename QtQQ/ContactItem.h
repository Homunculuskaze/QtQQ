#pragma once

#include <QWidget>
#include "ui_ContactItem.h"

class ContactItem : public QWidget
{
	Q_OBJECT

public:
	ContactItem(QWidget *parent = nullptr);
	~ContactItem();

	void setUserName(const QString& userName);
	void setSignName(const QString& signName);
	void setHeadPixmap(const QPixmap& headPath);
	QString getUserName()const;
	QSize getHeadLabelSize()const;				//头像尺寸,const防止意外的修改

private:
	void initControl();									//初始化控件
private:
	Ui::ContactItem ui;
};
