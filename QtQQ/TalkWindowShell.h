//原版
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
	void initTcpSocket();				//初始化TCP
	void initUdpSocket();			//初始化UDP
	//QStringList& getEmployeesID();			//获取所有员工QQ号
	//QStringList getEmployeesID();      // 获取所有员工QQ号
	void getEmployeesID(QStringList& employeesList);
	bool createJSFile(QStringList& employeesList);			//原版
	void handleReceivedMsg(int senderEmployeeID, int msgType, QString strMsg);
public slots:
	void onEmotionBtnClicked(bool checked);				//表情按钮点击后执行的槽函数
	void updateSendTcpMsg(QString& strData, int& msgType, QString fileName = "");			//客户端发送Tcp数据（数据，数据类型，文件）
private slots:
	   void onTalkWindowItemClicked(QListWidgetItem* item);			//左侧列表点击后执行的槽函数
	   void onEmotionItemClicked(int emotionNum);								//表情被选中
	   void processPendingData();							//处理udp广播收到的数据
private:
	Ui::TalkWindowClass ui;
	QMap<QListWidgetItem*, QWidget*>m_talkwindowItemMap;			//打开的聊天窗口
	EmotionWindow* m_emotionWindow;			//表情窗口

private:
	QTcpSocket* m_tcpClientSocket;			//Tcp客户端
	QUdpSocket* m_udpReceiver;				//Udp接收端
	//gpt新加
	QString m_jsFilePath;                                   // 存储 msgtmpl.js 的文件路径
};






//#pragma once			//gpt给的能用的
//
//#include <QWidget>
//#include <QMap>
//#include <QListWidgetItem>
//#include <QTcpSocket>
//#include <QStringList> // 确保包含 QStringList
//
//#include "ui_TalkWindowShell.h"
//#include "basicwindow.h"
//#include "EmotionWindow.h"
//
//class TalkWindow;
//class TalkWindowItem;
//
//// TCP 端口定义保持不变
//const int gtcpPort = 6666;
//
//// 群组类型枚举保持不变
//enum GroupType {
//    COMPANY = 0,        // 公司群
//    PERSONELGROUP,      // 人事部
//    DEVELOPMENTGROUP,   // 研发部
//    MARKETGROUP,        // 市场部
//    PTOP                // 同事之间单独聊天
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
//    // 添加新的聊天窗口
//    void addTalkWindow(TalkWindow* talkWindow, TalkWindowItem* talkWindowItem, const QString& uid/*, GroupType grouptype*/);
//
//    // 设置当前聊天窗口
//    void setCurrentWidget(QWidget* widget);
//
//    // 获取聊天窗口映射
//    const QMap<QListWidgetItem*, QWidget*>& getTalkWindowItemMap() const;
//
//private:
//    void initControl();				// 初始化控件
//    void initTcpSocket();			// 初始化TCP
//    QStringList getEmployeesID();	// 获取所有员工QQ号，修改返回类型
//    bool createJSFile(const QStringList& employeesList, const QString& jsFilePath); // 修改函数签名，增加 jsFilePath 参数
//
//public slots:
//    void onEmotionBtnClicked(bool checked);				// 表情按钮点击后执行的槽函数
//    void updateSendTcpMsg(QString& strData, int& msgType, QString sFile = "");	// 客户端发送Tcp数据（数据，数据类型，文件）
//
//private slots:
//    void onTalkWindowItemClicked(QListWidgetItem* item);	// 左侧列表点击后执行的槽函数
//    void onEmotionItemClicked(int emotionNum);				// 表情被选中
//
//private:
//    Ui::TalkWindowClass ui;										// UI指针
//    QMap<QListWidgetItem*, QWidget*> m_talkwindowItemMap;		// 打开的聊天窗口
//    EmotionWindow* m_emotionWindow;							// 表情窗口
//    QTcpSocket* m_tcpClientSocket;								// Tcp客户端
//};
//

