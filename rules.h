#ifndef RULES_H
#define RULES_H

#include <QDialog>
#include <QSqlQueryModel>
#include <QWidget>
#include <QDebug>
#include <QMessageBox>
#include <QSqlError>

#include "ruleedit.h"
#include "ruleedit2.h"

namespace Ui {
class rules;
}

class rules : public QDialog
{
    Q_OBJECT

public:
    explicit rules(QWidget *parent = nullptr);
    ~rules();

    void allshow();

private slots:
    void on_ruleAdd_clicked();

    void on_ruleDelete_clicked();

    void on_ruleEdit_clicked();

private:
    Ui::rules *ui;
    QSqlQueryModel *model = new QSqlQueryModel;
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL","first");
};

#endif // RULES_H
