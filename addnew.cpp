#include "addnew.h"
#include "ui_addnew.h"

addnew::addnew(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addnew)
{
    ui->setupUi(this);

    setWindowTitle("新增用户");
    QIcon icon(":/picture/4.jpg"); // 替换为您的Logo图片的文件路径
    setWindowIcon(icon);

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

    //用户类型选择---写死
    QSqlQuery query;
    query.prepare("select name from Purview");
    QSet<QString> type;// 创建一个 QSet 来存储唯一的项
    // 遍历查询结果并将唯一的项添加到 ComboBox
    if(query.exec())
    {
        while (query.next())
        {
            QString item = query.value(0).toString();
            if (item != "宇宙无敌用户")
            {
                type.insert(item);
            }

        }
        // 将唯一的项添加到 ComboBox
        foreach (const QString &item, type)
        {
            ui->addtype->addItem(item);
        }
    }

    //性别选择
    ui->addsex->addItem("男");
    ui->addsex->addItem("女");

    //状态选择
    ui->addstatus->addItem("在职");
    ui->addstatus->addItem("离职");
    ui->addstatus->addItem("实习");

//    //班组选择--写死
//    ui->addclass->addItem("数车1组");
//    ui->addclass->addItem("数车2组");
//    ui->addclass->addItem("数车3组");
//    ui->addclass->addItem("数车4组");
//    ui->addclass->addItem("数车5组");
//    ui->addclass->addItem("数车6组");
//    ui->addclass->addItem("数车7组");
//    ui->addclass->addItem("数车8组");
//    ui->addclass->addItem("数车9组");
//    ui->addclass->addItem("数车10组");
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
            ui->addclass->addItem(item);
        }
    }

    //岗位--写死
    //岗位
    query.prepare("select post from PostScore");
    if(query.exec())
    {
        while (query.next())
        {
            QString post = query.value(0).toString();
            ui->addpost->addItem(post);
        }
    }
}

addnew::~addnew()
{
    delete ui;
}

void addnew::on_addcancel_clicked()
{
    close();
}

void addnew::on_addadd_clicked()
{
    adduser();
    QString num = ui->addaccount->text();
    QSqlQuery query;
    query.prepare("INSERT INTO Image (account) VALUES (:employeeId)");
    query.bindValue(":employeeId",num);
    query.exec();
}

void addnew::adduser()
{
    //获取输入框内容
    QString employeeName = ui->addname->text();
    QString employeeId = ui->addaccount->text();
    QString employeepassword = ui->addpassword->text();
    QString employeetype = ui->addtype->currentText();
    QString employeesex = ui->addsex->currentText();
    QDate employeebirth = ui->addbirth->date();
    QString employeeaddress = ui->addaddress->text();
    QString employeephone = ui->addphone->text();
    QString employeetele = ui->addtelephone->text();
    QString employeestatus = ui->addstatus->currentText();
    QDate employeeyear = ui->addyear->date();
    QString employeeDepartment = ui->addclass->currentText();
    QString employeepost = ui->addpost->currentText();

    //新增
    QSqlQuery query;
    query.prepare("INSERT INTO UserInfo (name, account, password, post, sex, birth, address, phone, tel, status, startY, class, type) "
                  "VALUES (:name, :employeeId, :password, :post, :sex, :birth, :address, :phone, :tel, :status, :startY, :department, :type)");

    query.bindValue(":name", employeeName);
    query.bindValue(":employeeId", employeeId);
    query.bindValue(":department", employeeDepartment);
    query.bindValue(":password", employeepassword);
    query.bindValue(":type", employeetype);
    query.bindValue(":sex", employeesex);
    query.bindValue(":birth", employeebirth);
    query.bindValue(":address", employeeaddress);
    query.bindValue(":phone", employeephone);
    query.bindValue(":tel", employeetele);
    query.bindValue(":status", employeestatus);
    query.bindValue(":startY", employeeyear);
    query.bindValue(":post", employeepost);

    if (query.exec())
    {
        // 插入成功
//       QMessageBox::information(NULL,"提示","成功了,好厉害~~");
        QMessageBox msgBox;
            msgBox.setText("成功了,好厉害~~！");
            msgBox.setInformativeText("是否继续添加新用户？");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::Yes);
            int userChoice = msgBox.exec();

            if (userChoice == QMessageBox::Yes)
            {
                // 用户选择继续添加，清空输入框等准备下一次添加
                // 清空你的输入框，下拉框等控件内容
                ui->addname->clear();
                ui->addaccount->clear();
                ui->addpassword->clear();
                ui->addtype->clear();
                ui->addsex->clear();
                ui->addbirth->clear();
                ui->addaddress->clear();
                ui->addphone->clear();
                ui->addtelephone->clear();
                ui->addstatus->clear();
                ui->addyear->clear();
                ui->addclass->clear();
                ui->addpost->clear();
            }
            else
            {
                // 用户选择不继续添加，退出新增用户界面
                this->accept(); // 或者使用你的关闭界面的方法
            }

        qDebug() << "Employee added successfully!";
    }
    else
    {
        // 插入失败
        QMessageBox::warning(NULL,"提示","你小子哪里搞错了！！");
        qDebug() << "Failed to add employee: " << query.lastError().text();
    }
}
