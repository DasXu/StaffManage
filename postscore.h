#ifndef POSTSCORE_H
#define POSTSCORE_H

#include <QDialog>
#include <QSqlQueryModel>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlTableModel>
#include <QKeyEvent>


namespace Ui {
class postScore;
}

class postScore : public QDialog
{
    Q_OBJECT

public:
    explicit postScore(QWidget *parent = nullptr);
    ~postScore();

    void initial();
public slots:
    void handleEnterKey();
private slots:
    void on_addPost_clicked();

    void on_deletePost_clicked();

private:
    Ui::postScore *ui;
    QSqlTableModel *model = new QSqlTableModel(this);;
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL","first");
};

#endif // POSTSCORE_H
