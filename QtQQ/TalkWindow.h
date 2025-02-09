#pragma once

#include <QWidget>
#include "ui_TalkWindow.h"
#include "TalkWindowShell.h"

class TalkWindow : public QWidget
{
	Q_OBJECT

public:
	TalkWindow(QWidget *parent , const QString& uid/*, GroupType groupType*/);
	~TalkWindow();
public:
	void addEmotionImage(int emotionNum);
	void setWindowName(const QString& name);
	QString getTalkId();

	// ========== 新增：setTalkId ==========
	void setTalkId(const QString& newId);
private slots:
	void onFileOpenBtnClicked(bool);
	void onSendBtnClicked(bool);
	void onItemDoubleClicked(QTreeWidgetItem* item,int column);


private:

	void initControl();				
	void initGroupTalkStatus();			//初始化群聊状态
	int getCompDepID();					//获取公司名，ID号
	//void initComPanyTalk();			//初始化公司群聊天
	//void initPersonelTalk();			//初始化人事部聊天
	//void initDevelopTalk();			//初始化研发部聊天
	//void initMarketTalk();				//初始化市场部聊天
	void initTalkWindow();			//初始化群聊
	void initPtoPTalk();					//初始化单独聊天

	//void addPeopInfo(QTreeWidgetItem* pRootGroupItem);
	void addPeopInfo(QTreeWidgetItem* pRootGroupItem, int employeeID);
private:
	Ui::TalkWindow ui;
	QString m_talkId;
	bool m_isGroupTalk;			//是否为群聊
	//GroupType m_groupType;
	QMap<QTreeWidgetItem*, QString> m_groupPeopleMap;			//所有分组联系人姓名	
	friend class TalkWindowShell;
};
