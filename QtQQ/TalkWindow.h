#pragma once

#include <QWidget>
#include "ui_TalkWindow.h"
#include "TalkWindowShell.h"

class TalkWindow : public QWidget
{
	Q_OBJECT

public:
	TalkWindow(QWidget *parent , const QString& uid, GroupType groupType);
	~TalkWindow();
public:
	void addEmotionImage(int emotionNum);
	void setWindowName(const QString& name);

private slots:
	void onSendBtnClicked(bool);
	void onItemDoubleClicked(QTreeWidgetItem* item,int column);


private:

	void initControl();

	void initComPanyTalk();			//��ʼ����˾Ⱥ����
	void initPersonelTalk();			//��ʼ�����²�����
	void initDevelopTalk();			//��ʼ���з�������
	void initMarketTalk();				//��ʼ���г�������
	void initPtoPTalk();					//��ʼ����������

	void addPeopInfo(QTreeWidgetItem* pRootGroupItem);
private:
	Ui::TalkWindow ui;
	QString m_talkId;
	GroupType m_groupType;
	QMap<QTreeWidgetItem*, QString> m_groupPeopleMap;			//���з�����ϵ������	
};