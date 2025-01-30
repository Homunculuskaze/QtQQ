//ԭ��
#pragma once	

#include <QWidget>
#include <QMap>
#include <QListWidgetItem>
#include <QTcpSocket>
#include <QStringList> 
#include <QUdpSocket>
#include "ui_TalkWindowShell.h"
#include "basicwindow.h"
#include "EmotionWindow.h"

class TalkWindow;
class TalkWindowItem;
class QListWidgetItem;
class EmotionWindow;

const int gtcpPort = 8888;

enum GroupType {
	COMPANY = 0,		//��˾Ⱥ
	PERSONELGROUP,		//���²�
	DEVELOPMENTGROUP,		//�з���
	MARKETGROUP,				//�г���
	PTOP								//ͬ��֮�䵥������
};

class TalkWindowShell : public BasicWindow
{
	Q_OBJECT

public:
	TalkWindowShell(QWidget *parent = Q_NULLPTR);
	~TalkWindowShell();

public:
	//����µ����촰��
	void addTalkWindow(TalkWindow* talkWindow, TalkWindowItem* talkWindowItem, const QString& uid/*, GroupType grouptype*/);

	//���õ�ǰ���촰��
	void setCurrentWidget(QWidget* widget);

	const QMap<QListWidgetItem*, QWidget*>& getTalkWindowItemMap()const;
private:
	void initControl();				//��ʼ���ؼ�
	void initTcpSocket();				//��ʼ��TCP
	void initUdpSocket();			//��ʼ��UDP
	//QStringList& getEmployeesID();			//��ȡ����Ա��QQ��
	//QStringList getEmployeesID();      // ��ȡ����Ա��QQ��
	void getEmployeesID(QStringList& employeesList);
	bool createJSFile(QStringList& employeesList);			//ԭ��
	void handleReceivedMsg(int senderEmployeeID, int msgType, QString strMsg);
public slots:
	void onEmotionBtnClicked(bool checked);				//���鰴ť�����ִ�еĲۺ���
	void updateSendTcpMsg(QString& strData, int& msgType, QString fileName = "");			//�ͻ��˷���Tcp���ݣ����ݣ��������ͣ��ļ���
private slots:
	   void onTalkWindowItemClicked(QListWidgetItem* item);			//����б�����ִ�еĲۺ���
	   void onEmotionItemClicked(int emotionNum);								//���鱻ѡ��
	   void processPendingData();							//����udp�㲥�յ�������
private:
	Ui::TalkWindowClass ui;
	QMap<QListWidgetItem*, QWidget*>m_talkwindowItemMap;			//�򿪵����촰��
	EmotionWindow* m_emotionWindow;			//���鴰��

private:
	QTcpSocket* m_tcpClientSocket;			//Tcp�ͻ���
	QUdpSocket* m_udpReceiver;				//Udp���ն�
	//gpt�¼�
	QString m_jsFilePath;                                   // �洢 msgtmpl.js ���ļ�·��
};






//#pragma once			//gpt�������õ�
//
//#include <QWidget>
//#include <QMap>
//#include <QListWidgetItem>
//#include <QTcpSocket>
//#include <QStringList> // ȷ������ QStringList
//
//#include "ui_TalkWindowShell.h"
//#include "basicwindow.h"
//#include "EmotionWindow.h"
//
//class TalkWindow;
//class TalkWindowItem;
//
//// TCP �˿ڶ��屣�ֲ���
//const int gtcpPort = 6666;
//
//// Ⱥ������ö�ٱ��ֲ���
//enum GroupType {
//    COMPANY = 0,        // ��˾Ⱥ
//    PERSONELGROUP,      // ���²�
//    DEVELOPMENTGROUP,   // �з���
//    MARKETGROUP,        // �г���
//    PTOP                // ͬ��֮�䵥������
//};
//
//class TalkWindowShell : public BasicWindow
//{
//    Q_OBJECT
//
//public:
//    TalkWindowShell(QWidget* parent = Q_NULLPTR);
//    ~TalkWindowShell();
//
//public:
//    // ����µ����촰��
//    void addTalkWindow(TalkWindow* talkWindow, TalkWindowItem* talkWindowItem, const QString& uid/*, GroupType grouptype*/);
//
//    // ���õ�ǰ���촰��
//    void setCurrentWidget(QWidget* widget);
//
//    // ��ȡ���촰��ӳ��
//    const QMap<QListWidgetItem*, QWidget*>& getTalkWindowItemMap() const;
//
//private:
//    void initControl();				// ��ʼ���ؼ�
//    void initTcpSocket();			// ��ʼ��TCP
//    QStringList getEmployeesID();	// ��ȡ����Ա��QQ�ţ��޸ķ�������
//    bool createJSFile(const QStringList& employeesList, const QString& jsFilePath); // �޸ĺ���ǩ�������� jsFilePath ����
//
//public slots:
//    void onEmotionBtnClicked(bool checked);				// ���鰴ť�����ִ�еĲۺ���
//    void updateSendTcpMsg(QString& strData, int& msgType, QString sFile = "");	// �ͻ��˷���Tcp���ݣ����ݣ��������ͣ��ļ���
//
//private slots:
//    void onTalkWindowItemClicked(QListWidgetItem* item);	// ����б�����ִ�еĲۺ���
//    void onEmotionItemClicked(int emotionNum);				// ���鱻ѡ��
//
//private:
//    Ui::TalkWindowClass ui;										// UIָ��
//    QMap<QListWidgetItem*, QWidget*> m_talkwindowItemMap;		// �򿪵����촰��
//    EmotionWindow* m_emotionWindow;							// ���鴰��
//    QTcpSocket* m_tcpClientSocket;								// Tcp�ͻ���
//};
//

