//#include "CCMainWindow.h"
#include "UserLogin.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    /*CCMainWindow w;
    w.show();*/
    a.setQuitOnLastWindowClosed(false); //设置应用程序在最后一个窗口关闭时不退出。这通常用于多窗口程序，主窗口可能被隐藏而不是销毁。
    UserLogin* userLogin = new UserLogin;
    userLogin->show();
    return a.exec();
}
