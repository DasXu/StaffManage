#include "ruleedit.h"
#include "ui_ruleedit.h"

#include <QMessageBox>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>

ruleEdit::ruleEdit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ruleEdit)
{
    ui->setupUi(this);

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

    QStringList list;
    list<<"奖励"<<"处罚";
    ui->type->addItems(list);
}

ruleEdit::~ruleEdit()
{
    delete ui;
}

void ruleEdit::on_ok_clicked()
{
   QString name = ui->name->text();
   QString score = ui->score->text();
   bool ok;
   int num = score.toInt(&ok);
   if (ok)
   {
       // 转换成功，'number' 包含从文本中提取的整数
       qDebug() << "Parsed number: " << num;
   }
   else
   {
       // 转换失败，文本不是有效的整数
       QMessageBox::warning(this,"提示", "分数栏为无效文本！");
   }
   QString explain = ui->explain->toPlainText();
   int del = 0;
   QString type = ui->type->currentText();

   QSqlQuery query;
   query.prepare("INSERT INTO ManageRule (name, explaination, score, del, type) VALUES (:name, :explain, :score, :del, :type)");

   query.bindValue(":name", name);
   query.bindValue(":explain", explain);
   query.bindValue(":score", score);
   query.bindValue(":del",del);
   query.bindValue(":type",type);

   if (query.exec())
   {
       // 插入成功
//       QMessageBox::information(NULL,"提示","成功了,好厉害~~");
       QMessageBox msgBox;
       msgBox.setText("添加成功！");
       msgBox.setInformativeText("是否继续添加新的管理条例？");
       msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
       msgBox.setDefaultButton(QMessageBox::Yes);
       int userChoice = msgBox.exec();

       if (userChoice == QMessageBox::Yes)
       {
           // 用户选择继续添加，清空输入框等准备下一次添加
           // 清空你的输入框，下拉框等控件内容
           ui->name->clear();
           ui->score->clear();
           ui->explain->clear();
       }
       else
       {
           // 用户选择不继续添加，退出新增用户界面
           this->accept(); // 或者使用你的关闭界面的方法
       }
   }
   else
   {
       // 插入失败
       QMessageBox::warning(NULL,"提示","你小子哪里搞错了！！");
       qDebug() << "Failed to add employee: " << query.lastError().text();
   }

}

void ruleEdit::on_cancel_clicked()
{
    close();
}
