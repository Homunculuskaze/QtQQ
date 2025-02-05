#include "TalkWindow.h"
#include "RootContactItem.h"
#include "ContactItem.h"
#include "CommonUtils.h"
#include "WindowManager.h"
#include "SendFile.h"
#include <QToolTip>
#include <QFile>
#include <QMessageBox>
#include <QSqlQueryModel>
#include <QSqlQuery>
TalkWindow::TalkWindow(QWidget* parent , const QString& uid/*, GroupType groupType*/)
	: QWidget(parent)
	,m_talkId(uid)
	//,m_groupType(groupType)
{
	ui.setupUi(this);
	WindowManager::getInstance()->addWindowName(m_talkId, this);
	setAttribute(Qt::WA_DeleteOnClose);

	//在初始化控件之前，初始化状态，否则群聊状态永远都是false
	initGroupTalkStatus();
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

QString TalkWindow::getTalkId()
{
	return m_talkId;
}

// ========== 新增：setTalkId ========== 在talkwindowshell使用过
void TalkWindow::setTalkId(const QString& newId)
{
	// 先把旧 ID 从 WindowManager 中移除（若你希望覆盖旧键）
	WindowManager::getInstance()->deleteWindowName(m_talkId);

	// 更新成员变量
	m_talkId = newId;

	// 重新注册到 WindowManager，保持一致
	WindowManager::getInstance()->addWindowName(m_talkId, this);
}

void TalkWindow::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
	bool bIsChild=item->data(0, Qt::UserRole).toBool();
	if (bIsChild)
	{
		//QString strPeppleName = m_groupPeopleMap.value(item);
		WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString()/*, PTOP, strPeppleName*/);
	}
}

void TalkWindow::onFileOpenBtnClicked(bool)
{
	SendFile* sendFile = new SendFile(this);
	sendFile->show();
}

/*
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
*/

void TalkWindow::initTalkWindow()
{
	//先构造根项
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();


	//接下来，设置孩子，指示器的策略
	//指示器设置的策略，不管这个子项有没有存在，
	//这个指示器都会存在，都可以用于收缩
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);

	//开始设置数据
	pRootItem->setData(0, Qt::UserRole, 0);			//参数（第0列，用户，数据设置为0）

	//构造根项
	//第一个参数，是否需要箭头，不需要就写false
	//第二个参数，让树，作为他的父部件
	RootContactItem* pItemName = new RootContactItem(false, ui.treeWidget);

	ui.treeWidget->setFixedHeight(646);				//设置Shell高度减去shell头高（talkwindow titleWidget）

	//获取群的名称
	QString strGroupName;
	//查询分组名称/////////////////////自己弄的prepare和bind
	//QSqlQuery queryGroupName(QString("SELECT department_name FROM tab_department WHERE departmentID = %1").arg(m_talkId));
	QSqlQuery queryGroupName;
	queryGroupName.prepare("SELECT department_name FROM tab_department WHERE departmentID = :talkID");
	queryGroupName.bindValue(":talkID", m_talkId);
	queryGroupName.exec();

	if (queryGroupName.next())
	{
		strGroupName = queryGroupName.value(0).toString();
	}




	/*
		如果不是公司群，就要进行筛选
		在员工表里面，想查一下人事部，有几个员工，
		只要进行departmentID等于人事部的ID，进行筛选

		如果是2000(公司群)的时候，就要进行特殊处理
		那就将员工表里面的数据，全部拿过来，直接 select * 全部选择
		在根据departmentID，进行筛选
	*/
	//先获取群号，再与m_talkID进行比对
	QSqlQueryModel queryEmployeeModel;


	qDebug() << "m_talkId: " << m_talkId.toInt() << ", Company Department ID: " << getCompDepID();


	//如果是一样的，那就是公司群
	if (getCompDepID() == m_talkId.toInt())
	{
		//查询，状态为1
		queryEmployeeModel.setQuery("SELECT employeeID FROM tab_employees WHERE status =1");
	}
	else
	{	//如果不是公司群，通过公司群号，进行过滤
		queryEmployeeModel.setQuery(QString("SELECT employeeID FROM tab_employees WHERE status =1 AND departmentID = %1").arg(m_talkId));
	}

	//获取到当前员工的数量
	int nEmployeeNum = queryEmployeeModel.rowCount();

	//群聊名字，在线人数是多少，员工数量
	QString qsGroupName = QString::fromUtf8("%1 %2/%3").arg(strGroupName).arg(0).arg(nEmployeeNum);

	//将qsGroupName这个文本设置为当前项pItemName的文本
	pItemName->setText(qsGroupName);

	//插入分组节点
	ui.treeWidget->addTopLevelItem(pRootItem);				//添加最上面的一项，就添加一项
	//设置项目部件(对根项pRootItem设置，第0列，设置它的部件是pItemName)
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);

	//将所有的子项，默认展开
	pRootItem->setExpanded(true);

	//添加子项
	for (int i = 0; i < nEmployeeNum; i++)
	{
		//查询员工模型索引，获取员工QQ号
		QModelIndex modelIndex = queryEmployeeModel.index(i, 0);
		int employeeID = queryEmployeeModel.data(modelIndex).toInt();

		//传入员工ID，这样每次添加的员工都是独一无二的
		addPeopInfo(pRootItem, employeeID);
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

void TalkWindow::addPeopInfo(QTreeWidgetItem* pRootGroupItem, int employeeID)
{
	//
	QTreeWidgetItem* pChild = new QTreeWidgetItem();

	//添加子节点
	pChild->setData(0, Qt::UserRole, 1);
	pChild->setData(0, Qt::UserRole + 1, employeeID);
	ContactItem* pContactItem = new ContactItem(ui.treeWidget);

	QPixmap pix1;
	pix1.load(":/Resources/MainWindow/head_mask.png");
	
	//获取名、签名、头像
	QString strName, strSign, strPicturePath;
	QSqlQueryModel queryInfoModel;
	queryInfoModel.setQuery(QString("SELECT employee_name,employee_sign,picture FROM tab_employees WHERE employeeID = %1").arg(employeeID));
	QModelIndex nameIndex, signIndex, pictureIndex;
	nameIndex = queryInfoModel.index(0, 0);		//行，列
	signIndex = queryInfoModel.index(0, 0);
	signIndex = queryInfoModel.index(0, 1);
	pictureIndex = queryInfoModel.index(0, 2);

	strName = queryInfoModel.data(nameIndex).toString();
	strSign = queryInfoModel.data(signIndex).toString();
	strPicturePath = queryInfoModel.data(pictureIndex).toString();

	QImage imageHead;
	imageHead.load(strPicturePath);

	pContactItem->setHeadPixmap(CommonUtils::getRoundImage(QPixmap::fromImage(imageHead), pix1, pContactItem->getHeadLabelSize()));
	//pContactItem->setUserName(QString::fromLocal8Bit("奇奇%1").arg(i++));
	pContactItem->setUserName(strName);
	pContactItem->setSignName(strSign);

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
	connect(ui.fileopenBtn, SIGNAL(clicked(bool)), this, SLOT(onFileOpenBtnClicked(bool)));

	//树，被双击的时候
	//要带参数，双击的是哪一项，和哪一项的地址
	connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
		this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));

	if (m_isGroupTalk)
	{
		initTalkWindow();
	}
	else
	{
		initPtoPTalk();
	}


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

void TalkWindow::initGroupTalkStatus()
{
	QSqlQueryModel sqlDepModel;
	//SELECT * FROM tab_department WHERE departmentID = 2001
	QString strSql = QString("SELECT * FROM tab_department WHERE departmentID = %1").arg(m_talkId);

	sqlDepModel.setQuery(strSql);

	int rows = sqlDepModel.rowCount();			//返回总共的模型行号

	//==0为单聊
	if (rows == 0)
	{
		m_isGroupTalk = false;
	}
	else
	{
		m_isGroupTalk = true;
	}
}

int TalkWindow::getCompDepID()
{
	QSqlQuery queryDepID;
	//在使用 QSqlQuery 的 prepare 和 bindValue 方法时，Qt 会自动处理字符串的引号问题，你不需要手动在 SQL 查询中加单引号
	queryDepID.prepare("SELECT departmentID FROM tab_department WHERE department_name = :department_name");
	queryDepID.bindValue(":department_name", QString::fromUtf8("公司群"));			//这里不能用QString::fromLocal8Bit("公司群")否则会返回0，只能用fromUtf8
	queryDepID.exec();
	queryDepID.next();				//调用queryDepID.first();也行
	return queryDepID.value(0).toInt();
}