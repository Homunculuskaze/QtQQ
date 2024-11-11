#include "SkinWindow.h"
#include "QClickLabel.h"
#include "NotifyManager.h"

SkinWindow::SkinWindow(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);

	loadStyleSheet("SkinWindow");				//������ʽ��
	setAttribute(Qt::WA_DeleteOnClose);		//�������յ��ر��¼�ʱ�򣬾ͻ�ر�
	initControl();											//��ʼ��
}

SkinWindow::~SkinWindow()
{}

void SkinWindow::initControl()
{
	//��������ɫ
	QList<QColor> colorList = {
		QColor(22,154,218),QColor(40,138,221),QColor(49,166,107),QColor(218,67,68),
		QColor(177,99,158),QColor(107,81,92),QColor(89,92,160),QColor(21,156,199),
		QColor(79,169,172),QColor(155,183,154),QColor(128,77,77),QColor(240,188,189),
	};

	//�ٰ���ɫ����������
	for (int row = 0; row < 3; row++)
	{
		for (int column = 0; column < 4; column++)
		{
			QClickLabel* label = new QClickLabel(this);
			label->setCursor(Qt::PointingHandCursor);

			//C++11,lambda����ʽ
			//[row,column,colorList()]{},�źŽ�����
			// row,column,colorList()����lambda�ⲿ����ı�����ͨ�������б����ݸ�lambda�Թ��ڲ�ʹ��
			//����źŷ����ʱ��ֱ��ִ��{}����Ĵ��룬
			//��ʵ{}���ǲۺ�����ֻ��̫�����ۺ���������ȡ����
			//[]�����ǲ���ı���
			//QClickLabel.h�е��źź���void clicked();
			//�Ͳ�Ҫ��cpp�ļ�����д�ˣ������޷����У���Ϊ�ź���û�к������
			connect(label, &QClickLabel::clicked, [row, column, colorList]() {
				NotifyManager::getInstance()->notifyOtherWindowChangeSkin(colorList.at(row * 4 + column));		//һ��4����ɫ
				});

			label->setFixedSize(84, 84);		//84����84��

			QPalette palette;
			//palette.setColor(QPalette::Background, colorList.at(row * 4 + column));
			palette.setColor(QPalette::Window, colorList.at(row * 4 + column));		//qt6������QPalette::Background,��ΪQPalette::Window��Ϊ����ɫ
			label->setAutoFillBackground(true);
			label->setPalette(palette);

			ui.gridLayout->addWidget(label, row, column);		//��bodyWidget���������ã�Layout�����layoutName������ΪgridLayout
		}
	}

	//��С�������
	connect(ui.sysmin, SIGNAL(clicked(bool)), this, SLOT(onShowMin(bool)));
	connect(ui.sysclose, SIGNAL(clicked()), this, SLOT(onShowClose()));
}

void SkinWindow::onShowClose()
{
	close();
}