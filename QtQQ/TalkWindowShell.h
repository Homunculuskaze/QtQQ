#pragma once

#include <QWidget>
#include <QMap>
#include <QListWidgetItem>

#include "ui_TalkWindowShell.h"
#include "basicwindow.h"
#include "EmotionWindow.h"

class TalkWindow;
class TalkWindowItem;
class QListWidgetItem;
class EmotionWindow;

enum GroupType {
	COMPANY = 0,		//公司群
	PERSONELGROUP,		//人事部
	DEVELOPMENTGROUP,		//研发部
	MARKETGROUP,				//市场部
	PTOP								//同事之间单独聊天
};

class TalkWindowShell : public BasicWindow
{
	Q_OBJECT

public:
	TalkWindowShell(QWidget *parent = Q_NULLPTR);
	~TalkWindowShell();

public:
	//添加新的聊天窗口
	void addTalkWindow(TalkWindow* talkWindow, TalkWindowItem* talkWindowItem, const QString& uid/*, GroupType grouptype*/);

	//设置当前聊天窗口
	void setCurrentWidget(QWidget* widget);

	const QMap<QListWidgetItem*, QWidget*>& getTalkWindowItemMap()const;
private:
	void initControl();				//初始化控件

public slots:
	void onEmotionBtnClicked(bool);				//表情按钮点击后执行的槽函数

private slots:
	   void onTalkWindowItemClicked(QListWidgetItem* item);			//左侧列表点击后执行的槽函数
	   void onEmotionItemClicked(int emotionNum);								//表情被选中
private:
	Ui::TalkWindowClass ui;
	QMap<QListWidgetItem*, QWidget*>m_talkwindowItemMap;			//打开的聊天窗口
	EmotionWindow* m_emotionWindow;			//表情窗口
};