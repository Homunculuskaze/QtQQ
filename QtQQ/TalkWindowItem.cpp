#include "TalkWindowItem.h"
#include "CommonUtils.h"
TalkWindowItem::TalkWindowItem(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	initControl();
}

TalkWindowItem::~TalkWindowItem()
{}

void TalkWindowItem::setHeadPixmap(const QString & pixmap)
{
	QPixmap mask = QPixmap(":/Resources/MainWindow/head_mask.png");
	QPixmap head = QPixmap(":/Resources/MainWindow/girl.png");

	const QPixmap& headpixmap = CommonUtils::getRoundImage(head, mask, ui.headlabel->size());

	ui.headlabel->setPixmap(headpixmap);			//����ͷ��
}

void TalkWindowItem::setMsgLabelContent(const QString& msg)
{
	ui.msgLabel->setText(msg);
}

QString TalkWindowItem::getMsgLabelText()
{
	return ui.msgLabel->text();
}

void TalkWindowItem::enterEvent(QEnterEvent* event)
{
	ui.tclosebtn->setVisible(true);
	__super::enterEvent(event);			//����һЩ������Ĭ�ϲ���
}

void TalkWindowItem::leaveEvent(QEnterEvent* event)
{
	ui.tclosebtn->setVisible(false);
	__super::enterEvent(event);				//����һЩ�����ģ�Ĭ�ϵĲ���
}

void TalkWindowItem::resizeEvent(QResizeEvent* event)
{
	__super::resizeEvent(event);					//����һЩ�����ģ�Ĭ�ϲ���
}

void TalkWindowItem::initControl()
{
	//����տ�ʼ��ʱ�򣬶��ǲ��ɼ��ģ�������false
	//setVisible(),�����Ƿ�ɼ���true�ɼ���false���ɼ�
	ui.tclosebtn->setVisible(false);

	//�������رհ�ť���ұߵ���Ϣ���촰��ҲҪ�ر�
	connect(ui.tclosebtn, SIGNAL(clicked(bool)), this, SIGNAL(signalCloseClicked()));
}
