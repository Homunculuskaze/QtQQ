/********************************************************************************
** Form generated from reading UI file 'SkinWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SKINWINDOW_H
#define UI_SKINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SkinWindowClass
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *titleWidget;
    QHBoxLayout *horizontalLayout;
    QLabel *titleIcon;
    QSpacerItem *horizontalSpacer;
    QPushButton *sysmin;
    QPushButton *sysclose;
    QWidget *bodyWidget;
    QGridLayout *gridLayout;

    void setupUi(QWidget *SkinWindowClass)
    {
        if (SkinWindowClass->objectName().isEmpty())
            SkinWindowClass->setObjectName("SkinWindowClass");
        SkinWindowClass->resize(404, 309);
        verticalLayout = new QVBoxLayout(SkinWindowClass);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        titleWidget = new QWidget(SkinWindowClass);
        titleWidget->setObjectName("titleWidget");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(titleWidget->sizePolicy().hasHeightForWidth());
        titleWidget->setSizePolicy(sizePolicy);
        titleWidget->setProperty("titleskin", QVariant(true));
        horizontalLayout = new QHBoxLayout(titleWidget);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(12, 0, 4, 0);
        titleIcon = new QLabel(titleWidget);
        titleIcon->setObjectName("titleIcon");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(titleIcon->sizePolicy().hasHeightForWidth());
        titleIcon->setSizePolicy(sizePolicy1);
        titleIcon->setMinimumSize(QSize(42, 18));
        titleIcon->setMaximumSize(QSize(42, 18));

        horizontalLayout->addWidget(titleIcon);

        horizontalSpacer = new QSpacerItem(295, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        sysmin = new QPushButton(titleWidget);
        sysmin->setObjectName("sysmin");
        sizePolicy1.setHeightForWidth(sysmin->sizePolicy().hasHeightForWidth());
        sysmin->setSizePolicy(sizePolicy1);
        sysmin->setMinimumSize(QSize(24, 24));
        sysmin->setMaximumSize(QSize(24, 24));

        horizontalLayout->addWidget(sysmin);

        sysclose = new QPushButton(titleWidget);
        sysclose->setObjectName("sysclose");
        sizePolicy1.setHeightForWidth(sysclose->sizePolicy().hasHeightForWidth());
        sysclose->setSizePolicy(sizePolicy1);
        sysclose->setMinimumSize(QSize(24, 24));
        sysclose->setMaximumSize(QSize(24, 24));

        horizontalLayout->addWidget(sysclose);


        verticalLayout->addWidget(titleWidget);

        bodyWidget = new QWidget(SkinWindowClass);
        bodyWidget->setObjectName("bodyWidget");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(bodyWidget->sizePolicy().hasHeightForWidth());
        bodyWidget->setSizePolicy(sizePolicy2);
        gridLayout = new QGridLayout(bodyWidget);
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);

        verticalLayout->addWidget(bodyWidget);


        retranslateUi(SkinWindowClass);
        QObject::connect(sysclose, &QPushButton::clicked, SkinWindowClass, qOverload<>(&QWidget::close));

        QMetaObject::connectSlotsByName(SkinWindowClass);
    } // setupUi

    void retranslateUi(QWidget *SkinWindowClass)
    {
        SkinWindowClass->setWindowTitle(QCoreApplication::translate("SkinWindowClass", "SkinWindow", nullptr));
        titleIcon->setText(QString());
        sysmin->setText(QString());
        sysclose->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class SkinWindowClass: public Ui_SkinWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SKINWINDOW_H
