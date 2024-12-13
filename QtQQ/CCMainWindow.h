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
    void setUserName(const QString& username);      //�����û���
    void setLevelPixmap(int level);                                 //���õȼ�ͼƬ
    void setHeadPixmap(const QString& headPath);    //����ͷ��·��
    void setStatusMenuIcon(const QString& statusPath);   //����״̬

    //���Ӧ�ò�����appͼƬ·����app����������)
    QWidget* addOtherAppExtension(const QString& appPath, const QString& appName);

    //��ʼ����ϵ��
    void initContactTree();

    //���벿��ID
    //void addCompanyDeps(QTreeWidgetItem* pRootGroupItem, int DepID);
    void addCompanyDeps(QTreeWidgetItem* pRootGroupItem, int DepID);

private:
    //�����ڸı��ʱ���û���ǰ��Ҫ����һ��
    //ͨ������¼�ʵ��
    void resizeEvent(QResizeEvent* event);

    //��д�¼�������
    bool eventFilter(QObject* obj, QEvent* event);

    //��д��갴���¼�
    void mousePressEvent(QMouseEvent* event);

    //������������ʽ
    void updateSearchStyle();
    QString getHeadPicturePath();
private slots:
    void onAppIconClicked();            //����Ƥ��

    void onItemClicked(QTreeWidgetItem* item, int column);      //���������column��
    void onItemExpanded(QTreeWidgetItem* item);                    //չ��
    void onItemCollapsed(QTreeWidgetItem* item);                     //����
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);        //˫��

private:
    Ui::CCMainWindowClass ui;
    bool m_isAccountLogin;
    QString m_account;      //��¼�˺Ż�qq��
    //QMap<QTreeWidgetItem*, QString> m_groupMap;     //���з������
};
