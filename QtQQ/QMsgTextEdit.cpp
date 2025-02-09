#include "QMsgTextEdit.h"
#include <QMovie>
#include <QUrl>

QMsgTextEdit::QMsgTextEdit(QWidget *parent)
	: QTextEdit(parent)
{}

QMsgTextEdit::~QMsgTextEdit()
{
	//删除所有图片
	deleteAllEmotionImage();
}

void QMsgTextEdit::addEmotionUrl(int emotionNum)
{
	//添加表情
	//先获得表情的名称
	const QString& imageName = QString("qrc:/Resources/MainWindow/emotion/%1.png").arg(emotionNum);

	//进行图片拼接
	const QString& flagName = QString("%1").arg(imageName);

	//插入表情
	insertHtml(QString("<img src='%1'/>").arg(flagName));

	//判断，网页里面是否包含
	//如果包含就直接返回，啥也不干
	//如果不包含，就追加
	if (m_listEmotionUrl.contains(imageName))
	{
		return;
	}
	else
	{
		m_listEmotionUrl.append(imageName);
	}

	//表情映射管理
	QMovie* apngMovie = new QMovie(imageName, "apng", this);

	//添加到表情映射里，参数（插入谁的名字）
	m_emotionMap.insert(apngMovie, flagName);

	//数据帧数改变时，发射的信号
	//frameChanged(int),帧改变
	connect(apngMovie, SIGNAL(frameChanged(int)), this, SLOT(onEmotionImageFrameChange(int)));

	apngMovie->start();
	updateGeometry();				//更新几何形状
}

void QMsgTextEdit::deleteAllEmotionImage()
{
	//Key（键）：QMovie* ，是指向动画对象的指针，表示动态表情
	// Value（值）：QString，是该表情的标识符，通常是表情图片的文件路径或唯一标志
	//初始化映射起始，只要不等于映射结束，就itor++
	for (auto itor = m_emotionMap.constBegin(); itor != m_emotionMap.constEnd(); itor++)
	{
		//删除Key
		delete itor.key();
	}

	//删除之后，再进行清除映射
	m_emotionMap.clear();
}
void QMsgTextEdit::onEmotionImageFrameChange(int frame)
{
	//图片改变
	//先获取动图就是QMovie，通过sender()得到信号发送者
	//sender()的类型时QObject，需要进行转换
	QMovie* movie = qobject_cast<QMovie*>(sender());

	//动画进行改变之后，要对当前文档区进行重新添加资源
	//使用document（）获取当前文档，然后添加资源
	//addResource(资源类型，URL模式)
	//当前添加的时图片资源，资源的URL
	//这里使用的模式，就是图片自己的模式调用  currentPixmap()
	document()->addResource(QTextDocument::ImageResource, QUrl(m_emotionMap.value(movie)), movie->currentPixmap());
}
