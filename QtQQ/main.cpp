//#include "CCMainWindow.h"
#include "UserLogin.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    /*CCMainWindow w;
    w.show();*/
    a.setQuitOnLastWindowClosed(false); //����Ӧ�ó��������һ�����ڹر�ʱ���˳�����ͨ�����ڶര�ڳ��������ڿ��ܱ����ض��������١�
    UserLogin* userLogin = new UserLogin;
    userLogin->show();
    return a.exec();
}
