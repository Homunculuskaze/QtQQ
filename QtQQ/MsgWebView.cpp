#include "MsgWebView.h"

#include <QFile>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QWebChannel>
MsgHtmlObj::MsgHtmlObj(QObject* parent) :QObject(parent)
{
	initHtmlTmpl();
}

void MsgHtmlObj::initHtmlTmpl()
{
	m_msgLHtmlTmpl = getMsgTmplHtml("msgleftTmpl");
	m_msgRHtmlTmpl = getMsgTmplHtml("msgrightTmpl");
}

QString MsgHtmlObj::getMsgTmplHtml(const QString& code)
{
	//������ȫ����ȡ������Ȼ���ٷ���
	QFile file(":/Resources/MainWindow/MsgHtml/" + code + ".html");

	file.open(QFile::ReadOnly);
	QString strData;

	if (file.isOpen())
	{
		//ȫ��һ���Զ�ȡ
		strData = QLatin1String(file.readAll());
	}
	else
	{
		//information ��������ָ�룬���⣬��ʾ���ݣ�
		//��Ϊ��ǰд����Ķ����Ǵ�QObject�������ģ����Ǵ�ĳ��������������
		//���Ҫд��
		QMessageBox::information(nullptr, "Tips", "Failed to init html!");
		return QString("");
	}
	file.close();

	return strData;
}

MsgWebView::MsgWebView(QWidget *parent)
	: QWebEngineView(parent)
{
	MsgWebPage* page = new MsgWebPage(this);
	setPage(page);

	QWebChannel* channel = new QWebChannel(this);
	m_msgHtmlObj = new MsgHtmlObj(this);
	channel->registerObject("external", m_msgHtmlObj);
	this->page()->setWebChannel(channel);

	//��ʼ��������Ϣҳ��
	this->load(QUrl("qrc:/Resources/MainWindow/MsgHtml/msgTmpl.html"));
}

MsgWebView::~MsgWebView()
{}

void MsgWebView::appendMsg(const QString& html)
{
	QJsonObject msgObj;
	QString qsMsg;
	const QList<QStringList>msgList = parseHtml(html);			//����html


	//��Ϣ���ͣ�Ĭ�ϸ�1
	//0�Ǳ��飬1���ı���2���ļ�
	//int msgType = 1;
	//bool isImageMsg = false;			//Ĭ�ϵ�ͼƬ��Ϣ
	//int imageNum = 0;				//����������Ĭ�ϸ�0
	//QString strData;					//055��008���������ƣ���Ȳ�����0��ȫ

	for (int i = 0; i < msgList.size(); i++)
	{
		//���msgList��Ȩѭ��������ĵ�һ��Ԫ�� �� img�ļ�
		if (msgList.at(i).at(0) == "img")
		{
			//��ȡͼƬ·��
			//����д1��0��img��1��ͼƬ����ʵ����
			QString imagePath = msgList.at(i).at(1);
			QPixmap pixmap;			//���ڼ���ͼƬ

			//��ȡ�������Ƶ�λ��
			//����·��
			//QString strEmotionPath = "qrc:/Resources/MainWindow/emotion/";
			//int pos = strEmotionPath.size();			//����·������
			//isImageMsg = true;

			//��ȡ��������
			//ͨ��mid()��������ȡ�����ַ����еĲ����ַ���
			//QString strEmotionName = imagePath.mid(pos);
			//�ɡ�.png���á��ա��滻��ֱ�Ӹı�ľ���strEmotionName����
			//strEmotionName.replace(".png", "");

			//���ݱ������Ƶĳ��ȣ��������ñ�������
			//���� 3λ������3λ����23.png,����Ϊ023.png
			//int emotionNameLength = strEmotionName.length();

			//�жϳ���
			//if (emotionNameLength == 1)
			//{
			//	strData = strData + "00" + strEmotionName;
			//}
			//else if (emotionNameLength == 2)
			//{
			//	strData = strData + "0" + strEmotionName;
			//}
			//else if (emotionNameLength == 3)
			//{
			//	strData = strData + strEmotionName;
			//}

			//msgType = 0;			//������Ϣ
			//imageNum++;

			//�ж�����ߵ�3���ַ����Ƿ�Ϊqrc
			if (imagePath.left(3) == "qrc")				//����·��Ϊ qrc:/MainWindow/xxx
			{
				//mid()���������м�λ�ÿ�ʼ��ȡ
				//�ӵ������ַ���ʼ��ȡ������":"ð�ſ�ʼ
				//ȥ������·����qrc
				pixmap.load(imagePath.mid(3));		//��λ��3��ʼ��ȡ��λ��0��q������		:/MainWindow/xxx
			}
			else
			{
				//����qrc�ģ���ֱ�Ӽ���
				pixmap.load(imagePath);
			}

			//����ͼƬhtml��ʽ���ı���ϣ�·������ȣ��߶ȣ�
			QString imgPath=QString("<img src=\"%1\"width=\"%2\"heigh=\"%3\"/>")
				.arg(imagePath).arg(pixmap.width()).arg(pixmap.height());			//\" ��ʾ���ַ����в���һ��˫�����ַ���"��

			//�浽�ַ�����
			qsMsg += imgPath;
		}
		else if (msgList.at(i).at(0) == "text")
		{
			//����д1��0��text��1���ı�����ʵ����
			qsMsg += msgList.at(i).at(1);
			//strData = qsMsg;				//��ֵ
		}
	}

	//���뵽Json�����У��Ǽ�ֵ�ԣ�
	//��MSG���Ǽ���qsMsg��ֵ
	//��������Դ�ļ��У���msgleftTmpl.htmlΪ��
	//����������еġ�MSG��<div class="msg">{{MSG}}<span class>="trigon"
	msgObj.insert("MSG", qsMsg);


	//һ�е�Ŀ�ģ�����Ϊ��ת��QString�ַ�������
	//��ת����Json�ĵ�������Ҫת��UTF-8���ĵ�
	//QJsonDocument::Compact�����յ���˼
	const QString& Msg = QJsonDocument(msgObj).toJson(QJsonDocument::Compact);
	this->page()->runJavaScript(QString("appendHtml(%1)").arg(Msg));
	//����Ϣ
	//if (strObj == "0")
	//{
	//	//����Java�ű�����
	//	this->page()->runJavaScript(QString("appendHtml0(%1)").arg(Msg));

	//	//������͵��Ǳ��飬�þͶԷ��͵����ݽ��д���
	//	if (isImageMsg)
	//	{
	//		//��ǰstrData�����ֻ�Ǳ�������ƣ�ռλ��3�����
	//		//������ϱ��������
	//		strData = QString::number(imageNum) + "images" + strData;
	//	}

	//	//�����źţ�������Ϣ
	//	emit signalSendMsg(strData, msgType);
	//}
	//else    //����
	//{
	//	//׷�����ݣ��������recvHtml
	//	this->page()->runJavaScript(QString("recvHtml_%1(%2)").arg(strObj).arg(Msg));
	//}
}

QList<QStringList> MsgWebView::parseHtml(const QString & html)
{
	//��Ϊ����������html�ļ�������ת����Qt�ڵ��ļ�
	QDomDocument doc;
	doc.setContent(html);				//ת��

	//��Ҫ�����Ľڵ㣬����html�����body
	//���õ�body����Ҫ�Ȼ�ȡԪ��
	//�ڵ�Ԫ��
	const QDomElement& root = doc.documentElement();
	//��ȡ��һ��Ԫ�أ�node�ǽڵ����ͣ�����Ҫ�Խڵ���н���
	const QDomNode& node = root.firstChildElement("body");

	return parseDocNode(node);
}

bool MsgWebPage::acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame)
{
	//������ qrc:/*.html
	//�ж�URL���ͣ��Ƿ�Ϊqrc����
	if (url.scheme() == QString("qrc"))
	{
		return true;
	}
	else
	{
		return false;
	}
}
