#include "rules.h"
#include "ui_rules.h"

#include <QSqlQuery>

rules::rules(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::rules)
{
    ui->setupUi(this);
    setWindowTitle("员工行为管理条例");
    QIcon icon(":/picture/4.jpg"); // 替换为您的Logo图片的文件路径
    setWindowIcon(icon);

    //连接数据库
    db.setDatabaseName("StaffTest");
    db.setHostName("localhost");
    db.setPort(3306);
    db.setUserName("root");
    db.setPassword("123456");

    if(!db.open())
    {
        QMessageBox::warning(this,"连接提示",db.lastError().text());
    }

    allshow();
}

rules::~rules()
{
    delete ui;
}

void rules::on_ruleAdd_clicked()
{
    ruleEdit *newedit = new ruleEdit(this);
    newedit->setModal(false);
    newedit->setWindowTitle("新增条例");

    int i = newedit->exec();
    qDebug()<<"i:" <<i;
    if (i == QDialog::Accepted)
    {
        qDebug()<<"result:" <<i;
        // 用户点击了保存按钮，更新数据库模型以反映更改
        QString setQuer = "SELECT * FROM ManageRule";
        model->setQuery(setQuer,db);
        ui->ruleShow->horizontalHeader()->hideSection(4);//隐藏删除标记
    }
    else
    {
        //        QMessageBox::warning(NULL,"提示","未执行任何操作");
    }

}

void rules::on_ruleDelete_clicked()
{
    //实现tableview的单行选中功能
    ui->ruleShow->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->ruleShow->setSelectionMode(QAbstractItemView::SingleSelection);

    //实现添加删除标记功能
    QModelIndexList selectedIndexes = ui->ruleShow->selectionModel()->selectedIndexes();
        if (selectedIndexes.isEmpty())
        {
            // 没有选中任何行
            QMessageBox::warning(this, tr("警告"), tr("未选中任何信息！"));
            return;
        }

        int rowToDelete = selectedIndexes.first().row();
        // 获取你需要删除的行的唯一标识，如主键值
        QVariant primaryKeyValue = ui->ruleShow->model()->data(ui->ruleShow->model()->index(rowToDelete, 0));
        QVariant number = 1;

        QSqlQuery query;
        query.prepare("UPDATE ManageRule set del = :delete WHERE manageID = :id");
        query.bindValue(":id", primaryKeyValue);
        query.bindValue(":delete",number);

        if (query.exec())
        {
            // 删除成功
            ui->ruleShow->model()->removeRow(rowToDelete);
            QMessageBox::information(NULL,"提示","已删除该条例");
            QString setQuer = "SELECT * FROM ManageRule where del != 1";
            QSqlQueryModel *model = qobject_cast<QSqlQueryModel*>(ui->ruleShow->model());
            model->setQuery(setQuer,db);
        }
        else
        {
            // 删除失败
            QMessageBox::warning(NULL,"提示",query.lastError().text());
        }
}

void rules::on_ruleEdit_clicked()
{
    QModelIndexList selectedIndexes = ui->ruleShow->selectionModel()->selectedIndexes();

    if (!selectedIndexes.isEmpty())
    {
            QModelIndex selectedIndex = selectedIndexes.at(0); // Assuming single selection
            int row = selectedIndex.row();

            QSqlQueryModel *model = qobject_cast<QSqlQueryModel*>(ui->ruleShow->model());

            if (model)
            {
                QVariant id = model->data(model->index(row,0),Qt::DisplayRole);
                QVariant name = model->data(model->index(row, 2), Qt::DisplayRole); // Replace 'yourNameColumnIndex' with the actual column index
                QVariant explain = model->data(model->index(row, 3), Qt::DisplayRole); // Replace 'yourEmailColumnIndex' with the actual column index
                QVariant score = model->data(model->index(row, 4), Qt::DisplayRole);

                //创建修改窗口并传输数据
                ruleedit2 *newrule2 = new ruleedit2(id.toInt(), name.toString(), explain.toString(),score.toInt(),this);
                newrule2->setModal(false);

                int result = newrule2->exec();
                qDebug()<<"result:" <<result;
                if (result == QDialog::Accepted)
                {
                    // 用户点击了保存按钮，更新数据库模型以反映更改
                    QString setQuer = "SELECT * FROM ManageRule";
                    model->setQuery(setQuer,db);
                }
//                else
//                {
//                    QMessageBox::warning(NULL,"提示","未执行任何操作");
//                }
            }
        }
    else
    {
            QMessageBox::warning(this, tr("警告"), tr("请选择想要修改的条例"));
    }
}

void rules::allshow()
{
    //用户信息显示
    QString test1 = "SELECT * FROM ManageRule";
    model->setQuery(test1,db);

    //更改表头
    QStringList info1;
    info1<<"条例号"<<"行为类型"<<"行为名称"<<"详细说明"<<"分数";
    for (int column = 0; column < model->columnCount(); ++column)
    {
        if (column < info1.size())
        {
            model->setHeaderData(column, Qt::Horizontal, info1.at(column));
        }
    }
    ui->ruleShow->viewport()->update();//刷新试图

    QFont headerFont("song", 12, QFont::Bold);//表头字体修改
    ui->ruleShow->horizontalHeader()->setFont(headerFont);
    ui->ruleShow->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);//表头对齐设置
    ui->ruleShow->horizontalHeader()->setStyleSheet("background-color: lightgray; color: black;");//表头背景色
    QString headerStyle = "QHeaderView::section { background-color: lightblue; }";
    ui->ruleShow->horizontalHeader()->setStyleSheet(headerStyle);

    ui->ruleShow->setModel(model);
    ui->ruleShow->horizontalHeader()->hideSection(5);//隐藏删除标记列
    qDebug() << "111: " << model->rowCount();
    //  ui->tableView->horizontalHeader()->setDefaultSectionSize(100); // 适应列宽
    ui->ruleShow->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);    //让表格和窗口相等
}
