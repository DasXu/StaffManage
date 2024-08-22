#include "ruleedit2.h"
#include "ui_ruleedit2.h"

#include <QMessageBox>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>

ruleedit2::ruleedit2(const int &id, const QString &name, const QString &explaination,const int &score,QWidget *parent) :
    QDialog(parent),
    id(id),
    ui(new Ui::ruleedit2)
{
    ui->setupUi(this);

    setWindowTitle("修改条例");
    QIcon icon(":/picture/4.jpg"); // 替换为您的Logo图片的文件路径
    setWindowIcon(icon);
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

    // 创建一个正则表达式模式，用于匹配正负号（+/-）和阿拉伯数字
    QRegExp regex("[+-]?\\d+");

    // 创建一个验证器，将正则表达式应用于 QLineEdit
    QRegExpValidator *validator = new QRegExpValidator(regex, this);

    // 将验证器应用于 QLineEdit
    ui->score->setValidator(validator);

    ui->name->setText(name);
    ui->explain->setText(explaination);
    ui->score->setText(QString::number(score));
}

ruleedit2::~ruleedit2()
{
    delete ui;
}

void ruleedit2::on_ok_clicked()
{
    QString newname = ui->name->text();
    QString newexplain = ui->explain->toPlainText();
    QString newscore = ui->score->text();

    QString updateQuery = "UPDATE ManageRule set name = :newname, explaination = :explain, score = :score WHERE manageID = :id";

        // 准备查询
        QSqlQuery query(db);
        query.prepare(updateQuery);
        query.bindValue(":newname", newname);
        query.bindValue(":explain", newexplain);
        query.bindValue(":score",newscore);
        query.bindValue(":id", id); // 原始帐户，可能需要在构造函数中传入

        // 执行查询
        if (query.exec())
        {
            QMessageBox::information(NULL,"提示","成功修改条例");
            // 在这里可以发出信号，通知主窗口或其他需要刷新数据的地方
            accept(); // 关闭编辑界面
        }
        else
        {
            QMessageBox::warning(NULL,"提示",query.lastError().text());
            qDebug() << "Error updating employee information: " << query.lastError().text();
        }
}

void ruleedit2::on_cancel_clicked()
{
    accept();
}
