#include "detial.h"
#include "ui_detial.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QSqlError>

//detial::detial(QWidget *parent) :
detial::detial(const int &id, const QDate &time, const QString &place,const QString &executed,const QString &execute,const QString &type,const QString &reason,const QByteArray &prove,
               const QString &note, const int &score, const QString &status, const QString &reviewer, const QDate reviewTime, const int &i, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::detial)
{
    ui->setupUi(this);

    setWindowTitle("查看详情");
    QIcon icon(":/picture/4.jpg"); // 替换为您的Logo图片的文件路径
    setWindowIcon(icon);

    //显示内容到输入框
    ui->detID->setText(QString::number(id));
    ui->dateEdit->setDate(time);
    ui->detwhere->setText(place);
    ui->detwho->setText(executed);
    ui->dettype->setText(type);
    ui->detreason->addItem(reason);
    ui->detnote->setText(note);
    ui->detStatus->setText(status);
    ui->detReviewer->setText(reviewer);
    ui->detTime->setDate(reviewTime);
    int a = i;
    if(a == 1 )
    {
        ui->reporter->setText(execute);
    }
    else
    {
        ui->reporter->hide();
        ui->label_13->hide();
    }

    QImage image;
    image.loadFromData(prove);
    ui->detprove->setPixmap(QPixmap::fromImage(image));
    ui->detprove->setScaledContents(true);//让图片适应label
    ui->detprove->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);


    //禁止编辑
    ui->detID->setReadOnly(true);
    ui->dateEdit->setReadOnly(true);
    ui->detwhere->setReadOnly(true);
    ui->detwho->setReadOnly(true);
    ui->dettype->setReadOnly(true);
    ui->detnote->setReadOnly(true);
    ui->detStatus->setReadOnly(true);
    ui->detReviewer->setReadOnly(true);
    ui->detTime->setReadOnly(true);

    QString text = ui->detStatus->text();
    if(text != NULL)
    {
        ui->detreason->setDisabled(true); // 禁用 QComboBox
        ui->detend->setText(QString::number(score));
        ui->detend->setReadOnly(true);
    }
    else
    {
        QString selectedReason = ui->detreason->currentText();
        QSqlQuery query;
        query.prepare("SELECT score FROM ManageRule WHERE name = :reason");
        query.bindValue(":reason", selectedReason);

        if (query.exec() && query.next())
        {
            int score = query.value(0).toInt();
            ui->detend->setText(QString::number(score));
        }
        else
        {
            // 如果未找到分值，您可以设置一个默认值或清空detend
            ui->detend->clear();
        }
    }
    initial();
    connect(ui->detreason, SIGNAL(currentIndexChanged(int)), this, SLOT(change()));
}

detial::~detial()
{
    delete ui;
}

void detial::on_detcancel_clicked()
{
    close();
}

void detial::on_detok_clicked()
{
    QString text = ui->detStatus->text();
    int id = ui->detID->text().toInt();
    if(text != NULL)
    {
        close();
    }
    else
    {
        QString reason = ui->detreason->currentText();

        // 在此处编写代码以从数据库中检索分值，假设使用了QSqlQuery
        QSqlQuery query;
        query.prepare("SELECT score,manageID FROM ManageRule WHERE name = :reason");
        query.bindValue(":reason", reason);

        if (query.exec() && query.next())
        {
            int score = query.value(0).toInt();
            int manageid = query.value(1).toInt();

            QSqlQuery insertQuery;
            insertQuery.prepare("UPDATE punishInfo set reason = :reason, manageID = :manageid, score = :score WHERE id = :id");
            insertQuery.bindValue(":reason", reason);
            insertQuery.bindValue(":manageid", manageid);
            insertQuery.bindValue(":score", score);
            insertQuery.bindValue(":id", id);
            if(insertQuery.exec())
            {
                accept();
            }
            else
            {
                QMessageBox::warning(NULL,"提示",insertQuery.lastError().text());
            }
        }
        else
        {
            // 如果未找到分值，您可以设置一个默认值或清空detend
            ui->detend->clear();
        }
        accept();
    }
}

void detial::on_detrefuse_clicked()
{
    QString text = ui->detStatus->text();
    if(text != NULL)
    {
        close();
    }
    else
    {
        emit rejected();
        close();
    }
}

void detial::initial()
{
    QString textInfo = ui->dettype->text();
    if(textInfo == "奖励申请")
    {
        QStringList string;
        QSqlQuery query("SELECT name FROM ManageRule where type = '奖励' ");
        while (query.next())
        {
            QString data1 = query.value(0).toString();
            string.append(data1);
        }
        ui->detreason->addItems(string);

    }
    else if (textInfo == "检举")
    {
        QStringList string;
        QSqlQuery query("SELECT name FROM ManageRule where type = '处罚' ");
        while (query.next())
        {
            QString data1 = query.value(0).toString();
            string.append(data1);
        }
        ui->detreason->addItems(string);
    }
}

void detial::change()
{
    QString selectedReason = ui->detreason->currentText();

    // 在此处编写代码以从数据库中检索分值，假设使用了QSqlQuery
    QSqlQuery query;
    query.prepare("SELECT score FROM ManageRule WHERE name = :reason");
    query.bindValue(":reason", selectedReason);

    if (query.exec() && query.next())
    {
        int score = query.value(0).toInt();
        ui->detend->setText(QString::number(score));
    }
    else
    {
        // 如果未找到分值，您可以设置一个默认值或清空detend
        ui->detend->clear();
    }
}

void detial::hideButton()
{
    ui->detok->hide();
    ui->detcancel->hide();
    ui->detrefuse->hide();
}
