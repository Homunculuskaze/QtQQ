#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>

enum ButtonType
{
	MIN_BUTTON = 0,		//最小化+关闭，按钮
	MIN_MAX_BUTTON,		//最小化+最大化+关闭，按钮
	ONLY_CLOSE_BUTTON		//只有关闭按钮
};

//自定义标题栏
class TitleBar : public QWidget
{
	Q_OBJECT

public:
	TitleBar(QWidget* parent = nullptr);
	~TitleBar();

	void setTitleIcon(const QString& filePath);						//设置标题栏图标
	void setTitleContent(const QString& titleContent);		//设置标题栏内容
	void setTitleWidth(int width);											//设置标题栏宽度
	void setButtonType(ButtonType buttonType);				//设置标题栏按钮类型

	void saveRestoreInfo(const QPoint& point, const QSize& size);	//保存窗口最大化前，窗口的位置+大小
	void getRestoreInfo(QPoint& point, QSize& size);			//获取保存的窗口最大化前，窗口的位置+大小

private:
	void paintEvent(QPaintEvent* event);									//绘图事件
	void mouseDoubleClickEvent(QMouseEvent* event);		//鼠标双击事件
	void mousePressEvent(QMouseEvent* event);					//鼠标按下事件
	void mouseMoveEvent(QMouseEvent* event);					//鼠标移动事件
	void mouseReleaseEvent(QMouseEvent* event);				//鼠标松开事件，就是鼠标释放事件

	void initControl();				//初始化控件
	void initConnections();		//初始化信号与槽的连接
	void loadStyleSheet(const QString& sheetName);			//加载样式表

	//信号
signals:
	//点击标题栏按钮时，发射的信号
	void signalButtonMinClicked();					//最小化按钮
	void signalButtonRestoreClicked();				//最大化还原按钮
	void signalButtonMaxClicked();					//最大化按钮
	void signalButtonCloseClicked();					//关闭按钮

private slots:
	//按钮响应的槽
	void onButtonMinClicked();
	void onButtonRestoreClicked();
	void onButtonMaxClicked();
	void onButtonCloseClicked();

private:
	QLabel* m_pIcon;			//标题栏图标
	QLabel* m_pTitleContent;		//标题栏内容
	QPushButton* m_pButtonMin;		//最小化按钮
	QPushButton* m_pButtonRestore;		//最大化还原按钮
	QPushButton* m_pButtonMax;			//最大化按钮
	QPushButton* m_pButtonClose;			//关闭按钮

	//最大化还原按钮变量（用于保存窗口位置+大小）
	QPoint m_restorePos;
	QSize m_restoreSize;

	//移动窗口的变量
	bool m_isPressed;		//是否被按下
	QPoint m_startMovePos;			//开始移动的点

	QString m_titleContent;			//标题栏的内容
	ButtonType m_buttonType;		//标题栏右上角,按钮类型
};
