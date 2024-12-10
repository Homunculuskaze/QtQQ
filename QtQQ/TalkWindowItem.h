#pragma once

#include <QWidget>
#include "ui_TalkWindowItem.h"

class TalkWindowItem : public QWidget
{
	Q_OBJECT

public:
	TalkWindowItem(QWidget *parent = nullptr);
	~TalkWindowItem();
	void setHeadPixmap(const QString& pixmap);
	void setMsgLabelContent(const QString& msg);
	QString getMsgLabelText();				//获取在跟谁聊天，返回字符串


private:
	void enterEvent(QEnterEvent* event);				//进入事件
	void leaveEvent(QEnterEvent* event);				//离开事件
	void resizeEvent(QResizeEvent* event);			//尺寸进行resize

private:
	void initControl();

signals:
	void signalCloseClicked();				//关闭信号
private:
	Ui::TalkWindowItemClass ui;
};
