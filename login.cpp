#include "login.h"
#include "ui_login.h"
#include "mainwindow.h"

#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlQueryModel>

#include <QtSql/QSqlDatabase>
#include <QSqlError>
#include <QTextEdit>
#include <QDebug>

login::login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);

    setWindowTitle("登录界面");
    QIcon icon(":/picture/0.jpg"); // 替换为您的Logo图片的文件路径
    setWindowIcon(icon);

    QImage image;
    image.load(":/picture/01.png");
    ui->Label->setPixmap(QPixmap::fromImage(image));
//  ui->Label->resize(QSize(image.width(),image.height()));
    ui->Label->setScaledContents(true);//让图片适应label
//  ui->label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    database();
}

login::~login()
{
    delete ui;
}


void login::on_ok_clicked()
{
    //写死的账号密码
//    if(ui->Username->text() == "NJIT" && ui->Password->text() == "123456")
//    {
//        this->close();
//        MainWindow *h=new MainWindow;
//        h->show();
//    }
//    else
//    {
//        QMessageBox::warning(this,tr("登录失败"),tr("用户名或密码输入错误！"),QMessageBox::Ok);
//        ui->Username->clear();
//        ui->Password->clear();
//        ui->Username->setFocus();
//    }

    QString username = ui->Username->text();
    QString password = ui->Password->text();

    // 执行数据库查询以验证用户
    QSqlQuery query;
    query.prepare("SELECT * FROM UserInfo WHERE account = :username AND password = (select password from UserInfo where account = :username)");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (query.exec() && query.next())
    {
        // 用户验证成功，发射信号并传递用户账号
        QString userAccount = query.value("account").toString();
        QString passwordnow = query.value("password").toString();
        if(password==passwordnow)
        {
            QMessageBox::information(NULL, "登陆成功", "登陆成功！！！", QMessageBox::Yes);
            emit userLogIn(userAccount);
            accept(); // 关闭登录窗口
        }
        else
        {
            // 用户验证失败，显示错误消息
            QMessageBox::warning(NULL,"错误","用户名或密码错误");
        }
    }
    else
    {
        // 用户验证失败，显示错误消息
        QMessageBox::warning(NULL,"错误","为查询到相关信息");
    }
}

void login::on_cancel_clicked()
{
    close();
}

void login::database()
{
    //加载数据库驱动
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");

    //连接数据库
    db.setDatabaseName("StaffTest");
    db.setHostName("localhost");
    db.setPort(3306);
    db.setUserName("root");
    db.setPassword("123456");

    if(!db.open())
    {
        QMessageBox::warning(this,"连接提示",db.lastError().text());
    }
}

void login::reset()
{
    ui->Username->clear();
    ui->Password->clear();
}
