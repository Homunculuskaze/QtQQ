#include "CCMainWindow.h"
#include "NotifyManager.h"
#include "SkinWindow.h"
#include "SysTray.h"
#include <QProxyStyle>
#include <QPainter>
#include <QPixmap>
#include <QTimer>
#include <QEvent>
#include <QTreeWidgetItem>
class CustomProxyStyle :public QProxyStyle
{
public:
    virtual void drawPrimitive(PrimitiveElement element, const QStyleOption* option,
        QPainter* painter, const QWidget* widget = nullptr)const
    {
        //针对边框获取焦点时，进行处理
        //如果元素等于边框矩形，就直接返回，不绘制它
        if (element == PE_FrameFocusRect)
        {
            return;
        }
        else
        {
            //如果不是，就进行默认操作
            QProxyStyle::drawPrimitive(element, option, painter, widget);
        }
    }
};

CCMainWindow::CCMainWindow(QWidget *parent)
    : BasicWindow(parent)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() | Qt::Tool);
    loadStyleSheet("CCMainWindow");
    initControl();
    initTimer();
}

void CCMainWindow::initControl()
{
    //树获取焦点时不绘制边框
    ui.treeWidget->setStyle(new CustomProxyStyle);
    setLevelPixmap(0);
    setHeadPixmap(":/Resources/MainWindow/girl.png");
    setStatusMenuIcon(":/Resources/MainWindow/StatusSucceeded.png");

    //注意下面两种设置addWidget的方法，
    // 第一种是 Icons 都打包到appupLayout，然后再放到ui.appWidget里面
    // 第二种是 Icon 逐个放到ui.bottomLayout_up里面
    //主窗体,头像旁边的小app图标
    QHBoxLayout* appupLayout = new QHBoxLayout;
    appupLayout->setContentsMargins(0, 0, 0, 0);    //设置间隙
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_7.png","app_7"));
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_2.png", "app_2"));
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_3.png", "app_3"));
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_4.png", "app_4"));
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_5.png", "app_5"));
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_6.png", "app_6"));
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/skin.png", "app_skin"));
    appupLayout->addStretch();
    appupLayout->setSpacing(2);             //部件之间以2个像素的空位填充，显得更紧凑
    //设置布局
    ui.appWidget->setLayout(appupLayout);

    //主窗体，下面的小app图标
    ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_10.png","app_10"));
    ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_8.png", "app_8"));
    ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_11.png", "app_11"));
    ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_9.png", "app_9"));
    ui.bottomLayout_up->addStretch();       //加伸缩量，部件更紧凑，右侧变成空位填充


    //初始化联系人，树
    initContacTree();

    //个性签名
    //添加事件过滤器，让当前对象监视这个事件       过滤器主要用途包括拦截、修改甚至阻止事件
    ui.lineEdit->installEventFilter(this);

    //搜索栏
    ui.searchLineEdit->installEventFilter(this);

    //最小化
    connect(ui.sysmin, SIGNAL(clicked(bool)), this, SLOT(onShowHide(bool)));

    //关闭
    connect(ui.sysclose, SIGNAL(clicked(bool)), this, SLOT(onShowClose(bool)));

    //更新搜索风格
    connect(NotifyManager::getInstance(), &NotifyManager::signalSkinChanged, [this]()
        {
            updateSearchStyle();
        });

    //创建托盘对象
    SysTray* systray = new SysTray(this);
}

void CCMainWindow::initTimer()
{
    QTimer* timer = new QTimer(this);
    timer->setInterval(500);            //500毫秒

    //lambda表达式，发射事件信号，进行升级
    connect(timer, &QTimer::timeout, [this] {       //可以写成[this]，也可以写成[this]()因为()没有参数，这里省略了
        static int level = 0;
        if (level == 99)
        {
            level = 0;
        }
        level++;
        setLevelPixmap(level);
        });


    timer->start();             //启动计时器
}

void CCMainWindow::setUserName(const QString& username)
{
    ui.nameLabel->adjustSize();     //让标签根据内容调整自己的尺寸

    //文本过长，则进行省略处理...
    //对象调用fontMetrics()方法,返回的是QFontMetrics字体体积的类对象
    //再调用elidedText()省略文本方法，超过宽度（像素的宽度）就会以"..."缩略进行代替。ElideRight省略靠右
    //第一个参数，文本。第二个参数，省略模式
    //第三个参数：宽度     第四个参数int flags=0，是标志
    QString name = ui.nameLabel->fontMetrics().elidedText(username, Qt::ElideRight, ui.nameLabel->width());

    ui.nameLabel->setText(name);            //设置用户名
}

void CCMainWindow::setLevelPixmap(int level)
{
    //等级位图
    //大小尺寸，调用头像部件的大小
    QPixmap levelPixmap(ui.levelBtn->size());
    levelPixmap.fill(Qt::transparent);

    //设置一个画家,在levelPixmap上进行绘制
    QPainter painter(&levelPixmap);
    painter.drawPixmap(0, 4, QPixmap(":/Resources/MainWindow/lv.png"));

    int unitNum = level % 10;       //等级个位数，1-9级
    int tenNum = level / 10;        //等级两位数，10-99

    //十位，等级数字图片，截取图片种的部分进行绘制
    //只绘制其中一部分，不需要把图片全部绘制
    //10，4是绘图的起始位置
    //第三个参数，是图片文件
    //第4，5的参数是sx   图片左上角的位置，sy  指这个位图的高度
    //第6，7的参数，是sw   图片的宽度，sh  这个位图的宽度
    //说白了，就是指定要拷贝整个图片的哪一部分
    //void QPainter::drawPixmap(int x, int y, const QPixmap &pixmap, int sx, int sy, int sw, int sh)
    //图片像素位60*7，一共十个数字，每个数字为6*7即tenNum*6,0,6,7
    painter.drawPixmap(10, 4, QPixmap(":/Resources/MainWindow/levelvalue.png"),tenNum*6,0,6,7);

    //个位
    painter.drawPixmap(16, 4, QPixmap(":/Resources/MainWindow/levelvalue.png"), unitNum * 6, 0, 6, 7);

    //给头像部件设置图标
    ui.levelBtn->setIcon(levelPixmap);
    //设置大小
    ui.levelBtn->setIconSize(ui.levelBtn->size());
}

void CCMainWindow::setHeadPixmap(const QString& headPath)
{
    QPixmap pix;
    pix.load(":/Resources/MainWindow/head_mask.png");
    //设置标签的头像
    ui.headLabel->setPixmap(getRoundImage(QPixmap(headPath), pix, ui.headLabel->size()));
}

void CCMainWindow::setStatusMenuIcon(const QString& statusPath)
{
    //状态位图，设置大小
    QPixmap statusBtnPixmap(ui.stausBtn->size());
    statusBtnPixmap.fill(Qt::transparent);          //透明

    QPainter painter(&statusBtnPixmap);
    painter.drawPixmap(4, 4, QPixmap(statusPath));

    //将图标传进去
    ui.stausBtn->setIcon(statusBtnPixmap);
    ui.stausBtn->setIconSize(ui.stausBtn->size());
}

QWidget* CCMainWindow::addOtherAppExtension(const QString& appPath, const QString& appName)
{
    QPushButton* btn = new QPushButton(this);           //当前对象作为父类
    btn->setFixedSize(20, 20);                  //指向对象的指针用'->'

    QPixmap pixmap(btn->size());                //对象调用成员用'.'
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    QPixmap appPixmap(appPath);
    painter.drawPixmap( (btn->width() - appPixmap.width() ) / 2,
        (btn->height() - appPixmap.height() ) / 2, appPixmap);
    btn->setIcon(pixmap);
    btn->setIconSize(btn->size());
    btn->setObjectName(appName);        //设置对象名
    btn->setProperty("hasborder", true);     //让部件有边框

    //更换皮肤
    connect(btn, &QPushButton::clicked, this, &CCMainWindow::onAppIconClicked);

    return btn;
}

void CCMainWindow::initContacTree()
{
    //展开与收缩时的信号
    //（QTreeWidgetItem*,int）,点击的是哪一项，哪一列
    //点击
    connect(ui.treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onItemClicked(QTreeWidgetItem*, int)));

    //点击，展开
    connect(ui.treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(onItemExpanded(QTreeWidgetItem*)));

    //点击，收缩
    connect(ui.treeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(onItemCollapsed(QTreeWidgetItem*)));

    //双击
    connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));

    //根节点
    //pRootGroupItem,保存分组的根节点，群组
    QTreeWidgetItem* pRootGroupItem = new QTreeWidgetItem;

    //不管树里有没有子项，都永远进行展示
    pRootGroupItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    ////设置根节点数据（第0列，使用角色，值）
    pRootGroupItem->setData(0, Qt::UserRole, 0);            //根项数据的值为0

    ////RootContactItem* pItemName = new RootContactItem(true, ui.treeWidget);

    ////获取公司部门ID
    ////SELECT departmentID FROM tab_department WHERE department_name='ren1'
    //QSqlQuery queryCompDepID(QString("SELECT departmentID FROM tab_deparment WHERE deparment_name='%1'").arg(QString::fromLocal8Bit("公司群")));
    //queryCompDepID.exec();              //执行
    //queryCompDepID.first();               //指向结果集的第一条
    //int ComDepID = queryCompDepID.value(0).toInt();         //转换成int

    ////获取QQ登录者所在的部门ID（部门群号）
    ////这里的传值，应该是传ID员工号
    ////在用户登录的时候，登录成功后，保存登录者的账号，传到这里来
    ////用一个 全局变量保存
    ////SELECT departmentID FROM tab_employee WHRER employeeID =10001
    //QSqlQuery querySelfDepID(QString("SELECT deparmentID FROM tab_employee WHERE employeeID=%1").arg(gLoginEmployeeID));
    //querySelfDepID.exec();          //执行
    //querySelfDepID.first();         //指向结果集的第一条
    //int SelfDepID = querySelfDepID.value(0).toInt();        //转换成int

    ////初始化公司群和登录者所在群
    //addCompanyDeps(pRootGroupItem, ComDepID);           //传入公司群ID
    //addCompanyDeps(pRootGroupItem, SelfDepID);              //传入自己所在群的ID

    //QString strGroupName = QString::fromLocal8Bit("武林进行曲");
    //pItemName->setText(strGroupName);

    ////插入分组节点
    ////添加顶级项
    //ui.treeWidget->addTopLevelItem(pRootGroupItem);

    ////添加项的部件,根节点是pRootGroupItem,第0列
    ////第三个参数，设置的部件
    //ui.treeWidget->setItemWidget(pRootGroupItem, 0, pItemName);
}

void CCMainWindow::resizeEvent(QResizeEvent* event)
{
    //从本地编码，转变成Unicode
    setUserName(QString::fromLocal8Bit("三艺科技测试测试"));
    BasicWindow::resizeEvent(event);
}

bool CCMainWindow::eventFilter(QObject* obj, QEvent* event)
{
    //判断监视事件的对象是不是搜索部件
    if (ui.searchLineEdit == obj)
    {
        //判断事件的类型
        //event调用type（）方法，返回当前事件的类型
        //是否为键盘 获取焦点事件
        if (event->type() == QEvent::FocusIn)
        {
            //给搜索栏部件，设置样式
            //背景为白色rgb(255,255,255),黑色为rgb(0,0,0),rgba(%1,%2,%3,100)第四个参数控制透明度，取值范围是从 0 到 255,100表示一种半透明状态，大约是 39% 的不透明度（即 61% 的透明度）
           //hover鼠标悬浮，pressed按下效果
            ui.searchWidget->setStyleSheet(QString("QWidget#searchWidget{background-color:rgb(255,255,255);border-bottom:1px solid rgba(%1,%2,%3,100)}\
                                                                                QPushButton#searchBtn{border-image:url(:/Resources/MainWindow/search/main_search_deldown.png)}\
                                                                                QPushButton#searchBtn:hover{border-image:url(:/Resources/MainWindow/search/main_search_delhighlight.png)}\
                                                                                QPushButton#searchBtn:preesed{border-image:url(:/Resources/MainWindow/search/main_search_delhighdown.png)}")
                                                                                .arg(m_colorBackGround.red())
                                                                                .arg(m_colorBackGround.green())
                                                                                .arg(m_colorBackGround.blue()));
        }
        else if (event->type() == QEvent::FocusOut)
        {
            //鼠标移开，焦点不在搜索框时，要还原
            updateSearchStyle();
        }
    }

    return false;
}

void CCMainWindow::updateSearchStyle()
{
    //进行还原
    ui.searchWidget->setStyleSheet(QString("QWidget#searchWidget{background-color:rgba(%1,%2,%3,50);border-bottom:1px solid rgba(%1,%2,%3,30)}\
                                                                        QPushButton#searchBtn{border-image:url(:/Resources/MainWindow/search/search_icon.png)}")
                                                                        .arg(m_colorBackGround.red())
                                                                        .arg(m_colorBackGround.green())
                                                                        .arg(m_colorBackGround.blue()));
}

void CCMainWindow::onItemClicked(QTreeWidgetItem* item, int column)
{
}

void CCMainWindow::onItemExpamded(QTreeWidgetItem* item)
{
}

void CCMainWindow::onItemCollapsed(QTreeWidgetItem* item)
{
}

void CCMainWindow::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
}

void CCMainWindow::onAppIconClicked()
{
    //如果信号发送者的对象名，是app_skin
    if (sender()->objectName() == "app_skin")
    {
        SkinWindow* skinWindow = new SkinWindow;
        skinWindow->show();
    }
}

CCMainWindow::~CCMainWindow()
{}
