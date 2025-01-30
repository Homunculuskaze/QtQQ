#include "CCMainWindow.h"
#include "NotifyManager.h"
#include "SkinWindow.h"
#include "SysTray.h"
#include "RootContactItem.h"
#include "ContactItem.h"
#include "WindowManager.h"
#include "TalkWindowShell.h"

#include <QProxyStyle>
#include <QPainter>
#include <QPixmap>
#include <QTimer>
#include <QEvent>
#include <QTreeWidgetItem>
#include <QMouseEvent>
#include <QApplication>
#include <QSqlQuery>
#include <QSqlError>
//声明一个外部变量
extern QString gLoginEmployeeID;            //在UserLogin里面定义的gLoginEmployeeID
//获取自己的头像
QString gstrLoginHeadPath;

//QString connectionString = "Driver={ODBC Driver};Server=127.0.0.1;Database=qtqq;Uid=root;Pwd=wcl62426327;";

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

CCMainWindow::CCMainWindow(QString account, bool isAccountLogin, QWidget *parent)
    : BasicWindow(parent)
    ,m_isAccountLogin(isAccountLogin)
    ,m_account(account)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() | Qt::Tool);
    loadStyleSheet("CCMainWindow");
    setHeadPixmap(getHeadPicturePath());  
    initControl();
    initTimer();
}

void CCMainWindow::initControl()
{
    //树获取焦点时不绘制边框
    ui.treeWidget->setStyle(new CustomProxyStyle);
    setLevelPixmap(0);  // 设置等级图标
    //setHeadPixmap(":/Resources/MainWindow/girl.png");   // 设置头像
    setStatusMenuIcon(":/Resources/MainWindow/StatusSucceeded.png");    // 设置状态图标

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
    appupLayout->addStretch();      // 添加弹性空间
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
    initContactTree();

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

//void CCMainWindow::initContactTree()              //原版错误代码
//{
//    //展开与收缩时的信号
//    //（QTreeWidgetItem*,int）,点击的是哪一项，哪一列
//    //点击
//    connect(ui.treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onItemClicked(QTreeWidgetItem*, int)));
//
//    //点击，展开
//    connect(ui.treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(onItemExpanded(QTreeWidgetItem*)));
//
//    //点击，收缩
//    connect(ui.treeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(onItemCollapsed(QTreeWidgetItem*)));
//
//    //双击
//    connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));
//
//    //根节点
//    //pRootGroupItem,保存分组的根节点，群组
//    QTreeWidgetItem* pRootGroupItem = new QTreeWidgetItem;
//    //不管树里有没有子项，都永远进行展示
//    pRootGroupItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
//    //设置根节点数据（第0列，使用角色，值）
//    pRootGroupItem->setData(0, Qt::UserRole, 0);            //根项数据的值为0
//    RootContactItem* pItemName = new RootContactItem(true, ui.treeWidget);
//
//    //获取公司部门ID
//    //SELECT departmentID FROM tab_department WHERE department_name='ren1'
//    QSqlQuery queryCompDepID(QString("SELECT departmentID FROM tab_department WHERE department_name='%1'").arg(QString::fromLocal8Bit("公司群")));
//    queryCompDepID.exec();              //执行
//    queryCompDepID.first();               //指向结果集的第一条
//    int ComDepID = queryCompDepID.value(0).toInt();         //转换成int
//
//    //获取QQ登录者所在的部门ID（部门群号）
//    //这里的传值，应该是传ID员工号
//    //在用户登录的时候，登录成功后，保存登录者的账号，传到这里来
//    //用一个 全局变量保存
//    //SELECT departmentID FROM tab_employee WHRER employeeID =10001
//    qDebug() << "Login Employee ID: " << gLoginEmployeeID;
//
//    QSqlQuery querySelfDepID(QString("SELECT departmentID FROM tab_employees WHERE employeeID=%1").arg(gLoginEmployeeID));
//    querySelfDepID.exec();          //执行
//    querySelfDepID.first();         //指向结果集的第一条
//    int SelfDepID = querySelfDepID.value(0).toInt();        //转换成int
//
//    //初始化公司群和登录者所在群
//    addCompanyDeps(pRootGroupItem, ComDepID);           //传入公司群ID
//    addCompanyDeps(pRootGroupItem, SelfDepID);              //传入自己所在群的ID
//
//    QString strGroupName = QString::fromLocal8Bit("武林进行曲");
//    pItemName->setText(strGroupName);
//
//    //插入分组节点
//    //添加顶级项
//    ui.treeWidget->addTopLevelItem(pRootGroupItem);
//
//    //添加项的部件,根节点是pRootGroupItem,第0列
//    //第三个参数，设置的部件
//    ui.treeWidget->setItemWidget(pRootGroupItem, 0, pItemName);
//
//}

void CCMainWindow::initContactTree() {
    // 展开与收缩时的信号
    //（QTreeWidgetItem*,int），点击的是哪一项，哪一列
    // 点击
    connect(ui.treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onItemClicked(QTreeWidgetItem*, int)));

    // 点击，展开
    connect(ui.treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(onItemExpanded(QTreeWidgetItem*)));

    // 点击，收缩
    connect(ui.treeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(onItemCollapsed(QTreeWidgetItem*)));

    // 双击
    connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));

    // 根节点
    // pRootGroupItem，保存分组的根节点，群组
    QTreeWidgetItem* pRootGroupItem = new QTreeWidgetItem;
    // 不管树里有没有子项，都永远进行展示
    pRootGroupItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    // 设置根节点数据（第0列，使用角色，值）
    pRootGroupItem->setData(0, Qt::UserRole, 0); // 根项数据的值为0
    RootContactItem* pItemName = new RootContactItem(true, ui.treeWidget);

    // 获取公司部门ID
    // SELECT departmentID FROM tab_department WHERE department_name='公司群'
    QSqlQuery queryCompDepID;
    queryCompDepID.prepare("SELECT departmentID FROM tab_department WHERE department_name = :department_name");
    queryCompDepID.bindValue(":department_name", QString::fromLocal8Bit("公司群"));
    queryCompDepID.exec();              // 执行
    queryCompDepID.first();             // 指向结果集的第一条
    int ComDepID = queryCompDepID.value(0).toInt(); // 转换成int

    // 获取QQ登录者所在的部门ID（部门群号）
    // 这里的传值，应该是传ID员工号
    // 在用户登录的时候，登录成功后，保存登录者的账号，传到这里来
    // 用一个 全局变量保存
    // SELECT departmentID FROM tab_employee WHERE employeeID = 10001
    qDebug() << "Login Employee ID: " << gLoginEmployeeID;

    QSqlQuery querySelfDepID;
    querySelfDepID.prepare("SELECT departmentID FROM tab_employees WHERE employeeID = :employeeID");
    querySelfDepID.bindValue(":employeeID", gLoginEmployeeID);
    querySelfDepID.exec();          // 执行
    querySelfDepID.first();         // 指向结果集的第一条
    int SelfDepID = querySelfDepID.value(0).toInt(); // 转换成int

    // 初始化公司群和登录者所在群
    addCompanyDeps(pRootGroupItem, ComDepID); // 传入公司群ID
    addCompanyDeps(pRootGroupItem, SelfDepID); // 传入自己所在群的ID

    QString strGroupName = QString::fromLocal8Bit("武林进行曲");
    pItemName->setText(strGroupName);

    // 插入分组节点
    // 添加顶级项
    ui.treeWidget->addTopLevelItem(pRootGroupItem);

    // 添加项的部件，根节点是pRootGroupItem，第0列
    // 第三个参数，设置的部件
    ui.treeWidget->setItemWidget(pRootGroupItem, 0, pItemName);
}


//void CCMainWindow::initContactTree()                //正确运行代码
//{
//    //展开与收缩时的信号
//    //（QTreeWidgetItem*,int）,点击的是哪一项，哪一列
//    //点击
//    connect(ui.treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onItemClicked(QTreeWidgetItem*, int)));
//
//    //点击，展开
//    connect(ui.treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(onItemExpanded(QTreeWidgetItem*)));
//
//    //点击，收缩
//    connect(ui.treeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(onItemCollapsed(QTreeWidgetItem*)));
//
//    //双击
//    connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));
//
//    //根节点
//    //pRootGroupItem,保存分组的根节点，群组
//    QTreeWidgetItem* pRootGroupItem = new QTreeWidgetItem;
//    //不管树里有没有子项，都永远进行展示
//    pRootGroupItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
//    //设置根节点数据（第0列，使用角色，值）
//    pRootGroupItem->setData(0, Qt::UserRole, 0);            //根项数据的值为0
//    RootContactItem* pItemName = new RootContactItem(true, ui.treeWidget);
//
//    int ComDepID = 0; // 初始化公司部门ID
//    int SelfDepID = 0; // 初始化登录者所在部门ID
//
//    // 获取公司部门ID
//    QSqlQuery queryCompDepID;
//    queryCompDepID.prepare("SELECT departmentID FROM tab_department WHERE department_name = :department_name");
//    queryCompDepID.bindValue(":department_name", QString::fromLocal8Bit("公司群"));
//    if (queryCompDepID.exec()) {
//        if (queryCompDepID.first()) {
//            ComDepID = queryCompDepID.value(0).toInt();  // 转换成int
//            qDebug() << "Company Department ID: " << ComDepID;
//        }
//        else {
//            qDebug() << "No department found for '公司群'.";
//        }
//    }
//    else {
//        qDebug() << "Error executing query for company department: " << queryCompDepID.lastError().text();
//    }
//
//    // 获取QQ登录者所在的部门ID（部门群号）
//    qDebug() << "Login Employee ID: " << gLoginEmployeeID;
//    QSqlQuery querySelfDepID;
//    querySelfDepID.prepare("SELECT departmentID FROM tab_employees WHERE employeeID = :employeeID");
//    querySelfDepID.bindValue(":employeeID", gLoginEmployeeID);
//    if (querySelfDepID.exec()) {
//        if (querySelfDepID.first()) {
//            SelfDepID = querySelfDepID.value(0).toInt();  // 转换成int
//            qDebug() << "Self Department ID: " << SelfDepID;
//        }
//        else {
//            qDebug() << "No department found for employee ID: " << gLoginEmployeeID;
//        }
//    }
//    else {
//        qDebug() << "Error executing query for employee department: " << querySelfDepID.lastError().text();
//    }
//
//    // 初始化公司群和登录者所在群
//    addCompanyDeps(pRootGroupItem, ComDepID);           // 传入公司群ID
//    addCompanyDeps(pRootGroupItem, SelfDepID);          // 传入自己所在群的ID
//
//    QString strGroupName = QString::fromLocal8Bit("武林进行曲");
//    pItemName->setText(strGroupName);
//
//    //插入分组节点
//    //添加顶级项
//    ui.treeWidget->addTopLevelItem(pRootGroupItem);
//
//    //添加项的部件,根节点是pRootGroupItem,第0列
//    //第三个参数，设置的部件
//    ui.treeWidget->setItemWidget(pRootGroupItem, 0, pItemName);
//}




//void CCMainWindow::addCompanyDeps(QTreeWidgetItem* pRootGroupItem, int DepID)                 //原版错误代码
//{
//    QTreeWidgetItem* pChild = new QTreeWidgetItem;
//    
//    QPixmap pix;
//    pix.load(":/Resources/MainWindow/head_mask.png");
//
//    //添加子节点
//    pChild->setData(0, Qt::UserRole, 1);            //子项数据设为1
//    pChild->setData(0, Qt::UserRole + 1, DepID);
//
//    //获取公司、部门头像
//    QPixmap groupPix;
//    QSqlQuery queryPicture(QString("SELECT picture FROM tab_department WHERE departmentID = %1").arg(DepID));
//    queryPicture.exec();
//
//    queryPicture.first();
//    groupPix.load(queryPicture.value(0).toString());
//
//
//
//    //获取部门名称
//    QString strDepName;
//    QSqlQuery querDepName(QString("SELECT department_name FROM tab_department WHERE departmentID =%1").arg(DepID));
//    querDepName.exec();
//    querDepName.first();
//    strDepName = querDepName.value(0).toString();
//
//    ContactItem* pContactItem = new ContactItem(ui.treeWidget);
//    pContactItem->setHeadPixmap(getRoundImage(groupPix,pix,pContactItem->getHeadLabelSize()));
//    pContactItem->setUserName(strDepName);
//
//    pRootGroupItem->addChild(pChild);
//    ui.treeWidget->setItemWidget(pChild, 0, pContactItem);
//
//    //m_groupMap.insert(pChild, sDeps);
//}

void CCMainWindow::addCompanyDeps(QTreeWidgetItem* pRootGroupItem, int DepID) {
    QTreeWidgetItem* pChild = new QTreeWidgetItem;

    QPixmap pix;
    pix.load(":/Resources/MainWindow/head_mask.png");

    // 添加子节点数据
    pChild->setData(0, Qt::UserRole, true);          // 子项标记为 true
    pChild->setData(0, Qt::UserRole + 1, DepID);     // 存储部门ID

    // 获取公司/部门头像
    QPixmap groupPix(":/Resources/MainWindow/default_avatar.png"); // 默认头像
    QSqlQuery queryPicture;
    queryPicture.prepare("SELECT picture FROM tab_department WHERE departmentID = :DepID");
    queryPicture.bindValue(":DepID", DepID);
    queryPicture.exec();
    queryPicture.first();
    groupPix.load(queryPicture.value(0).toString()); // 加载图片路径

    // 获取部门名称
    QString strDepName = "Unknown Department";
    QSqlQuery queryDepName;
    queryDepName.prepare("SELECT department_name FROM tab_department WHERE departmentID = :DepID");
    queryDepName.bindValue(":DepID", DepID);
    queryDepName.exec();
    queryDepName.first();
    strDepName = queryDepName.value(0).toString(); // 获取部门名称

    // 创建联系人项并设置头像和名称
    ContactItem* pContactItem = new ContactItem(ui.treeWidget);
    pContactItem->setHeadPixmap(getRoundImage(groupPix, pix, pContactItem->getHeadLabelSize()));
    pContactItem->setUserName(strDepName);

    // 添加子节点到树结构
    pRootGroupItem->addChild(pChild);
    ui.treeWidget->setItemWidget(pChild, 0, pContactItem);
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

void CCMainWindow::mousePressEvent(QMouseEvent* event)
{
    //输入框，在不需要焦点得时候，进行焦点的清除
    //点击的不是搜索部件，同事搜索部件还有焦点，这就不合理
    //因此将焦点 清除
    if (qApp->widgetAt(event->pos()) != ui.searchLineEdit && ui.searchLineEdit->hasFocus())             //跑起来的程序就是qApp
    {
        ui.searchLineEdit-> clearFocus();
    }
    else if (qApp->widgetAt(event->pos()) != ui.lineEdit && ui.lineEdit->hasFocus())
    {
        ui.lineEdit->clearFocus();
    }
     
    //其它的，进行基础的事件的处理
    BasicWindow::mousePressEvent(event);
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

//QString CCMainWindow::getHeadPicturePath()            //原版错误代码
//{
//    QString strPicturePath;
//
//    if (!m_isAccountLogin)               //QQ号登录
//    {
//        QSqlQuery queryPicture(QString("SELECT picture FROM tab_employees WHERE employeeID = %1").arg(gLoginEmployeeID));
//        queryPicture.exec();
//        queryPicture.next();
//
//        strPicturePath = queryPicture.value(0).toString();
//    }
//    else            //账号登录
//    {
//        QSqlQuery queryEmployeeID(QString("SELECT employeeID FROM tab_accounts WHERE account = '%1'").arg(m_account));
//        queryEmployeeID.exec();
//        queryEmployeeID.next();
//
//        int employeeID = queryEmployeeID.value(0).toInt();
//
//        QSqlQuery queryPicture(QString("SELECT picture FROM tab_employees WHERE employeeID = %1").arg(employeeID));
//        queryPicture.exec();
//        queryPicture.next();
//
//        strPicturePath = queryPicture.value(0).toString();
//    }
//
//    //gstrLoginHeadPath = strPicturePath;
//    return strPicturePath;
//}


QString CCMainWindow::getHeadPicturePath() {
    //QString strPicturePath = ":/Resources/MainWindow/default_avatar.png";  // 默认头像
    QString strPicturePath;

    if (!m_isAccountLogin) {  // QQ号登录
        QSqlQuery queryPicture;
        queryPicture.prepare("SELECT picture FROM tab_employees WHERE employeeID = :employeeID");
        queryPicture.bindValue(":employeeID", gLoginEmployeeID);
        queryPicture.exec();
        queryPicture.next();

        strPicturePath = queryPicture.value(0).toString();
    }
    else {  // 账号登录
        QSqlQuery queryEmployeeID;
        queryEmployeeID.prepare("SELECT employeeID FROM tab_accounts WHERE account = :account");
        queryEmployeeID.bindValue(":account", m_account);
        queryEmployeeID.exec();
        queryEmployeeID.next();

        int employeeID = queryEmployeeID.value(0).toInt();

        QSqlQuery queryPicture;
        queryPicture.prepare("SELECT picture FROM tab_employees WHERE employeeID = :employeeID");
        queryPicture.bindValue(":employeeID", employeeID);
        queryPicture.exec();
        queryPicture.next();

        strPicturePath = queryPicture.value(0).toString();
    }

    gstrLoginHeadPath = strPicturePath;

    return strPicturePath;
}




//联系人树点击事件
void CCMainWindow::onItemClicked(QTreeWidgetItem* item, int column)
{
    //判断，是否有子项
    //判断数据，第0列，角色是UserRole的返回值
    bool blsChild = item->data(0, Qt::UserRole).toBool();       //false 表示该节点是一个根节点，true 表示该节点是一个子节点

    //如果子项不为空，说明有子项
    if (!blsChild)              //当前点击的节点是根节点，则进入if语句内
    {
        //判断是否展开
        //如果没有展开，则展开子项
        item->setExpanded(!item->isExpanded());
    }
}

void CCMainWindow::onItemExpanded(QTreeWidgetItem* item)
{
    bool bIsChild = item->data(0, Qt::UserRole).toBool();
    if (!bIsChild)
    {
        //先把QWidget指针，转换成RootContactItem类型的指针
        //dynamic_cast将基类对象指针(或引用)转换到继承类指针
        RootContactItem* prootItem = dynamic_cast<RootContactItem*>(ui.treeWidget->itemWidget(item, 0));

        //判断一下，进行角度转换
        if (prootItem)
        {
            prootItem->setExpanded(true);
        }
    }
}

void CCMainWindow::onItemCollapsed(QTreeWidgetItem* item)
{
    bool bIsChild = item->data(0, Qt::UserRole).toBool();
    if (!bIsChild)
    {
        //先把QWidget指针，转换成RootContactItem类型的指针
        //dynamic_cast将基类对象指针（或引用）转换到继承类指针
        RootContactItem* prootItem = dynamic_cast<RootContactItem*>(ui.treeWidget->itemWidget(item, 0));

        //判断一下，进行角度转换
        if (prootItem)
        {
            prootItem->setExpanded(false);
        }
    }
}

//联系人树双击事件
void CCMainWindow::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
    //判断双击的事根项，还是子项
    //data（0列，角色），再转换成bool类型
    bool blsChild = item->data(0, Qt::UserRole).toBool();

    //添加的
    QString idToOpen = item->data(0, Qt::UserRole + 1).toString();
    qDebug() << "DoubleClicked. child?" << blsChild << "ID:" << idToOpen;
    WindowManager::getInstance()->addNewTalkWindow(idToOpen);


    if (blsChild)
    {
        //添加新的聊天窗口
        WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString());
        //QString strGroup = m_groupMap.value(item);
/*
        if (strGroup == QString::fromLocal8Bit("公司群"))
        {
            //添加新的聊天窗口
            WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), COMPANY);
        }
        else if (strGroup == QString::fromLocal8Bit("人事部"))
        {
            WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), PERSONELGROUP);
        }
        else if (strGroup == QString::fromLocal8Bit("市场部"))
        {
            WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), MARKETGROUP);
        }
        else if (strGroup == QString::fromLocal8Bit("研发部"))
        {
            WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(), DEVELOPMENTGROUP);
        }
*/
    }
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
