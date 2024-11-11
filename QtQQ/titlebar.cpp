#include "titlebar.h"
#include	<QHBoxLayout>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QFile>
#include <QPainterPath>

#define BUTTON_HEIGHT 27		//��ť�߶�27����
#define BUTTON_WIDTH 27		//��ť���27����
#define TITLE_HEIGHT 27			//�������߶�27����
TitleBar::TitleBar(QWidget *parent)
	: QWidget(parent)
	,m_isPressed(false)
	,m_buttonType(MIN_MAX_BUTTON)
{
	initControl();
	initConnections();
	loadStyleSheet("Title");
}

TitleBar::~TitleBar()
{}

//��ʼ���������ؼ�
void TitleBar::initControl()
{
	//���ɶ��󣬲���
	m_pIcon = new QLabel(this);
	m_pTitleContent = new QLabel(this);
	m_pButtonMin = new QPushButton(this);
	m_pButtonRestore = new QPushButton(this);
	m_pButtonMax = new QPushButton(this);
	m_pButtonClose = new QPushButton(this);


	//���ð�ť�̶���С
	m_pButtonMin->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));						//��С��
	m_pButtonRestore->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));				//��󻯻�ԭ��ť
	m_pButtonMax->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));						//���
	m_pButtonClose->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));					//�رհ�ť

	//���ö�����
	m_pTitleContent->setObjectName("TitleContent");				//��������
	m_pButtonMin->setObjectName("ButtonMin");
	m_pButtonRestore->setObjectName("ButtonRestore");
	m_pButtonMax->setObjectName("ButtonMax");
	m_pButtonClose->setObjectName("ButtonClose");

	//���ò���
	QHBoxLayout* mylayout = new QHBoxLayout(this);
	mylayout->addWidget(m_pIcon);											//��Ӹ��ְ�ť
	mylayout->addWidget(m_pTitleContent);
	mylayout->addWidget(m_pButtonMin);
	mylayout->addWidget(m_pButtonRestore);
	mylayout->addWidget(m_pButtonMax);
	mylayout->addWidget(m_pButtonClose);

	mylayout->setContentsMargins(5,0,0,0);				//���ò��ֵļ�϶
	mylayout->setSpacing(0);										//���ò����ﲿ��֮��ļ�϶

	//����ĳߴ����
	//Expanding����ֱ����Fixed���̶�
	m_pTitleContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	setFixedHeight(TITLE_HEIGHT);			//�������߶�
	setWindowFlags(Qt::FramelessWindowHint);				//�ޱ߿�
}

//�ź���۵�����
void TitleBar::initConnections()
{
	connect(m_pButtonMin, SIGNAL(clicked()), this, SLOT(onButtonMinClicked()));
	connect(m_pButtonMax, SIGNAL(clicked()), this, SLOT(onButtonMaxClicked()));
	connect(m_pButtonRestore, SIGNAL(clicked()), this, SLOT(onButtonRestoreClicked()));
	connect(m_pButtonClose,SIGNAL(clicked()), this, SLOT(onButtonCloseClicked()));
}

//���ã�������ͼ��
void TitleBar::setTitleIcon(const QString& filePath)
{
	QPixmap titleIcon(filePath);
	m_pIcon->setFixedSize(titleIcon.size());				//��titleIcon�Ĵ�С,������ͼ���С
	m_pIcon->setPixmap(titleIcon);
}

//���ñ���������
void TitleBar::setTitleContent(const QString& titleContent)
{
	m_pTitleContent->setText(titleContent);		//�����ı�
	m_titleContent = titleContent;						//���ݸ���
}

//���ñ�����������
void TitleBar::setTitleWidth(int width)
{
	setFixedWidth(width);
}

//���ñ���������ť����
void TitleBar::setButtonType(ButtonType buttonType)
{
	m_buttonType = buttonType;			//����

	switch (buttonType)
	{
	case	MIN_BUTTON:
	{
		m_pButtonRestore->setVisible(false);				//setVisible,�Ƿ�ɼ�
		m_pButtonMax->setVisible(false);
	}
	break;
	case MIN_MAX_BUTTON:
	{
		m_pButtonRestore->setVisible(false);
	}
	break;
	case ONLY_CLOSE_BUTTON:
	{
		m_pButtonRestore->setVisible(false);
		m_pButtonMax->setVisible(false);
		m_pButtonMin->setVisible(false);
	}
	break;

	default:
		break;
	}
}

//���洰�����ǰ���ڵ�λ�ü���С
void TitleBar::saveRestoreInfo(const QPoint& point, const QSize& size)
{
	m_restorePos = point;
	m_restoreSize = size;
}

//��ȡ�������ǰ���ڵ�λ�ü���С
void TitleBar::getRestoreInfo(QPoint& point, QSize& size)
{
	point = m_restorePos;
	size = m_restoreSize;
}

//���Ʊ�����
void TitleBar::paintEvent(QPaintEvent* event)
{
	//���ñ���ɫ
	QPainter painter(this);
	QPainterPath pathBack;											//��ͼ·��
	pathBack.setFillRule(Qt::WindingFill);						//����������
	pathBack.addRoundedRect(QRect(0, 0, width(), height()), 3, 3);			//���Բ�Ǿ��ε���ͼ·��
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);		//ƽ��

	//��������󻯻�ԭ�󣬴��ڳ��ȸı䣬��������Ӧ�����ı�
	//parentWidget(),���ظ�����
	if (width() != parentWidget()->width())				//������ parentWidget,���ÿ��
	{
		setFixedWidth(parentWidget()->width());
	}

	//�ѱ������ػ��¼�����ӵ�Ĭ����
	QWidget::paintEvent(event);
}

//˫��������,�������/��С������
void TitleBar::mouseDoubleClickEvent(QMouseEvent* event)
{
	//ֻ�д�����󻯣���С����ť����Ч
	if (m_buttonType == MIN_MAX_BUTTON)
	{
		if (m_pButtonMax->isVisible())
		{
			onButtonMaxClicked();
		}
		else
		{
			onButtonRestoreClicked();
		}
	}
	return QWidget::mouseDoubleClickEvent(event);
}

//ͨ����갴�£�����ƶ�������ͷ��¼�ʵ���϶��������ﵽ�ƶ�����Ч��
void TitleBar::mousePressEvent(QMouseEvent* event)
{
	if (m_buttonType == MIN_MAX_BUTTON)
	{
		//�ڴ������ʱ��ֹ�϶�����
		if (m_pButtonMax->isVisible())
		{
			m_isPressed = true;
			m_startMovePos = event->globalPosition().toPoint();		//event->globalPosition().toPoint()�����¼�����ʱ������ڵ�ȫ��λ��,
														//û����globalPos()����ΪQt 5.15 �����߰汾�ж�����¼���ȫ��λ�ô�������һЩ���������ô����globalPosition()��QPointF���ͣ�
														// m_startMovePosΪQPoint����
		
		}
	}
	else
	{
		m_isPressed = true;
		m_startMovePos = event->globalPosition().toPoint();
	}

	return QWidget::mousePressEvent(event);
}

void TitleBar::mouseMoveEvent(QMouseEvent* event)
{
	if (m_isPressed)
	{
		QPoint movePoint = event->globalPosition().toPoint() - m_startMovePos;		//���ص�ǰ�����λ�� ��ȥ ֮ǰ��λ��
		QPoint widgetPos = parentWidget() ->pos();									//���ز�����λ��
		m_startMovePos = event->globalPosition().toPoint();

		//���ø��������ƶ������������ƶ�ǰ���������
		parentWidget()->move(widgetPos.x() + movePoint.x(), widgetPos.y() + movePoint.y());
	}

	return QWidget::mouseMoveEvent(event);
}

void TitleBar::mouseReleaseEvent(QMouseEvent* event)
{
	m_isPressed = false;
	return QWidget::mouseReleaseEvent(event);
}

//������ʽ��
void TitleBar::loadStyleSheet(const QString& sheetName)
{
	QFile file(":/Resources/QSS/"+sheetName+".css");
	file.open(QFile::ReadOnly);

	if (file.isOpen())
	{
		QString styleSheet = this->styleSheet();			//������ʽ��
		//���ļ��е���ʽ��ȫ���Ͻ�������+=�����������е��ļ����ݽ��ж�ȡ
		//QLatin1String�����ʱ�ַ����ĵ�ַ����Դ����С
		styleSheet += QLatin1String(file.readAll());			//һ����ȫ����ȡ
		setStyleSheet(styleSheet);					//����ʽ���������
	}
}

void TitleBar::onButtonMinClicked()
{
	emit signalButtonMinClicked();
}

void TitleBar::onButtonRestoreClicked()
{
	m_pButtonRestore->setVisible(false);
	m_pButtonMax->setVisible(true);
	emit signalButtonRestoreClicked();
}

void TitleBar::onButtonMaxClicked()
{
	m_pButtonRestore->setVisible(true);
	m_pButtonMax->setVisible(false);
	emit signalButtonMaxClicked();
}

void TitleBar::onButtonCloseClicked()
{
	emit signalButtonCloseClicked();
}









