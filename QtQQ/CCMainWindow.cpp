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
        //��Ա߿��ȡ����ʱ�����д���
        //���Ԫ�ص��ڱ߿���Σ���ֱ�ӷ��أ���������
        if (element == PE_FrameFocusRect)
        {
            return;
        }
        else
        {
            //������ǣ��ͽ���Ĭ�ϲ���
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
    //����ȡ����ʱ�����Ʊ߿�
    ui.treeWidget->setStyle(new CustomProxyStyle);
    setLevelPixmap(0);
    setHeadPixmap(":/Resources/MainWindow/girl.png");
    setStatusMenuIcon(":/Resources/MainWindow/StatusSucceeded.png");

    //ע��������������addWidget�ķ�����
    // ��һ���� Icons �������appupLayout��Ȼ���ٷŵ�ui.appWidget����
    // �ڶ����� Icon ����ŵ�ui.bottomLayout_up����
    //������,ͷ���Աߵ�Сappͼ��
    QHBoxLayout* appupLayout = new QHBoxLayout;
    appupLayout->setContentsMargins(0, 0, 0, 0);    //���ü�϶
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_7.png","app_7"));
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_2.png", "app_2"));
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_3.png", "app_3"));
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_4.png", "app_4"));
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_5.png", "app_5"));
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_6.png", "app_6"));
    appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/skin.png", "app_skin"));
    appupLayout->addStretch();
    appupLayout->setSpacing(2);             //����֮����2�����صĿ�λ��䣬�Եø�����
    //���ò���
    ui.appWidget->setLayout(appupLayout);

    //�����壬�����Сappͼ��
    ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_10.png","app_10"));
    ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_8.png", "app_8"));
    ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_11.png", "app_11"));
    ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_9.png", "app_9"));
    ui.bottomLayout_up->addStretch();       //�������������������գ��Ҳ��ɿ�λ���


    //��ʼ����ϵ�ˣ���
    initContacTree();

    //����ǩ��
    //����¼����������õ�ǰ�����������¼�       ��������Ҫ��;�������ء��޸�������ֹ�¼�
    ui.lineEdit->installEventFilter(this);

    //������
    ui.searchLineEdit->installEventFilter(this);

    //��С��
    connect(ui.sysmin, SIGNAL(clicked(bool)), this, SLOT(onShowHide(bool)));

    //�ر�
    connect(ui.sysclose, SIGNAL(clicked(bool)), this, SLOT(onShowClose(bool)));

    //�����������
    connect(NotifyManager::getInstance(), &NotifyManager::signalSkinChanged, [this]()
        {
            updateSearchStyle();
        });

    //�������̶���
    SysTray* systray = new SysTray(this);
}

void CCMainWindow::initTimer()
{
    QTimer* timer = new QTimer(this);
    timer->setInterval(500);            //500����

    //lambda���ʽ�������¼��źţ���������
    connect(timer, &QTimer::timeout, [this] {       //����д��[this]��Ҳ����д��[this]()��Ϊ()û�в���������ʡ����
        static int level = 0;
        if (level == 99)
        {
            level = 0;
        }
        level++;
        setLevelPixmap(level);
        });


    timer->start();             //������ʱ��
}

void CCMainWindow::setUserName(const QString& username)
{
    ui.nameLabel->adjustSize();     //�ñ�ǩ�������ݵ����Լ��ĳߴ�

    //�ı������������ʡ�Դ���...
    //�������fontMetrics()����,���ص���QFontMetrics��������������
    //�ٵ���elidedText()ʡ���ı�������������ȣ����صĿ�ȣ��ͻ���"..."���Խ��д��档ElideRightʡ�Կ���
    //��һ���������ı����ڶ���������ʡ��ģʽ
    //���������������     ���ĸ�����int flags=0���Ǳ�־
    QString name = ui.nameLabel->fontMetrics().elidedText(username, Qt::ElideRight, ui.nameLabel->width());

    ui.nameLabel->setText(name);            //�����û���
}

void CCMainWindow::setLevelPixmap(int level)
{
    //�ȼ�λͼ
    //��С�ߴ磬����ͷ�񲿼��Ĵ�С
    QPixmap levelPixmap(ui.levelBtn->size());
    levelPixmap.fill(Qt::transparent);

    //����һ������,��levelPixmap�Ͻ��л���
    QPainter painter(&levelPixmap);
    painter.drawPixmap(0, 4, QPixmap(":/Resources/MainWindow/lv.png"));

    int unitNum = level % 10;       //�ȼ���λ����1-9��
    int tenNum = level / 10;        //�ȼ���λ����10-99

    //ʮλ���ȼ�����ͼƬ����ȡͼƬ�ֵĲ��ֽ��л���
    //ֻ��������һ���֣�����Ҫ��ͼƬȫ������
    //10��4�ǻ�ͼ����ʼλ��
    //��������������ͼƬ�ļ�
    //��4��5�Ĳ�����sx   ͼƬ���Ͻǵ�λ�ã�sy  ָ���λͼ�ĸ߶�
    //��6��7�Ĳ�������sw   ͼƬ�Ŀ�ȣ�sh  ���λͼ�Ŀ��
    //˵���ˣ�����ָ��Ҫ��������ͼƬ����һ����
    //void QPainter::drawPixmap(int x, int y, const QPixmap &pixmap, int sx, int sy, int sw, int sh)
    //ͼƬ����λ60*7��һ��ʮ�����֣�ÿ������Ϊ6*7��tenNum*6,0,6,7
    painter.drawPixmap(10, 4, QPixmap(":/Resources/MainWindow/levelvalue.png"),tenNum*6,0,6,7);

    //��λ
    painter.drawPixmap(16, 4, QPixmap(":/Resources/MainWindow/levelvalue.png"), unitNum * 6, 0, 6, 7);

    //��ͷ�񲿼�����ͼ��
    ui.levelBtn->setIcon(levelPixmap);
    //���ô�С
    ui.levelBtn->setIconSize(ui.levelBtn->size());
}

void CCMainWindow::setHeadPixmap(const QString& headPath)
{
    QPixmap pix;
    pix.load(":/Resources/MainWindow/head_mask.png");
    //���ñ�ǩ��ͷ��
    ui.headLabel->setPixmap(getRoundImage(QPixmap(headPath), pix, ui.headLabel->size()));
}

void CCMainWindow::setStatusMenuIcon(const QString& statusPath)
{
    //״̬λͼ�����ô�С
    QPixmap statusBtnPixmap(ui.stausBtn->size());
    statusBtnPixmap.fill(Qt::transparent);          //͸��

    QPainter painter(&statusBtnPixmap);
    painter.drawPixmap(4, 4, QPixmap(statusPath));

    //��ͼ�괫��ȥ
    ui.stausBtn->setIcon(statusBtnPixmap);
    ui.stausBtn->setIconSize(ui.stausBtn->size());
}

QWidget* CCMainWindow::addOtherAppExtension(const QString& appPath, const QString& appName)
{
    QPushButton* btn = new QPushButton(this);           //��ǰ������Ϊ����
    btn->setFixedSize(20, 20);                  //ָ������ָ����'->'

    QPixmap pixmap(btn->size());                //������ó�Ա��'.'
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    QPixmap appPixmap(appPath);
    painter.drawPixmap( (btn->width() - appPixmap.width() ) / 2,
        (btn->height() - appPixmap.height() ) / 2, appPixmap);
    btn->setIcon(pixmap);
    btn->setIconSize(btn->size());
    btn->setObjectName(appName);        //���ö�����
    btn->setProperty("hasborder", true);     //�ò����б߿�

    //����Ƥ��
    connect(btn, &QPushButton::clicked, this, &CCMainWindow::onAppIconClicked);

    return btn;
}

void CCMainWindow::initContacTree()
{
    //չ��������ʱ���ź�
    //��QTreeWidgetItem*,int��,���������һ���һ��
    //���
    connect(ui.treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onItemClicked(QTreeWidgetItem*, int)));

    //�����չ��
    connect(ui.treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(onItemExpanded(QTreeWidgetItem*)));

    //���������
    connect(ui.treeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(onItemCollapsed(QTreeWidgetItem*)));

    //˫��
    connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));

    //���ڵ�
    //pRootGroupItem,�������ĸ��ڵ㣬Ⱥ��
    QTreeWidgetItem* pRootGroupItem = new QTreeWidgetItem;

    //����������û���������Զ����չʾ
    pRootGroupItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    ////���ø��ڵ����ݣ���0�У�ʹ�ý�ɫ��ֵ��
    pRootGroupItem->setData(0, Qt::UserRole, 0);            //�������ݵ�ֵΪ0

    ////RootContactItem* pItemName = new RootContactItem(true, ui.treeWidget);

    ////��ȡ��˾����ID
    ////SELECT departmentID FROM tab_department WHERE department_name='ren1'
    //QSqlQuery queryCompDepID(QString("SELECT departmentID FROM tab_deparment WHERE deparment_name='%1'").arg(QString::fromLocal8Bit("��˾Ⱥ")));
    //queryCompDepID.exec();              //ִ��
    //queryCompDepID.first();               //ָ�������ĵ�һ��
    //int ComDepID = queryCompDepID.value(0).toInt();         //ת����int

    ////��ȡQQ��¼�����ڵĲ���ID������Ⱥ�ţ�
    ////����Ĵ�ֵ��Ӧ���Ǵ�IDԱ����
    ////���û���¼��ʱ�򣬵�¼�ɹ��󣬱����¼�ߵ��˺ţ�����������
    ////��һ�� ȫ�ֱ�������
    ////SELECT departmentID FROM tab_employee WHRER employeeID =10001
    //QSqlQuery querySelfDepID(QString("SELECT deparmentID FROM tab_employee WHERE employeeID=%1").arg(gLoginEmployeeID));
    //querySelfDepID.exec();          //ִ��
    //querySelfDepID.first();         //ָ�������ĵ�һ��
    //int SelfDepID = querySelfDepID.value(0).toInt();        //ת����int

    ////��ʼ����˾Ⱥ�͵�¼������Ⱥ
    //addCompanyDeps(pRootGroupItem, ComDepID);           //���빫˾ȺID
    //addCompanyDeps(pRootGroupItem, SelfDepID);              //�����Լ�����Ⱥ��ID

    //QString strGroupName = QString::fromLocal8Bit("���ֽ�����");
    //pItemName->setText(strGroupName);

    ////�������ڵ�
    ////��Ӷ�����
    //ui.treeWidget->addTopLevelItem(pRootGroupItem);

    ////�����Ĳ���,���ڵ���pRootGroupItem,��0��
    ////���������������õĲ���
    //ui.treeWidget->setItemWidget(pRootGroupItem, 0, pItemName);
}

void CCMainWindow::resizeEvent(QResizeEvent* event)
{
    //�ӱ��ر��룬ת���Unicode
    setUserName(QString::fromLocal8Bit("���տƼ����Բ���"));
    BasicWindow::resizeEvent(event);
}

bool CCMainWindow::eventFilter(QObject* obj, QEvent* event)
{
    //�жϼ����¼��Ķ����ǲ�����������
    if (ui.searchLineEdit == obj)
    {
        //�ж��¼�������
        //event����type�������������ص�ǰ�¼�������
        //�Ƿ�Ϊ���� ��ȡ�����¼�
        if (event->type() == QEvent::FocusIn)
        {
            //��������������������ʽ
            //����Ϊ��ɫrgb(255,255,255),��ɫΪrgb(0,0,0),rgba(%1,%2,%3,100)���ĸ���������͸���ȣ�ȡֵ��Χ�Ǵ� 0 �� 255,100��ʾһ�ְ�͸��״̬����Լ�� 39% �Ĳ�͸���ȣ��� 61% ��͸���ȣ�
           //hover���������pressed����Ч��
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
            //����ƿ������㲻��������ʱ��Ҫ��ԭ
            updateSearchStyle();
        }
    }

    return false;
}

void CCMainWindow::updateSearchStyle()
{
    //���л�ԭ
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
    //����źŷ����ߵĶ���������app_skin
    if (sender()->objectName() == "app_skin")
    {
        SkinWindow* skinWindow = new SkinWindow;
        skinWindow->show();
    }
}

CCMainWindow::~CCMainWindow()
{}
