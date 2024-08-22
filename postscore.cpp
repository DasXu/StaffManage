#include "postscore.h"
#include "ui_postscore.h"

postScore::postScore(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::postScore)
{
    ui->setupUi(this);

    setWindowTitle("岗位管理");
    QIcon icon(":/picture/4.jpg"); // 替换为您的Logo图片的文件路径
    setWindowIcon(icon);

    //连接数据库
    db.setDatabaseName("StaffTest");
    db.setHostName("localhost");
    db.setPort(3306);
    db.setUserName("root");
    db.setPassword("123456");

    initial();

    connect(ui->postInfo, &QTableView::viewportEntered, this, &postScore::handleEnterKey);
}

postScore::~postScore()
{
    delete ui;
}

void postScore::on_addPost_clicked()
{
    int newRow = model->rowCount();
    model->insertRow(newRow);
    QModelIndex index = model->index(newRow, 0);

    // 将焦点设置到新增行的第一个单元格，以支持直接编辑
    ui->postInfo->setCurrentIndex(index);
    ui->postInfo->edit(index);
}

void postScore::on_deletePost_clicked()
{
    //实现tableview的单行选中功能
    ui->postInfo->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->postInfo->setSelectionMode(QAbstractItemView::SingleSelection);

    //实现删除功能
    QModelIndexList selectedIndexes =  ui->postInfo->selectionModel()->selectedIndexes();
    if (selectedIndexes.isEmpty())
    {
        // 没有选中任何行
        QMessageBox::warning(this, tr("Tips"), tr("你未选中任何信息！"));
        return;
    }

    int rowToDelete = selectedIndexes.first().row();
    // 获取你需要删除的行的唯一标识，如主键值
    QVariant primaryKeyValue =  ui->postInfo->model()->data( ui->postInfo->model()->index(rowToDelete, 0));

    QSqlQuery query;
    query.prepare("DELETE FROM PostScore WHERE id = :primaryKeyValue");
    query.bindValue(":primaryKeyValue", primaryKeyValue);
    if (query.exec())
    {
        // 删除成功
        ui->postInfo->model()->removeRow(rowToDelete);
        QMessageBox::information(NULL,"提示","删掉咯~~");

        //刷新表格
        model->setTable("PostScore"); // 设置要查询的表名
        model->setEditStrategy(QSqlTableModel::OnFieldChange); // 设置编辑策略，这里是在字段更改时立即更新到数据库
        model->select(); // 执行查询
        ui->postInfo->setModel(model);
        model->setHeaderData(0, Qt::Horizontal, "序号");
        model->setHeaderData(1, Qt::Horizontal, "岗位名称");
        model->setHeaderData(2, Qt::Horizontal, "岗位积分");
        ui->postInfo->show();
    }
    else
    {
        // 删除失败
        QMessageBox::warning(NULL,"提示","失败啦！！！");
    }
}

void postScore::initial()
{
    QString test = "select * from PostScore";
    model->setTable("PostScore"); // 替换为你的数据库表名
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    model->select();

    QStringList info;
    info<<"序号"<<"岗位名称"<<"岗位积分";
    int numColumns = info.size(); // 设置模型的列数
    for (int column = 0; column < numColumns; ++column)
    {
        if (column < info.size())
        {
            model->setHeaderData(column, Qt::Horizontal, info.at(column));
        }
    }

    QFont headerFont("song", 12, QFont::Bold);//表头字体修改
    ui->postInfo->horizontalHeader()->setFont(headerFont);
    ui->postInfo->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);//表头对齐设置
    ui->postInfo->horizontalHeader()->setStyleSheet("background-color: lightgray; color: black;");//表头背景色
    QString headerStyle = "QHeaderView::section { background-color: lightblue; }";
    ui->postInfo->horizontalHeader()->setStyleSheet(headerStyle);

    ui->postInfo->setModel(model);
    //  ui->tableView->horizontalHeader()->setDefaultSectionSize(100); // 适应列宽
    ui->postInfo->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);    //让表格和窗口相等
}

void postScore::handleEnterKey()
{
    int currentRow = ui->postInfo->currentIndex().row();
    int currentColumn = ui->postInfo->currentIndex().column();

    // 如果当前单元格是最后一列，跳转到下一行的第一个单元格
    if (currentColumn == model->columnCount() - 1)
    {
        if (currentRow < model->rowCount() - 1) {
            QModelIndex nextIndex = model->index(currentRow + 1, 0);
            ui->postInfo->setCurrentIndex(nextIndex);
            ui->postInfo->edit(nextIndex);
        }
    }
    else
    {
        // 否则，跳转到当前行的下一个单元格
        QModelIndex nextIndex = model->index(currentRow, currentColumn + 1);
        ui->postInfo->setCurrentIndex(nextIndex);
        ui->postInfo->edit(nextIndex);
    }

    // 保存数据到数据库
    model->submitAll();
    // 刷新模型以更新显示
    model->select();
}
