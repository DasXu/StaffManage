#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QSqlTableModel>
#include <QMessageBox>
#include <QTimer>
#include <QDateTime>
#include <QStandardItemModel>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>
#include <QSqlError>
#include <QTableView>
#include <QFileDialog>
#include <QAxObject>
#include <QDesktopServices>
#include <QProcess>
#include <QAction>
#include <QDateTimeEdit>

#include "addnew.h"
#include "changeuser.h"
#include "prutype.h"
#include "detial.h"
#include "login.h"
#include "rules.h"
#include "postscore.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void pItable();//个人信息表初始化函数
    void staffshow();//用户管理-信息显示函数
    void deleteuser();//删除用户
    void sendshow();//修改编辑用户
    void TableExcelout(QTableView *table,QString title);//导出tableview
    void modifyP();//改密码
    void punInfo();
    void submitRewardInfo();
    void submitReportInfo();
    void addNewP();
    void checkInfo();
    void yearInitialize();
    void personInfoModify();
    void aggree();

    void refuse();
    void detialBack();
    void refuse2();
    void tab3TableShow();
    void tab3count();

    void ThankText();
    void imageshow();

    void DailyInfo();//工作日历--每日信息
    void workInfo();

    void showButton(const QMap<QString, int>& userPermissions);

    void dailyAdd();
    void dailyRenew();//工作日历，信息更新和插入
    void dailyshow();//及时更新工作日历--每日记录信息
    void YMDshow();//及时更新工作日历--工作统计
    void execute(QString text, QStandardItemModel *model2);
    void updateTableView();
public slots:

    void getlogin(const QString &account);//接收登录账号的信息
    void detrefuse();
    void checkit();
    void checkit2();
    void connectaDetial(const QModelIndex& index);
    void DialogClosed();//关闭窗口更新状态栏

signals:
    void sendInfo(const QString &name, const QString &sex,const QString &accounbt,const QString &password,const QDate &birth,
                               const QString &post, const QString &clas,const QDate &year,const QString &address,
                               const QString &phone,const QString &tel,const QString &status,const QString &type);

private slots:
//    void onTabWidgetCurrentChanged(int index);

    void on_PersonInfo_clicked();
    void on_YearInfo_clicked();
    void on_punishInfo_clicked();
    void on_reward_clicked();
    void on_report_clicked();
    void on_audit_clicked();
    void on_staff_clicked();
    void on_accountNo_clicked();
    void on_addnew_clicked();
    void on_change_clicked();
    void on_del_clicked();
    void on_purType_clicked();
    void on_detial_clicked();
    void on_in_clicked();
    void on_out_clicked();
    void on_tab7check_clicked();
    void on_renew_clicked();
    void timeUpdate();//时间刷新函数
    void tuichu();//退出主程序
    void aboutThis();//关于本软件

    void on_tab1ok_clicked();

    void on_submit_clicked();

    void on_submit2_clicked();

    void on_more_clicked();

    void on_punishDelete_clicked();

    void on_PunRenew_clicked();

    void on_PunCheck_clicked();

    void on_manageRule_clicked();

    void on_aggree_clicked();

    void on_refuse_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_back_3_clicked();

    void on_back_2_clicked();

    void on_back_1_clicked();

    void on_purType_2_clicked();

    void on_tab1choose_clicked();

    void on_pushButton_7_clicked();

    void on_punishBtn_clicked();

    void on_structure_clicked();

    void on_dailyInfo_ok_clicked();

    void on_newAdd_rule_clicked();

private:
    Ui::MainWindow *ui;

    addnew * a;//新增用户
    changeUser * c;//修改用户
    detial *d;//详情界面
    QSqlQueryModel *model = new QSqlQueryModel;
    QStandardItemModel *model5 = new QStandardItemModel;
    QTimer *timer_calendar;//时钟
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL","first");
    QString nowaccount;//从getlogin获取了传送来的账号信息
    rules *newrule = nullptr;
    postScore *newpost = nullptr;
    prutype *p = nullptr;//权限类型界面
    bool readAndInsertTableData(const QString &filePath);//导入用
    bool isSimulatingButtonClick;
    QList<QPushButton*> buttonList;
    void showArguments(QStringList& arguments);
    bool exportExcelFile(QTableView *v,
                         const QString& sFileName,
                         bool bAddTime/* = true*/);
};

#endif // MAINWINDOW_H
