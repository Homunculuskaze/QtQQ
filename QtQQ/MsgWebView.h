#pragma once

#include <QWebEngineView>
#include <QDomNode>
#include<QURL>

class MsgHtmlObj :public QObject
{
	Q_OBJECT

	//��̬����
	//���������ͣ��������ƣ����Ա�����Ե����Ա���źţ������źţ�
	//m_msgLHtmlTmpl��m_msgRHtmlTmpl��������Ҫ����ſ���
	Q_PROPERTY(QString msgLHtmlTmpl MEMBER m_msgLHtmlTmpl NOTIFY signalMsgHtml)				//��������Ϣ
	Q_PROPERTY(QString msgRHtmlTmpl MEMBER m_msgRHtmlTmpl NOTIFY signalMsgHtml)				//�ҷ�����Ϣ

public:
	//msgLPicPath������Ϣ�����˵�ͷ��·��
	//MsgHtmlObj(QObject* parent, QString msgLPocPath = "");
	MsgHtmlObj(QObject* parent);

signals:
	void signalMsgHtml(const QString& html);

private:
	void initHtmlTmpl();			//��ʼ��������ҳ

	//��ʼ����ҳ��Ϣ�������ǶԷ��ģ������ҵģ���Ҫ��ʼ��
	//����һ��code����������Ϣ�����г�ʼ��
	QString getMsgTmplHtml(const QString& code);

private:
	QString m_msgLHtmlTmpl;					//���˷�������Ϣ
	QString m_msgRHtmlTmpl;				//�Լ�������Ϣ

	QString m_msgLPicPath;						//����Ϣ�����˵�ͷ��·��

};

//��д��ҳ�����յ���ҳ��ֻ����qrc��Դ���������
class MsgWebPage :public QWebEnginePage
{
	Q_OBJECT

public:
	MsgWebPage(QObject* parent = nullptr) :QWebEnginePage(parent)
	{

	}

protected:
	//���ã�������Ľ������ƣ�ֻ����qrc��Դ���������
	bool acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame);
};

//QWebEngineView��ͼ������ʾ��ҳ����
class MsgWebView  : public QWebEngineView
{
	Q_OBJECT

public:
	MsgWebView(QWidget *parent);
	~MsgWebView();

	//׷����Ϣ ���ַ���
	//strObj,������Ա����QQ��
	//external0 external_xxx
	//������Լ�����Ϣ����Ĭ��Ϊ0
	//������ǣ���QQ��Ȼ����ƴ��
	//void appendMsg(const QString& html,QString strObj="0");
	void appendMsg(const QString& html);

private:
	QList<QStringList> parseHtml(const QString& html);	//����html
	//Qt������DOM�ڵ㣨���ԡ�˵�����ı��ȣ�������ʹ��QDomNode���б�ʾ
	QList<QStringList>parseDocNode(const QDomNode& node)			//�����ڵ�
	{
		//��Ҫ���ս��������ģ����ַ�������
		QList<QStringList> attribute;
		//list���淵�����е��ӽڵ�
		const QDomNodeList& list = node.childNodes();

		//list.count(),������
		for (int i = 0; i < list.count(); i++)
		{
			//������ȡ��ǰ�����еĽڵ�
			const QDomNode& node = list.at(i);

			//�ٶԽڵ���н������ж��Ƿ�ΪԪ��
			if (node.isElement())
			{
				//toElement()������ֱ��ת����Ԫ��
				const QDomElement& element = node.toElement();

				//�ж�Ԫ�����Ƿ�ΪͼƬ
				if (element.tagName() == "img")
				{
					//��ȡͼƬ��ֵ
					QStringList attributeList;
					//�õ�ͼƬ��·��(src)�����飬��ͷ��
					attributeList << "img" << element.attribute("src");
					attribute << attributeList;
				}

				if (element.tagName() == "span")
				{
					QStringList attributeList;
					attributeList << "text" << element.text();
					attribute << attributeList;
				}

				//�������ڵ㻹���ӽڵ�
				if (node.hasChildNodes())
				{
					//�ټ��������ڵ㣬����֮���ٽ�����浽attribute
					attribute << parseDocNode(node);
				}
			}
		}

		return attribute;
	}

signals:
	void signalSendMsg(QString& strData, int& msgType, QString sFile = "");

private:
	//��ҳ��ʾ�Ļ�����Ҫһ������
	//Qt��html����ͨ�ţ���Ҫ����QWeb��ҳ����
	MsgHtmlObj* m_msgHtmlObj;

	QWebChannel* m_channel;				//����ͨ��
};
