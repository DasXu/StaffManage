#ifndef PRUTYPE_H
#define PRUTYPE_H

#include <QCheckBox>
#include <QDialog>
#include <QSqlQueryModel>
#include <QStandardItemModel>

namespace Ui {
class prutype;
}

class prutype : public QDialog
{
    Q_OBJECT

public:
    explicit prutype(QWidget *parent = nullptr);
    ~prutype();

    void showInfo();

    QMap<QString, int> getUserPermissionsFromDatabase(const QString &userType);
public slots:
    void selectAll(int state);
    void onTableViewClicked(const QModelIndex &index);
private slots:
    void on_ptcancel_clicked();

    void on_addtype_clicked();

    void on_deltype_clicked();

    void on_ptsave_clicked();

    void handleDataChanged(const QModelIndex &index);
private:
    Ui::prutype *ui;
//    QSqlQueryModel model;
    QStandardItemModel *model = new QStandardItemModel(this);
//    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL","first");

    QList<QCheckBox*> checkBoxList; //获取checkbox返回值

    QMap<QString, int> userPermissions;
};

#endif // PRUTYPE_H
