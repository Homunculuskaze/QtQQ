#include "basicwindow.h"
#include "CommonUtils.h"
#include "NotifyManager.h"

//#include <QDesktopWidget>
#include <QScreen>
#include <QFile>
#include <QStyleOption>
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>
#include <QSqlQuery>

//�ⲿȫ�ֱ�������¼��ID
extern QString gLoginEmployeeID;

BasicWindow::BasicWindow(QWidget* parent)
	:QDialog(parent)
{
	//��ȡĬ�ϵ���ɫֵ
	m_colorBackGround = CommonUtils::getDefaultSkinColor();

	//�����ô��ڷ��
	setWindowFlags(Qt::FramelessWindowHint);			//û�б߿�
	//setAttribute(Qt::WA_TranslucentBackground, true);				//͸��Ч����qt6ʹ�ú�Ƥ������û�б߿�����,qt6��qt5�ò��

	//�������Ƥ���źţ��޸�Ƥ��
	connect(NotifyManager::getInstance(), SIGNAL(signalSkinChanged(const QColor&)), this, SLOT(onSignalSkinChanged(const QColor&)));

	
}

BasicWindow::~BasicWindow()
{}


void BasicWindow::onSignalSkinChanged(const QColor& color)
{
	//�޸���ɫ��������ʽ���������
	m_colorBackGround = color;
	loadStyleSheet(m_styleName);
}

void BasicWindow::setTitleBarTitle(const QString& title, const QString& icon)
{
	m_titleBar->setTitleIcon(icon);
	m_titleBar->setTitleContent(title);
}

void BasicWindow::iniTitleBar(ButtonType buttontype)
{
	m_titleBar = new TitleBar(this);
	m_titleBar->setButtonType(buttontype);
	m_titleBar->move(0, 0);

	connect(m_titleBar, SIGNAL(signalButtonMinClicked()), this, SLOT(onButtonMinClicked()));
	connect(m_titleBar, SIGNAL(signalButtonRestoreClicked()), this, SLOT(onButtonRestoreClicked()));
	connect(m_titleBar, SIGNAL(signalButtonMaxClicked()), this, SLOT(onButtonMaxClicked()));
	connect(m_titleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(onButtonCloseClicked()));
}

void BasicWindow::loadStyleSheet(const QString& sheetName)
{
	m_styleName = sheetName;
	QFile file(":/Resources/QSS/" + sheetName + ".css");
	file.open(QFile::ReadOnly);

	if (file.isOpen())
	{
		setStyleSheet("");			//������ʽ��
		QString qsstyleSheet = QLatin1String(file.readAll());			//��������ȫ����ȡ���������浽qsstyleSheet�ַ���

		//��ȡ��ǰ�û���Ƥ��RGBֵ
		QString r = QString::number(m_colorBackGround.red());
		QString g = QString::number(m_colorBackGround.green());
		QString b = QString::number(m_colorBackGround.blue());

		//titleskin����Ƥ������Ϊtrue
		//�ײ���Ƥ��bottomskinΪtrue
		//rgba�� a ��͸����
		qsstyleSheet += QString("QWidget[titleskin=true]\
												{background-color:rgb(%1,%2,%3);\
												 border-top-left-radius:4px;}\
												 QWidget[bottomskin=true]\
												{border-top:1px solid rgba(%1,%2,%3,100);\
												background-color:rgba(%1,%2,%3,50);\
												border-bottom-left-radius:4px;\
												border-bottom-right-radius:4px;}")
												.arg(r).arg(g).arg(b);
										//border-bottom-right-radius:4px;}")��û�м�'\'��������δ�������һ�����Ѿ����������� ") ��������û�м������е�����
										//background-color:rgba(%1,%2,%3,50); 50��͸����
										//border - bottom - left - radius:4px; ��б��
		//������ʽ��
		setStyleSheet(qsstyleSheet);
	}

	file.close();
}

//����ͼ
void BasicWindow::initBackGroundColor()
{
	QStyleOption opt;
	//opt.init(this);
	opt.initFrom(this);			//ʹ��initFrom ����ʼ�� QStyleOption

	QPainter p(this);			//��this��"��ǰ"����
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);					//ͼԴ�����
}

//���໯����ʱ����Ҫ��д��ͼ�¼����ñ���ͼ
void BasicWindow::paintEvent(QPaintEvent* event)
{
	initBackGroundColor();
	QDialog::paintEvent(event);
}


//ͷ��תԲͷ��
QPixmap BasicWindow::getRoundImage(const QPixmap& src, QPixmap& mask, QSize maskSize)		//srcΪͼƬ����Դ��maskΪԲͷ��maskSizeԲͷ���С
{
	if (maskSize == QSize(0, 0))
	{
		maskSize = mask.size();
	}
	else
	{
		// Qt::KeepAspectRatio , ����ʱ, �������Դ�ľ�������
		// Qt::SmoothTransformation , ƽ����ʽ
		mask=mask.scaled(maskSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}

	//����ת�����ͼ��
	QImage resultImage(maskSize, QImage::Format_ARGB32_Premultiplied);
	QPainter painter(&resultImage);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.fillRect(resultImage.rect(), Qt::transparent);
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painter.drawPixmap(0, 0, mask);
	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
	painter.drawPixmap(0, 0, src.scaled(maskSize,Qt::KeepAspectRatio, Qt::SmoothTransformation));
	painter.end();

	return QPixmap::fromImage(resultImage);
}

void BasicWindow::onShowClose(bool)
{
	close();
}

void BasicWindow::onShowMin(bool)
{
	showMinimized();
}

void BasicWindow::onShowHide(bool)			//����
{
	hide();
}

void BasicWindow::onShowNormal(bool)
{
	show();
	activateWindow();
}

void BasicWindow::onShowQuit(bool)
{
	//���µ�¼״̬Ϊ"����"
	QString strSqlStatus = QString("UPDATE tab_employees SET online_status = 1 WHERE employeeID = %1").arg(gLoginEmployeeID);
	QSqlQuery queryStatus(strSqlStatus);			//queryStatusֻ�Ǹ���������������췽ʽʵ�����ǵ��� QSqlQuery(const QString &query, QSqlDatabase db = QSqlDatabase())�������Զ�ʹ��Ĭ�����ݿ⣬���� strSqlStatus ��Ϊ��ѯ��䡣
	queryStatus.exec();

	QApplication::quit();
}

//����ƶ��¼�
void BasicWindow::mouseMoveEvent(QMouseEvent* e)
{
	if (m_mousePressed && (e->buttons() && Qt::LeftButton))
	{
		//e->globalPosition().toPoint()�¼�����ʱȫ�����꣬�������Ļ���Ͻǣ�0��0��
		move(e->globalPosition().toPoint() - m_mousePoint);						//globalPos() , ��ȡ��ǰ�¼�����ʱ, ���ȫ�ֵ�����ֵ
			e->accept();
	}
}

//���İ����¼�
void BasicWindow::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton)
	{
		//pos(),���鷢��ʱ������ڴ������Ͻǵ�ƫ������
		m_mousePressed = true;
		m_mousePoint = e->globalPosition().toPoint() - pos();			//��ȥ����ֵ��pos()�¼�����ʱ����ڴ������Ͻǣ�0��0����ƫ������
		e->accept();
	}
}

//����ͷ��¼�
void BasicWindow::mouseReleaseEvent(QMouseEvent* event)
{
	m_mousePressed = false;
}

void BasicWindow::onButtonMinClicked()
{
	if (Qt::Tool == (windowFlags() & Qt::Tool))
	{
		hide();
	}
	else
	{
		showMinimized();
	}
}

//��󻯻�ԭ��ť
void BasicWindow::onButtonRestoreClicked()
{
	QPoint windowPos;
	QSize windowSize;
	m_titleBar->getRestoreInfo(windowPos, windowSize);
	setGeometry(QRect(windowPos, windowSize));
}

void BasicWindow::onButtonMaxClicked()
{
	m_titleBar->saveRestoreInfo(pos(), QSize(width(), height()));
	//QRect desktopRect = QApplication::desktop()->availableGeometry();
	QScreen* screen = QGuiApplication::primaryScreen();
	QRect desktopRect = screen->availableGeometry();
	QRect factRect = QRect(desktopRect.x()-3,desktopRect.y()-3,
			desktopRect.width() + 6, desktopRect.height() + 6);

	setGeometry(factRect);
}


void BasicWindow::onButtonCloseClicked()
{
	close();
}