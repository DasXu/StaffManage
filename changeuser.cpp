#include "changeuser.h"
#include "ui_changeuser.h"

#include <QMessageBox>
#include <QDebug>

//changeUser::changeUser(QWidget *parent) :
changeUser::changeUser(const QString &name, const QString &sex,const QString &account,const QString &password,const QDate &birth,
                       const QString &post, const QString &clas,const QDate &year,const QString &address,
                       const QString &phone,const QString &tel,const QString &status,const QString &type,QWidget *parent) :
    QDialog(parent),
    m_account(account),
    ui(new Ui::changeUser)
{
    ui->setupUi(this);

    setWindowTitle("修改用户信息");
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

    QSqlQuery query;
    query.prepare("select name from Purview");
    QSet<QString> chatype;// 创建一个 QSet 来存储唯一的项
    // 遍历查询结果并将唯一的项添加到 ComboBox
    if(query.exec())
    {
        while (query.next())
        {
            QString item = query.value(0).toString();
            if (item != "宇宙无敌用户")
            {
               chatype.insert(item);
            }
        }
        // 将唯一的项添加到 ComboBox
        foreach (const QString &item, chatype)
        {
            ui->chatype->addItem(item);
        }
    }

    //性别选择
    ui->chasex->addItem("男");
    ui->chasex->addItem("女");

    //状态选择
    ui->chastatus->addItem("在职");
    ui->chastatus->addItem("离职");
    ui->chastatus->addItem("实习");

    //班组选择
    query.prepare("select class from UserInfo");
    QSet<QString> uniqueItems;// 创建一个 QSet 来存储唯一的项
    // 遍历查询结果并将唯一的项添加到 ComboBox
    if(query.exec())
    {
        while (query.next())
        {
            QString item = query.value(0).toString();
            uniqueItems.insert(item);
        }
        // 将唯一的项添加到 ComboBox
        foreach (const QString &item, uniqueItems)
        {
            ui->chaclass->addItem(item);
        }
    }

    //岗位
    query.prepare("select post from PostScore");
    if(query.exec())
    {
        while (query.next())
        {
            QString post = query.value(0).toString();
            ui->chapost->addItem(post);
        }
    }

    //显示内容到输入框
    ui->chaname->setText(name);
    ui->chasex->setCurrentText(sex);
    ui->chaaccount->setText(account);
    ui->chapassword->setText(password);
    ui->chabirth->setDate(birth);
    ui->chapost->setCurrentText(post);
    ui->chaclass->setCurrentText(clas);
    ui->chayear->setDate(year);
    ui->chaaddress->setText(address);
    ui->chaphone->setText(phone);
    ui->chatelephone->setText(tel);
    ui->chastatus->setCurrentText(status);
    ui->chatype->setCurrentText(type);
}

changeUser::~changeUser()
{
    delete ui;
}

void changeUser::on_chacancel_clicked()
{
    close();
}

void changeUser::on_chachange_clicked()
{
    modifyInfo();
}

void changeUser::modifyInfo()
{
    //获取输入框内容
    QString newname = ui->chaname->text();
    QString newpassword = ui->chapassword->text();
    QString newtype = ui->chatype->currentText();
    QString newsex = ui->chasex->currentText();
    QDate newbirth = ui->chabirth->date();
    QString newaddress = ui->chaaddress->text();
    QString newphone = ui->chaphone->text();
    QString newtele = ui->chatelephone->text();
    QString newstatus = ui->chastatus->currentText();
    QDate newyear = ui->chayear->date();
    QString newclass = ui->chaclass->currentText();
    QString newpost = ui->chapost->currentText();

    QString updateQuery = "UPDATE UserInfo set name = :newname, phone = :newphone, password = :newpassword, type = :newtype,"
                          " sex = :newsex, birth = :newbirth, address = :newaddress, tel = :newtele, status = :newstatus,"
                          " startY = :newyear, class = :newclass, post = :newpost WHERE account = :account";

        // 准备查询
        QSqlQuery query(db);
        query.prepare(updateQuery);
        query.bindValue(":newname", newname);
        query.bindValue(":newpassword", newpassword);
        query.bindValue(":newtype", newtype);
        query.bindValue(":newsex", newsex);
        query.bindValue(":newbirth", newbirth);
        query.bindValue(":newaddress", newaddress);
        query.bindValue(":newphone", newphone);
        query.bindValue(":newtele", newtele);
        query.bindValue(":newstatus", newstatus);
        query.bindValue(":newyear", newyear);
        query.bindValue(":newclass", newclass);
        query.bindValue(":newpost", newpost);
        query.bindValue(":account", m_account); // 原始帐户，可能需要在构造函数中传入

        // 执行查询
        if (query.exec())
        {
            QMessageBox::information(NULL,"提示","已经修改成功咯~~");
            // 在这里可以发出信号，通知主窗口或其他需要刷新数据的地方
            accept(); // 关闭编辑界面
        }
        else
        {
            QMessageBox::warning(NULL,"提示",query.lastError().text());
            qDebug() << "Error updating employee information: " << query.lastError().text();
        }
}
