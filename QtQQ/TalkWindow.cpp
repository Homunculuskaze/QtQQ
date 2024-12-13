#include "TalkWindow.h"
#include "RootContactItem.h"
#include "ContactItem.h"
#include "CommonUtils.h"
#include "WindowManager.h"
#include <QToolTip>
#include <QFile>
#include <QMessageBox>
TalkWindow::TalkWindow(QWidget* parent , const QString& uid/*, GroupType groupType*/)
	: QWidget(parent)
	,m_talkId(uid)
	//,m_groupType(groupType)
{
	ui.setupUi(this);
	WindowManager::getInstance()->addWindowName(m_talkId, this);
	setAttribute(Qt::WA_DeleteOnClose);
	initControl();
}

TalkWindow::~TalkWindow()
{
	WindowManager::getInstance()->deleteWindowName(m_talkId);
}

void TalkWindow::addEmotionImage(int emotionNum)
{
	//先获取焦点
	ui.textEdit->setFocus();
	//调用 添加表情图片
	ui.textEdit->addEmotionUrl(emotionNum);
}

void TalkWindow::setWindowName(const QString& name)
{
	ui.nameLabel->setText(name);
}

void TalkWindow::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
	bool bIsChild=item->data(0, Qt::UserRole).toBool();
	if (bIsChild)
	{
		QString strPeppleName = m_groupPeopleMap.value(item);
		WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString()/*, PTOP, strPeppleName*/);
	}
}



void TalkWindow::initComPanyTalk()
{
	//先构造根项
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();

	//接下来设置孩子，指示器的策略
	//指示器设置的策略，不管这个子项有没有存在
	//这个指示器都会存在，都可以用于收缩
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);

	//开始设置数据
	pRootItem->setData(0, Qt::UserRole, 0);			//参数（第0列，用户，数据设置为0）

	//构造根项
	//第一个参数，是否需要箭头，不需要就写false
	//第二个参数，让树作为他的父部件
	RootContactItem* pItemName = new RootContactItem(false, ui.treeWidget);

	ui.treeWidget->setFixedHeight(646);				//设置Shell高度减去shell头高（talkwindow titleWidget）

	int nEmployeeNum = 50;			//假设有50人
	//QString qsGroupName = QString::fromLocal8Bit("公司群 %1/%2").arg(0).arg(nEmployeeNum);
	QString qsGroupName = QString::fromUtf8("公司群 %1/%2").arg(0).arg(nEmployeeNum);
	//将qsGroupName这个文本，设置为当前项pItemName的文本
	pItemName->setText(qsGroupName);

	//插入分组节点
	ui.treeWidget->addTopLevelItem(pRootItem);			//添加最上面的一项，就添加一项
	//设置项目部件（对根项pRootItem设置，第0列。设置它的部件是pItemName）
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);

	//将所有的子项，默认展开
	pRootItem->setExpanded(true);

	//添加子项
	for (int i = 0; i < nEmployeeNum; i++)
	{
		addPeopInfo(pRootItem);
	}
}

void TalkWindow::initPersonelTalk()
{
	//先构造根项
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();

	//接下来，设置孩子，指示器的策略
	//指示器设置的策略，不管这个子项有没有存在
	//这个指示器都会存在，都可以用于收缩
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);

	//开始设置数据
	pRootItem->setData(0, Qt::UserRole, 0);				//参数（第0列，用户，数据设置为0）

	//构造根项
	//第一个参数，是否需要箭头，不需要就写false
	//第二个参数，让树作为它的父部件
	RootContactItem* pItemName = new RootContactItem(false, ui.treeWidget);

	ui.treeWidget->setFixedHeight(646);				//设置Shell高度减去shell头高（talkwindow titleWidget）

	int nEmployeeNum = 5;			//假设有5人
	//QString qsGroupName = QString::fromLocal8Bit("人事部 %1/%2").arg(0).arg(nEmployeeNum);
	QString qsGroupName = QString::fromUtf8("人事部 %1/%2").arg(0).arg(nEmployeeNum);
	//将qsGroupName这个文本，设置为当前项pItemName的文本
	pItemName->setText(qsGroupName);

	//插入分组节点
	ui.treeWidget->addTopLevelItem(pRootItem);			//添加最上面的一项，就添加一项
	//设置项目部件（对根项pRootItem设置，第0列，设置它的部件是pItemName）
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);

	//将所有的子项，默认展开
	pRootItem->setExpanded(true);

	//添加子项
	for (int i = 0; i < nEmployeeNum; i++)
	{
		addPeopInfo(pRootItem);
	}
}

void TalkWindow::initDevelopTalk()
{
	//先构造根项
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();

	//接下来，设置孩子，指示器的策略
	//指示器设置的策略，不管这个子项有没有存在
	//这个指示器都会存在，都可以用于收缩
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);

	//开始设置数据
	pRootItem->setData(0, Qt::UserRole, 0);			//参数（第0列，用户，数据设置为0）

	//构造根项
	//第一个参数，是否需要箭头，不需要就写false
	//第二个参数，让树作为它的父部件
	RootContactItem* pItemName = new RootContactItem(false, ui.treeWidget);

	ui.treeWidget->setFixedHeight(646);			//设置Shell高度 减去Shell头高(talkwindow titleWidget)

	int nEmployeeNum = 32;			//假设有32人
	//QString qsGroupName = QString::fromLocal8Bit("研发部 %1/%2").arg(0).arg(nEmployeeNum);
	QString qsGroupName = QString::fromUtf8("研发部 %1/%2").arg(0).arg(nEmployeeNum);
	//将qsGroupName这个文本，设置为当前项pItemName的文本
	pItemName->setText(qsGroupName);

	//插入分组节点
	ui.treeWidget->addTopLevelItem(pRootItem);				//添加最上面的一项，就添加一项
	//设置项目部件（对根项pRootItem设置，第0列，设置它的部件是pItemName)
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);

	//将所有的子项，默认展开
	pRootItem->setExpanded(true);


	//添加子项
	for (int i = 0; i < nEmployeeNum; i++)
	{
		addPeopInfo(pRootItem);
	}
}

void TalkWindow::initMarketTalk()
{
	//先构造根项
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();

	//接下来，设置孩子，指示器的策略
	//指示器设置的策略，不管这个子项有没有存在
	//这个指示器都会存在，都可以用于收缩
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);

	//开始设置数据
	pRootItem->setData(0, Qt::UserRole, 0);			//参数（第0列，用户，数据设置为0）

	//构造根项
	//第一个参数，是否需要箭头，不需要就写false
	//第二个参数，让树作为它的父部件
	RootContactItem* pItemName = new RootContactItem(false, ui.treeWidget);

	ui.treeWidget->setFixedHeight(646);			//设置Shell高度 减去Shell头高(talkwindow titleWidget)

	int nEmployeeNum = 8;			//假设有8人
	//QString qsGroupName = QString::fromLocal8Bit("市场部 %1/%2").arg(0).arg(nEmployeeNum);
	QString qsGroupName = QString::fromUtf8("市场部 %1/%2").arg(0).arg(nEmployeeNum);
	//将qsGroupName这个文本，设置为当前项pItemName的文本
	pItemName->setText(qsGroupName);

	//插入分组节点
	ui.treeWidget->addTopLevelItem(pRootItem);				//添加最上面的一项，就添加一项
	//设置项目部件（对根项pRootItem设置，第0列，设置它的部件是pItemName)
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);

	//将所有的子项，默认展开
	pRootItem->setExpanded(true);


	//添加子项
	for (int i = 0; i < nEmployeeNum; i++)
	{
		addPeopInfo(pRootItem);
	}
}

void TalkWindow::initPtoPTalk()
{
	//单独私聊的时候，在右侧放一张图片
	QPixmap pixSkin;
	pixSkin.load(":/Resources/MainWindow/skin.png");

	ui.widget->setFixedSize(pixSkin.size());			//设置尺寸

	//给widget，设置图片
	QLabel* skinLabel = new QLabel(ui.widget);
	skinLabel->setPixmap(pixSkin);

	//同时对标签，设置大小
	skinLabel->setFixedSize(ui.widget->size());
}

void TalkWindow::addPeopInfo(QTreeWidgetItem* pRootGroupItem)
{
	//
	QTreeWidgetItem* pChild = new QTreeWidgetItem();

	QPixmap pix1;
	pix1.load(":/Resources/MainWindow/head_mask.png");
	//QImage image(":/Resources/MainWindow/girl.png");
	const QPixmap image(":/Resources/MainWindow/girl.png");

	//添加子节点
	pChild->setData(0, Qt::UserRole, 1);
	pChild->setData(0, Qt::UserRole + 1, QString::number((int)pChild));
	ContactItem* pContactItem = new ContactItem(ui.treeWidget);

	static int i = 0;
	pContactItem->setHeadPixmap(CommonUtils::getRoundImage(image, pix1, pContactItem->getHeadLabelSize()));
	//pContactItem->setUserName(QString::fromLocal8Bit("奇奇%1").arg(i++));
	pContactItem->setUserName(QString::fromUtf8("奇奇%1").arg(i++));
	pContactItem->setSignName(QString::fromLocal8Bit(""));

	pRootGroupItem->addChild(pChild);
	ui.treeWidget->setItemWidget(pChild, 0, pContactItem);

	QString str = pContactItem->getUserName();
	m_groupPeopleMap.insert(pChild, str);
}

void TalkWindow::onSendBtnClicked(bool)
{
	//发送信息不能为空，如果为空 给个提示
	//转换成群文本 toPlainText();
	if (ui.textEdit->toPlainText().isEmpty())
	{
		//显示文本，showText（坐标，字符串文本，窗口，绘制矩形，显示2000毫秒）
		//先给个坐标，是一个相对位置，要转成全局坐标位置(this->mapToGlobal)
		QToolTip::showText(this->mapToGlobal(QPoint(630, 660)),
		//QString::fromLocal8Bit("发送的信息不能为空"), this, QRect(0, 0, 120, 100), 2000);
			QString::fromUtf8("发送的信息不能为空"), this, QRect(0, 0, 120, 100), 2000);
		return;
	}

	//将文本转成html类型
	//QString& html = ui.textEdit->document()->toHtml();
	QString html = ui.textEdit->document()->toHtml();

	//文本html，如果没有字体，则添加字体
	if (!html.contains(".png") && !html.contains("</span>"))
	{
		QString fontHtml;
		QString text = ui.textEdit->toPlainText();
		QFile file(":/Resources/MainWindow/MsgHtml/msgFont.txt");
		if (file.open(QIODevice::ReadOnly))							
		{
			fontHtml = file.readAll();
			//将html文件里的%1,用字符串text替换
			fontHtml.replace("%1", text);
			file.close();
		}
		else
		{
			//this,当前聊天部件，作为父窗体
			QMessageBox::information(this, QString::fromLocal8Bit("提示"),
				QString::fromLocal8Bit("文件 msgFont.txt 不存在! "));
			return;
		}

		//判断转换后，有没有包含fontHtml
		if (!html.contains(fontHtml))
		{
			html.replace(text, fontHtml);
		}
	}

	//清空
	ui.textEdit->clear();
	ui.textEdit->deleteAllEmotionImage();

	//收信息窗口，追加信息
	ui.msgWidget->appendMsg(html);


}

void TalkWindow::initControl()
{
	//献给右边分裂器设置尺寸
	QList<int> rightWidgetSize;
	rightWidgetSize << 600 << 138;

	//参数是一个链表
	//如果是左右的分裂器，会给每个部分设置宽度
	//如果是上下的分裂器，会给每个部分设置高度
	ui.bodySplitter->setSizes(rightWidgetSize);

	ui.textEdit->setFontPointSize(10);
	ui.textEdit->setFocus();			//获取焦点，打开窗口的时候，直接就可以输入，不需要先用鼠标点一下

	//最小化，窗口右上角关闭，关闭（发送旁边的按钮）
	connect(ui.sysmin, SIGNAL(clicked(bool)), parent(), SLOT(onShowMin(bool)));
	connect(ui.sysclose, SIGNAL(clicked(bool)), parent(), SLOT(onShowClose(bool)));
	connect(ui.closeBtn, SIGNAL(clicked(bool)), parent(), SLOT(onShowMin(bool)));

	//表情按钮被点击
	connect(ui.faceBtn, SIGNAL(clicked(bool)), parent(), SLOT(onEmotionBtnClicked(bool)));

	//发送
	connect(ui.sendBtn, SIGNAL(clicked(bool)), this, SLOT(onSendBtnClicked(bool)));

	//发送文件信号
	//connect(ui.fileopenBtn,SIGNAL(clicked(bool)), parent(), SLOT(onFileOpenBtnClicked(bool)));

	//树，被双击的时候
	//要带参数，双击的是哪一项，和哪一项的地址
	connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
		this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));

	/*
	//初始化结束，开始做判断
	switch (m_groupType)
	{
		case COMPANY:
		{
			initComPanyTalk();			//初始化公司群聊天
			break;
		}
		case PERSONELGROUP:
		{
			initPersonelTalk();
			break;
		}
		case DEVELOPMENTGROUP:
		{
			initDevelopTalk();
			break;
		}
		case MARKETGROUP:
		{
			initMarketTalk();
			break;
		}
		default:		//默认单聊
		{
			initPtoPTalk();				//初始化市场部聊天
			break;
		}
	}
	*/
}
