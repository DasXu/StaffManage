#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>


namespace Ui {
class login;
}

class login : public QDialog
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();
    void database();
    void reset();

signals:
    void userLogIn(const QString &account);

private slots:
    void on_ok_clicked();

    void on_cancel_clicked();

private:
    Ui::login *ui;
};

#endif // LOGIN_H
