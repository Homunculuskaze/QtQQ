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

//外部全局变量，登录者ID
extern QString gLoginEmployeeID;

BasicWindow::BasicWindow(QWidget* parent)
	:QDialog(parent)
{
	//获取默认得颜色值
	m_colorBackGround = CommonUtils::getDefaultSkinColor();

	//先设置窗口风格
	setWindowFlags(Qt::FramelessWindowHint);			//没有边框
	//setAttribute(Qt::WA_TranslucentBackground, true);				//透明效果，qt6使用后皮肤窗口没有边框线条,qt6和qt5得差别

	//发射更改皮肤信号，修改皮肤
	connect(NotifyManager::getInstance(), SIGNAL(signalSkinChanged(const QColor&)), this, SLOT(onSignalSkinChanged(const QColor&)));

	
}

BasicWindow::~BasicWindow()
{}


void BasicWindow::onSignalSkinChanged(const QColor& color)
{
	//修改颜色，加载样式表，重新添加
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
		setStyleSheet("");			//设置样式表
		QString qsstyleSheet = QLatin1String(file.readAll());			//数据内容全部读取出来，保存到qsstyleSheet字符串

		//获取当前用户的皮肤RGB值
		QString r = QString::number(m_colorBackGround.red());
		QString g = QString::number(m_colorBackGround.green());
		QString b = QString::number(m_colorBackGround.blue());

		//titleskin标题皮肤属性为true
		//底部的皮肤bottomskin为true
		//rgba的 a 是透明度
		qsstyleSheet += QString("QWidget[titleskin=true]\
												{background-color:rgb(%1,%2,%3);\
												 border-top-left-radius:4px;}\
												 QWidget[bottomskin=true]\
												{border-top:1px solid rgba(%1,%2,%3,100);\
												background-color:rgba(%1,%2,%3,50);\
												border-bottom-left-radius:4px;\
												border-bottom-right-radius:4px;}")
												.arg(r).arg(g).arg(b);
										//border-bottom-right-radius:4px;}")后没有加'\'是由于这段代码的最后一部分已经结束在引号 ") 处，而且没有继续分行的需求
										//background-color:rgba(%1,%2,%3,50); 50是透明度
										//border - bottom - left - radius:4px; 是斜率
		//设置样式表
		setStyleSheet(qsstyleSheet);
	}

	file.close();
}

//背景图
void BasicWindow::initBackGroundColor()
{
	QStyleOption opt;
	//opt.init(this);
	opt.initFrom(this);			//使用initFrom 来初始化 QStyleOption

	QPainter p(this);			//用this是"当前"窗口
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);					//图源，风格
}

//子类化部件时，需要重写绘图事件设置背景图
void BasicWindow::paintEvent(QPaintEvent* event)
{
	initBackGroundColor();
	QDialog::paintEvent(event);
}


//头像转圆头像
QPixmap BasicWindow::getRoundImage(const QPixmap& src, QPixmap& mask, QSize maskSize)		//src为图片的来源，mask为圆头像，maskSize圆头像大小
{
	if (maskSize == QSize(0, 0))
	{
		maskSize = mask.size();
	}
	else
	{
		// Qt::KeepAspectRatio , 缩放时, 尽可能以大的矩形缩放
		// Qt::SmoothTransformation , 平滑方式
		mask=mask.scaled(maskSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}

	//保存转换后的图像
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

void BasicWindow::onShowHide(bool)			//隐藏
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
	//更新登录状态为"离线"
	QString strSqlStatus = QString("UPDATE tab_employees SET online_status = 1 WHERE employeeID = %1").arg(gLoginEmployeeID);
	QSqlQuery queryStatus(strSqlStatus);			//queryStatus只是个变量名，这个构造方式实际上是调用 QSqlQuery(const QString &query, QSqlDatabase db = QSqlDatabase())，它会自动使用默认数据库，并将 strSqlStatus 作为查询语句。
	queryStatus.exec();

	QApplication::quit();
}

//鼠标移动事件
void BasicWindow::mouseMoveEvent(QMouseEvent* e)
{
	if (m_mousePressed && (e->buttons() && Qt::LeftButton))
	{
		//e->globalPosition().toPoint()事件发生时全局坐标，相对于屏幕左上角（0，0）
		move(e->globalPosition().toPoint() - m_mousePoint);						//globalPos() , 获取当前事件发生时, 鼠标全局的坐标值
			e->accept();
	}
}

//鼠标的按下事件
void BasicWindow::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == Qt::LeftButton)
	{
		//pos(),事情发生时，相对于窗口左上角的偏移坐标
		m_mousePressed = true;
		m_mousePoint = e->globalPosition().toPoint() - pos();			//减去坐标值，pos()事件发生时相对于窗口左上角（0，0）的偏移坐标
		e->accept();
	}
}

//鼠标释放事件
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

//最大化还原按钮
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