#ifndef RULEEDIT2_H
#define RULEEDIT2_H

#include <QDialog>
#include <QRegExpValidator> //正则表达式，限制输入内容
#include <QSqlQueryModel>

namespace Ui {
class ruleedit2;
}

class ruleedit2 : public QDialog
{
    Q_OBJECT

public:
    explicit ruleedit2(const int &id, const QString &name, const QString &explaination,const int &score,QWidget *parent = nullptr);
    ~ruleedit2();

private slots:
    void on_ok_clicked();

    void on_cancel_clicked();

private:
    int id;
    Ui::ruleedit2 *ui;
    QSqlQueryModel model;
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL","first");

};

#endif // RULEEDIT2_H
