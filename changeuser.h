#ifndef CHANGEUSER_H
#define CHANGEUSER_H

#include <QDialog>
#include <QMessageBox>
#include <QDebug>
#include <QSqlQuery>
#include <QtSql/QSqlDatabase>
#include <QSqlError>

namespace Ui {
class changeUser;
}

class changeUser : public QDialog
{
    Q_OBJECT

public:
//    explicit changeUser(const QString &name, const QString &sex,const QString &account,const QString &password,const QDate &birth,
//                        const QString &post, const QString &clas,const QDate &year,const QString &address,
//                        const QString &phone,const QString &tel,const QString &status,const QString &type, QWidget *parent = nullptr);
    changeUser(const QString &name, const QString &sex,const QString &account,const QString &password,const QDate &birth,
                        const QString &post, const QString &clas,const QDate &year,const QString &address,
                        const QString &phone,const QString &tel,const QString &status,const QString &type, QWidget *parent = nullptr);
    ~changeUser();

//    void getInfo(const QString &name, const QString &sex,const QString &account,const QString &password,const QDate &birth,
//                               const QString &post, const QString &clas,const QDate &year,const QString &address,
//                               const QString &phone,const QString &tel,const QString &status,const QString &type);

    void modifyInfo();

private slots:
    void on_chacancel_clicked();

    void on_chachange_clicked();

private:
    QString m_account;
    Ui::changeUser *ui;
    //加载数据库驱动
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");


};

#endif // CHANGEUSER_H
