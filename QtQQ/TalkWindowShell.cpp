#include "TalkWindowShell.h"
#include "EmotionWindow.h"
#include "CommonUtils.h"
#include "TalkWindow.h"
#include "TalkWindowItem.h"
#include <QListWidget>
//#include <QPoint>
#include <QFile>
#include <QSqlQueryModel>
TalkWindowShell::TalkWindowShell(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initControl();
}

TalkWindowShell::~TalkWindowShell()
{
	delete m_emotionWindow;
	m_emotionWindow=nullptr;
}

void TalkWindowShell::addTalkWindow(TalkWindow* talkWindow, TalkWindowItem* talkWindowItem,const QString& uid/*, GroupType grouptype*/)
{
	ui.rightStackedWidget->addWidget(talkWindow);

	//发送者：表情窗口，发射表情窗口隐藏信号
	//接收者：聊天窗口，设置表情按钮的状态
	connect(m_emotionWindow, SIGNAL(signalEmotionWindowHide()),
		talkWindow, SLOT(onSetEmotionBtnStatus()));

	//创建新聊天项
	QListWidgetItem* aItem = new QListWidgetItem(ui.listWidget);

	//把新聊天项，插入到映射链表中
	//一项 对应 一个聊天窗口
	m_talkwindowItemMap.insert(aItem, talkWindow);

	aItem->setSelected(true);			//被选中

	//判断群聊还是单聊
	QSqlQueryModel sqlDepModel;
	QString strQuery = QString("SELECT picture FROM tab_department WHERE departmentID = %1").arg(uid);
	sqlDepModel.setQuery(strQuery);
	int rows = sqlDepModel.rowCount();

	if (rows == 0)		//单聊
	{
		strQuery = QString("SELECT picture FROM tab_employees WHERE employeeID = %1").arg(uid);
		sqlDepModel.setQuery(strQuery);
	}

	QModelIndex index;
	index = sqlDepModel.index(0, 0);			//0行0列
	QImage img;
	img.load(sqlDepModel.data(index).toString());

	talkWindowItem->setHeadPixmap(QPixmap::fromImage(img));			//设置头像
	ui.listWidget->addItem(aItem);
	ui.listWidget->setItemWidget(aItem, talkWindowItem);

	onTalkWindowItemClicked(aItem);

	connect(talkWindowItem, &TalkWindowItem::signalCloseClicked, [talkWindowItem, talkWindow, aItem, this]() {
		m_talkwindowItemMap.remove(aItem);
		talkWindow->close();
		ui.listWidget->takeItem(ui.listWidget->row(aItem));
		delete talkWindowItem;
		ui.rightStackedWidget->removeWidget(talkWindow);
		if (ui.rightStackedWidget->count() < 1)
			close();
		});

	////判断是 群聊 还是单聊
	//QSqlQueryModel sqlDepModel;
	//QString strQuery = QString("SELECT picture FROM tab_department WHERE departmentID = %1").arg(uid);
	//sqlDepModel.setQuery(strQuery);
	//int rows = sqlDepMedel.rowCount();				//获取行号

	////判断是单聊
	//if (rows == 0)
	//{
	//	strQuery = QString("SELECT picture FROM tab_employee WHERE employeeID=%1").arg(uid);
	//	sqlDepModel.setQuery(strQuery);
	//}

	//QModelIndex index;
	//index = sqlDepModel.index(0, 0);					//0行,0列

	//QImage img;
	////获取图片路径，加载
	//img.load(sqlDepModel.data(index).toString());

	////设置头像
	//talkWindowItem->setHeadPixmap(QPixmap::fromImage(img));
	////添加项
	//ui.listWidget->addItem(aItem);
	////设置列表部件
	//ui.listWidget->setItemWidget(aItem, talkWindowItem);

	////被点击
	//onTalkWindowItemClicked(aItem);

	////需要捕获的信号非常多，窗口关闭之后，要进行资源回收
	////（1）聊天窗口要关闭 ，	（2）当前聊天项要移除    （3）talkWindow资源要释放	（4）右边多页面的显示，也要删除
	//connect(talkWindowItem, &TalkWindowItem::signalCloseClicked, [talkWindowItem, talkWindow, aItem, this]()
	//	{
	//		//先处理左边部分
	//		m_talkwindowItemMap.remove(aItem);			//删除项
	//		talkWindow->close();										//聊天窗口关闭
	//		//通过takeItem方法移除，参数是行号，
	//		//调用row方法，将aItem项，传进去
	//		ui.listWidget->talkItem(ui.listWidget->row(aItem));
	//		delete talkWindowItem;

	//		//再处理右边部分
	//		ui.rightStackedWidget->removeWidget(talkWindow);

	//		//做一个判断，如果删除的是最后一个 窗口就将窗口关闭
	//		if (ui.rightStackedWidget->count() < 1)
	//		{
	//			close();
	//		}
	//	});
}

void TalkWindowShell::setCurrentWidget(QWidget* widget)
{
	ui.rightStackedWidget->setCurrentWidget(widget);
}

const QMap<QListWidgetItem*, QWidget*>& TalkWindowShell::getTalkWindowItemMap() const
{
	return m_talkwindowItemMap;
}

void TalkWindowShell::initControl()
{
	loadStyleSheet("TalkWindow");
	setWindowTitle(QString::fromLocal8Bit("奇奇-聊天窗口"));

	m_emotionWindow = new EmotionWindow;
	m_emotionWindow->hide();		//隐藏表情窗口

	QList<int> leftWidgetSize;
	leftWidgetSize << 154 << width() - 154;
	ui.splitter->setSizes(leftWidgetSize);				//分裂器设置尺寸
	
	ui.listWidget->setStyle(new CustomProxyStyle(this));

	connect(ui.listWidget, &QListWidget::itemClicked, this, &TalkWindowShell::onTalkWindowItemClicked);
	connect(m_emotionWindow, SIGNAL(signalEmotionItemClicked(int)), this, SLOT(onEmotionItemClicked(int)));
}

void TalkWindowShell::onEmotionBtnClicked(bool)
{
	//表情窗口，点击之后，要隐藏起来
	//设置成【不可见】
	m_emotionWindow->setVisible(!m_emotionWindow->isVisible());			//如果   可见   则设置为   不可见，不可见   则设置为   可见

	//将当前控件的相对位置，转换为屏幕的绝对位置
	QPoint emotionPoint = this->mapToGlobal(QPoint(0, 0));

	emotionPoint.setX(emotionPoint.x() + 170);
	emotionPoint.setY(emotionPoint.y() + 220);
	m_emotionWindow->move(emotionPoint);					//移动

}

void TalkWindowShell::onTalkWindowItemClicked(QListWidgetItem* item)
{
	//取得窗体地址
	QWidget* talkWindowWidget = m_talkwindowItemMap.find(item).value();

	//将右边的窗体更新
	ui.rightStackedWidget->setCurrentWidget(talkWindowWidget);
}


void TalkWindowShell::onEmotionItemClicked(int emotionNum)
{
	//获取当前的聊天窗口
	//获取右边当前的部件，rightStackWidget调用currentWidget，
	//返回的指针类型是Widget部件类型，需要转换成TalkWindow* 指针类型
	TalkWindow* curTalkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());

	//判断指针是否有效
	if (curTalkWindow)
	{
		//对聊天窗口添加图片
		//表情序号传进去，进行加载
		curTalkWindow->addEmotionImage(emotionNum);
	}
}
