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
		��Ϊ�����ͼƬ�Ƕ�ͼ������֡�ı��ʱ���ı��༭�����ݵ�λ�ã�ҲҪ��Ӧ�ĸı�
	*/

	//��ӱ��飬�������ID
	void addEmotionUrl(int emotionNum);

	//�����������Ϣʱ��������б��飬Ҫ��յ�
	void deleteAllEmotionImage();

private slots:
	//�����ı�ʱ����Ӧ�Ĳۺ���
	void onEmotionImageFrameChange(int frame);

private:
	//��������
	QList<QString>m_listEmotionUrl;
	
	//����͵�Ӱ��ӳ���ϵ
	//Key��������QMovie* ����ָ�򶯻������ָ�룬��ʾ��̬����
	// Value��ֵ����QString���Ǹñ���ı�ʶ����ͨ���Ǳ���ͼƬ���ļ�·����Ψһ��־
	QMap<QMovie*, QString>m_emotionMap;
};
