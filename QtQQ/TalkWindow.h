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

private slots:
	void onSendBtnClicked(bool);
	void onItemDoubleClicked(QTreeWidgetItem* item,int column);


private:

	void initControl();

	void initComPanyTalk();			//初始化公司群聊天
	void initPersonelTalk();			//初始化人事部聊天
	void initDevelopTalk();			//初始化研发部聊天
	void initMarketTalk();				//初始化市场部聊天
	void initPtoPTalk();					//初始化单独聊天

	void addPeopInfo(QTreeWidgetItem* pRootGroupItem);
private:
	Ui::TalkWindow ui;
	QString m_talkId;
	//GroupType m_groupType;
	QMap<QTreeWidgetItem*, QString> m_groupPeopleMap;			//所有分组联系人姓名	
};
