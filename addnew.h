#ifndef ADDNEW_H
#define ADDNEW_H

#include <QDialog>
#include <QSqlQuery>
#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QMessageBox>
#include <QSqlError>

namespace Ui {
class addnew;
}

class addnew : public QDialog
{
    Q_OBJECT

public:
    explicit addnew(QWidget *parent = nullptr);
    ~addnew();

    void adduser();
private slots:
    void on_addcancel_clicked();

    void on_addadd_clicked();

private:
    Ui::addnew *ui;
};

#endif // ADDNEW_H
