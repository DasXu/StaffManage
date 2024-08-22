#ifndef RULEEDIT_H
#define RULEEDIT_H

#include <QDialog>
#include <QRegExpValidator> //正则表达式，限制输入内容
#include <QSqlQueryModel>

namespace Ui {
class ruleEdit;
}

class ruleEdit : public QDialog
{
    Q_OBJECT

public:
    explicit ruleEdit(QWidget *parent = nullptr);
    ~ruleEdit();

private slots:
    void on_ok_clicked();

    void on_cancel_clicked();

private:
    Ui::ruleEdit *ui;

    QSqlQueryModel model;
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL","first");
};

#endif // RULEEDIT_H
