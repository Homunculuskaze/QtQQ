#include "WindowManager.h"
#include "TalkWindowItem.h"
#include "TalkWindow.h"

#include <QSqlQueryModel>
//单例模式，创建全局静态对象
Q_GLOBAL_STATIC(WindowManager, theInstance);


WindowManager::WindowManager()
	:QObject(nullptr)
	,m_talkwindowshell(nullptr)
{}

WindowManager::~WindowManager()
{}

QWidget* WindowManager::findWindowName(const QString & qsWindowName)
{
	//根据传进来的窗口名称，来做判断
	//映射中，如果包含传进来的窗体
	if (m_windowMap.contains(qsWindowName))
	{
		//如果包含，就返回m_windowMap的值
		return m_windowMap.value(qsWindowName);
	}

	//如果没有就返回空
	return nullptr;
}

void WindowManager::deleteWindowName(const QString& qsWindowName)
{
	//从映射中，进行删除，就是关闭窗口
	//根据窗口名称，进行删除
	m_windowMap.remove(qsWindowName);
}

void WindowManager::addWindowName(const QString& qsWindowName, QWidget* qWidget)
{
	//在映射中，添加窗口名
	//先判断，映射中有没有包含传进来的窗口名
	//没有的话，再进行添加
	if (!m_windowMap.contains(qsWindowName))
	{
		m_windowMap.insert(qsWindowName, qWidget);
	}
}

WindowManager* WindowManager::getInstance()
{
	return theInstance();
}

void WindowManager::addNewTalkWindow(const QString& uid/*, GroupType groupType, const QString strPeople*/)
{
	if (m_talkwindowshell == nullptr)
	{
		m_talkwindowshell = new TalkWindowShell;
		connect(m_talkwindowshell, &TalkWindowShell::destroyed, [this](QObject* obj) {
			m_talkwindowshell = nullptr;
			});
	}

	// 判断，窗口的名字
	// 传进来的uid，
	// 如果是私聊的话，这个uid就是聊天对象的QQ号
	// 如果是群聊的话，就是 群号
	QWidget* widget=findWindowName(uid);

	if (!widget)
	{
		TalkWindow* talkwindow = new TalkWindow(m_talkwindowshell, uid/*, groupType*/);
		TalkWindowItem* talkwindowItem = new TalkWindowItem(talkwindow);

		//判断是群聊还是单聊
		QSqlQueryModel sqlDepModel;
		QString strSql = QString("SELECT department_name,sign FROM tab_department WHERE departmentID = %1").arg(uid);
		sqlDepModel.setQuery(strSql);
		int rows = sqlDepModel.rowCount();

		QString strWindowName, strMsgLabel;
		if (rows == 0)		//单聊
		{
			QString sql = QString("SELECT employee_name,employee_sign FROM tab_employees WHERE employeeID = %1").arg(uid);
			sqlDepModel.setQuery(sql);
		}

		QModelIndex indexDepIndex, signIndex;
		indexDepIndex = sqlDepModel.index(0, 0);				//部门索引的第0行第0列
		signIndex = sqlDepModel.index(0, 1);						//0行1列
		strWindowName = sqlDepModel.data(signIndex).toString();
		strMsgLabel = sqlDepModel.data(indexDepIndex).toString();

		talkwindow->setWindowName(strWindowName);				//窗口名称
		talkwindowItem->setMsgLabelContent(strMsgLabel);			//左侧联系人文本显示
		m_talkwindowshell->addTalkWindow(talkwindow, talkwindowItem, uid);

		/*
		switch (groupType)
		{
		case COMPANY:
		{
			talkwindow->setWindowName(QStringLiteral("奇牛科技-越分享越拥有"));
			talkwindowItem->setMsgLabelContent(QStringLiteral("公司群"));
			break;
		}
		case PERSONELGROUP:
		{
			talkwindow->setWindowName(QStringLiteral("公心、民心、爱心、事事在心"));
			talkwindowItem->setMsgLabelContent(QStringLiteral("人事部"));
			break;
		}
		case MARKETGROUP:
		{
			talkwindow->setWindowName(QStringLiteral("今天工作不努力，明天努力找工作"));
			talkwindowItem->setMsgLabelContent(QStringLiteral("市场群"));
			break;
		}
		case DEVELOPMENTGROUP:
		{
			talkwindow->setWindowName(QStringLiteral("只有两种编程语言：一种是天天挨骂的，另一种是没人用的"));
			talkwindowItem->setMsgLabelContent(QStringLiteral("研发部"));
			break;
		}
		case PTOP:
		{
			talkwindow->setWindowName(QStringLiteral(""));
			talkwindowItem->setMsgLabelContent(strPeople);			
			break;
		}
		default:
			break;
		}

		m_talkwindowshell->addTalkWindow(talkwindow, talkwindowItem, groupType);
		*/
	}
	else
	{
		//左侧聊天列表设为选中
		QListWidgetItem* item=m_talkwindowshell->getTalkWindowItemMap().key(widget);
		item->setSelected(true);

		//设置右侧当前聊天窗口
		m_talkwindowshell->setCurrentWidget(widget);
	}

	m_talkwindowshell->show();
	m_talkwindowshell->activateWindow();
}
