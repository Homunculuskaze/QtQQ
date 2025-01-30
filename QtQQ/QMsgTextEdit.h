#pragma once

#include <QTextEdit>

class QMsgTextEdit  : public QTextEdit
{
	Q_OBJECT

public:
	QMsgTextEdit(QWidget *parent = nullptr);
	~QMsgTextEdit();

public:
	/*
		因为插入的图片是动图，所以帧改变的时候，文本编辑器内容的位置，也要相应的改变
	*/

	//添加表情，传入表情ID
	void addEmotionUrl(int emotionNum);

	//当点击发送信息时，输入框中表情，要清空掉
	void deleteAllEmotionImage();

private slots:
	//动画改变时，响应的槽函数
	void onEmotionImageFrameChange(int frame);

private:
	//表情链表
	QList<QString>m_listEmotionUrl;
	
	//表情和电影的映射关系
	//Key（键）：QMovie* ，是指向动画对象的指针，表示动态表情
	// Value（值）：QString，是该表情的标识符，通常是表情图片的文件路径或唯一标志
	QMap<QMovie*, QString>m_emotionMap;
};
