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
	QString getMsgLabelText();				//��ȡ�ڸ�˭���죬�����ַ���


private:
	void enterEvent(QEnterEvent* event);				//�����¼�
	void leaveEvent(QEnterEvent* event);				//�뿪�¼�
	void resizeEvent(QResizeEvent* event);			//�ߴ����resize

private:
	void initControl();

signals:
	void signalCloseClicked();				//�ر��ź�
private:
	Ui::TalkWindowItemClass ui;
};
