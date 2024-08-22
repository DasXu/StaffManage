#include "mainwindow.h"
#include <QApplication>
#include "login.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    login w;
    MainWindow m;
    QObject::connect(&w, &login::userLogIn, &m, &MainWindow::getlogin);
//    QObject::connect(&w, &login::userLogIn, [&m]() {
//            // 打开主窗口
//            m.show();
//        });
    if(w.exec() == QDialog::Accepted)
    {
        m.show();

        return a.exec();
    }
    else
    {
            // 用户取消登录或登录失败，关闭应用程序
            return -1;
    }

}
