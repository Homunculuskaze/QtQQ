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

	// ========== ������setTalkId ==========
	void setTalkId(const QString& newId);
private slots:
	void onSendBtnClicked(bool);
	void onItemDoubleClicked(QTreeWidgetItem* item,int column);


private:

	void initControl();				
	void initGroupTalkStatus();			//��ʼ��Ⱥ��״̬
	int getCompDepID();					//��ȡ��˾����ID��
	//void initComPanyTalk();			//��ʼ����˾Ⱥ����
	//void initPersonelTalk();			//��ʼ�����²�����
	//void initDevelopTalk();			//��ʼ���з�������
	//void initMarketTalk();				//��ʼ���г�������
	void initTalkWindow();			//��ʼ��Ⱥ��
	void initPtoPTalk();					//��ʼ����������

	//void addPeopInfo(QTreeWidgetItem* pRootGroupItem);
	void addPeopInfo(QTreeWidgetItem* pRootGroupItem, int employeeID);
private:
	Ui::TalkWindow ui;
	QString m_talkId;
	bool m_isGroupTalk;			//�Ƿ�ΪȺ��
	//GroupType m_groupType;
	QMap<QTreeWidgetItem*, QString> m_groupPeopleMap;			//���з�����ϵ������	
	friend class TalkWindowShell;
};
