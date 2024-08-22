#ifndef DETIAL_H
#define DETIAL_H

#include <QDialog>

namespace Ui {
class detial;
}

class detial : public QDialog
{
    Q_OBJECT

public:
//    explicit detial(const QString &time, const QString &place,const QString &executed,const QString &type,const QDate &reason,
//                    const QString &consquence,QWidget *paren,QWidget *parent = nullptr);
    detial(const int &id, const QDate &time, const QString &place,const QString &executed,const QString &execute, const QString &type,const QString &reason, const QByteArray &prove,
           const QString &note, const int &score, const QString &status, const QString &reviewer, const QDate reviewTime,const int &i, QWidget *parent = nullptr);
    ~detial();

    void initial();
    void hideButton();

signals:
    void rejected();

public slots:
        void change();

private slots:
    void on_detcancel_clicked();

    void on_detok_clicked();


    void on_detrefuse_clicked();

private:
    Ui::detial *ui;


};

#endif // DETIAL_H
