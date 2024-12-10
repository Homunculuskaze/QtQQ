#pragma once

#include <QWebEngineView>
#include <QDomNode>
#include<QURL>

class MsgHtmlObj :public QObject
{
	Q_OBJECT

	//动态属性
	//（属性类型，属性名称，类成员，属性的类成员，信号，触发信号）
	//m_msgLHtmlTmpl和m_msgRHtmlTmpl，在下面要定义才可以
	Q_PROPERTY(QString msgLHtmlTmpl MEMBER m_msgLHtmlTmpl NOTIFY signalMsgHtml)				//发来的信息
	Q_PROPERTY(QString msgRHtmlTmpl MEMBER m_msgRHtmlTmpl NOTIFY signalMsgHtml)				//我发的信息

public:
	//msgLPicPath，发信息来的人的头像路径
	//MsgHtmlObj(QObject* parent, QString msgLPocPath = "");
	MsgHtmlObj(QObject* parent);

signals:
	void signalMsgHtml(const QString& html);

private:
	void initHtmlTmpl();			//初始化聊天网页

	//初始化网页信息，不管是对方的，还是我的，都要初始化
	//传入一个code，对左右信息，进行初始化
	QString getMsgTmplHtml(const QString& code);

private:
	QString m_msgLHtmlTmpl;					//别人发来的信息
	QString m_msgRHtmlTmpl;				//自己发的信息

	QString m_msgLPicPath;						//发信息来的人的头像路径

};

//重写网页，接收的网页，只接受qrc资源里面的类型
class MsgWebPage :public QWebEnginePage
{
	Q_OBJECT

public:
	MsgWebPage(QObject* parent = nullptr) :QWebEnginePage(parent)
	{

	}

protected:
	//作用：对请求的接收限制，只接收qrc资源里面的类型
	bool acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame);
};

//QWebEngineView视图用于显示网页内容
class MsgWebView  : public QWebEngineView
{
	Q_OBJECT

public:
	MsgWebView(QWidget *parent);
	~MsgWebView();

	//追加信息 是字符串
	//strObj,用来换员工的QQ号
	//external0 external_xxx
	//如果是自己发消息，则默认为0
	//如果不是，则传QQ，然后再拼接
	//void appendMsg(const QString& html,QString strObj="0");
	void appendMsg(const QString& html);

private:
	QList<QStringList> parseHtml(const QString& html);	//解析html
	//Qt中所有DOM节点（属性、说明、文本等）都可以使用QDomNode进行表示
	QList<QStringList>parseDocNode(const QDomNode& node)			//解析节点
	{
		//需要最终解析出来的，是字符串链表
		QList<QStringList> attribute;
		//list保存返回所有的子节点
		const QDomNodeList& list = node.childNodes();

		//list.count(),链表长度
		for (int i = 0; i < list.count(); i++)
		{
			//挨个获取当前链表中的节点
			const QDomNode& node = list.at(i);

			//再对节点进行解析，判断是否为元素
			if (node.isElement())
			{
				//toElement()方法，直接转换成元素
				const QDomElement& element = node.toElement();

				//判断元素名是否为图片
				if (element.tagName() == "img")
				{
					//获取图片的值
					QStringList attributeList;
					//拿到图片的路径(src)，表情，人头像
					attributeList << "img" << element.attribute("src");
					attribute << attributeList;
				}

				if (element.tagName() == "span")
				{
					QStringList attributeList;
					attributeList << "text" << element.text();
					attribute << attributeList;
				}

				//如果这个节点还有子节点
				if (node.hasChildNodes())
				{
					//再继续解析节点，解析之后再将结果存到attribute
					attribute << parseDocNode(node);
				}
			}
		}

		return attribute;
	}

signals:
	void signalSendMsg(QString& strData, int& msgType, QString sFile = "");

private:
	//网页显示的话，需要一个对象
	//Qt与html进行通信，需要借助QWeb网页对象
	MsgHtmlObj* m_msgHtmlObj;

	QWebChannel* m_channel;				//网络通道
};
