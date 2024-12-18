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

void TalkWindowItem::setHeadPixmap(const QPixmap& pixmap)
{
	QPixmap mask = QPixmap(":/Resources/MainWindow/head_mask.png");
	//QPixmap head = QPixmap(":/Resources/MainWindow/girl.png");

	const QPixmap& headpixmap = CommonUtils::getRoundImage(pixmap, mask, ui.headlabel->size());

	ui.headlabel->setPixmap(headpixmap);			//设置头像
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
	__super::enterEvent(event);			//进行一些其它的默认操作
}

void TalkWindowItem::leaveEvent(QEnterEvent* event)
{
	ui.tclosebtn->setVisible(false);
	__super::enterEvent(event);				//进行一些其它的，默认的操作
}

void TalkWindowItem::resizeEvent(QResizeEvent* event)
{
	__super::resizeEvent(event);					//进行一些其它的，默认操作
}

void TalkWindowItem::initControl()
{
	//软件刚开始的时候，都是不可见的，所以用false
	//setVisible(),设置是否可见，true可见，false不可见
	ui.tclosebtn->setVisible(false);

	//如果点击关闭按钮，右边的消息聊天窗口也要关闭
	connect(ui.tclosebtn, SIGNAL(clicked(bool)), this, SIGNAL(signalCloseClicked()));
}
