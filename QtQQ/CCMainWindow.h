#pragma once

//#include <QtWidgets/QWidget>
#include "basicwindow.h"
#include "ui_CCMainWindow.h"

class QTreeWidgetItem;

class CCMainWindow : public BasicWindow
{
    Q_OBJECT

public:
    CCMainWindow(QString account,bool isAccountLogin,QWidget *parent = nullptr);
    ~CCMainWindow();

public:
    void initControl();
    void initTimer();
    void setUserName(const QString& username);      //设置用户名
    void setLevelPixmap(int level);                                 //设置等级图片
    void setHeadPixmap(const QString& headPath);    //设置头像路径
    void setStatusMenuIcon(const QString& statusPath);   //设置状态

    //添加应用部件（app图片路径，app部件对象名)
    QWidget* addOtherAppExtension(const QString& appPath, const QString& appName);

    //初始化联系人
    void initContactTree();

    //传入部门ID
    //void addCompanyDeps(QTreeWidgetItem* pRootGroupItem, int DepID);
    void addCompanyDeps(QTreeWidgetItem* pRootGroupItem, int DepID);

private:
    //当窗口改变的时候，用户名前后要保持一致
    //通过这个事件实现
    void resizeEvent(QResizeEvent* event);

    //重写事件过滤器
    bool eventFilter(QObject* obj, QEvent* event);

    //重写鼠标按下事件
    void mousePressEvent(QMouseEvent* event);

    //更新搜索框样式
    void updateSearchStyle();
    QString getHeadPicturePath();
private slots:
    void onAppIconClicked();            //更换皮肤

    void onItemClicked(QTreeWidgetItem* item, int column);      //单击，点击column列
    void onItemExpanded(QTreeWidgetItem* item);                    //展开
    void onItemCollapsed(QTreeWidgetItem* item);                     //收缩
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);        //双击

private:
    Ui::CCMainWindowClass ui;
    bool m_isAccountLogin;
    QString m_account;      //登录账号或qq号
    //QMap<QTreeWidgetItem*, QString> m_groupMap;     //所有分组的项
};
