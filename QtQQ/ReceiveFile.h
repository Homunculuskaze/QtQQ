#pragma once

#include <QWidget>
#include "BasicWindow.h"
#include "ui_ReceiveFile.h"

class ReceiveFile : public BasicWindow
{
	Q_OBJECT

public:
	ReceiveFile(QWidget *parent = nullptr);				// Ĭ���޸�����.���� recvFile ʱδ��ʽָ�������������� new ReceiveFile()������ recvFile �ĸ�����Ϊ nullptr����û�и�����
	~ReceiveFile();



	//��Ϣ���ݴ�����ַ�����������
	void setMsg(QString& msgLabel);

signals:
	void refuseFile();				//�ܾ�

private slots:
	void on_okBtn_clicked();				//ȷ��
	void on_cancelBtn_clicked();					//ȡ��
private:
	Ui::ReceiveFileClass ui;
};
