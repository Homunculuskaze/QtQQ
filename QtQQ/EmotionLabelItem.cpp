#include "EmotionLabelItem.h"
#include <QMovie>

EmotionLabelItem::EmotionLabelItem(QWidget *parent)
	: QClickLabel(parent)
{
	initControl();
	//点击表情，发射信号
	connect(this, &QClickLabel::clicked, [this]
		{
			emit emotionClicked(m_emotionName);
		});
}

EmotionLabelItem::~EmotionLabelItem()
{}

void EmotionLabelItem::setEmotionName(int emotionName)
{
	m_emotionName = emotionName;
	QString imageName = QString(":/Resources/MainWindow/emotion/%1.png").arg(emotionName);

	//因为是动图，所以要设置QMovie
	//QMovie(路径，格式，父类)；
	m_apngMovie = new QMovie(imageName, "apng", this);

	m_apngMovie->start();				//放映
	m_apngMovie->stop();				//暂停
	setMovie(m_apngMovie);			//设置，对当前的标签设置电影
}

void EmotionLabelItem::initControl()
{
	setAlignment(Qt::AlignCenter);			//居中对齐
	setObjectName("emotionLabelItem");		//设置对象名
	setFixedSize(32, 32);				//每一个表情所占像素
}
