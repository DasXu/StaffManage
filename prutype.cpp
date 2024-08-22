#include "prutype.h"
#include "ui_prutype.h"

#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QMessageBox>

prutype::prutype(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::prutype)
{
    ui->setupUi(this);

    setWindowTitle("权限类型");
    QIcon icon(":/picture/4.jpg"); // 替换为您的Logo图片的文件路径
    setWindowIcon(icon);

    showInfo();
    connect(model, &QStandardItemModel::dataChanged, this, &prutype::handleDataChanged);
    ui->prutype_2->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->prutype_2->setSelectionMode(QAbstractItemView::SingleSelection);

//    //获取checkbox返回值
//    QList<QCheckBox*> checkBoxList;
    // 在构造函数或初始化函数中将这些复选框添加到列表中
    checkBoxList.append(ui->pushButton);
    checkBoxList.append(ui->PersonInfo);
    checkBoxList.append(ui->YearInfo);
    checkBoxList.append(ui->punishInfo);
    checkBoxList.append(ui->dutycbx);

    checkBoxList.append(ui->pushButton_2);
    checkBoxList.append(ui->reward);
    checkBoxList.append(ui->report);
    checkBoxList.append(ui->audit);

    checkBoxList.append(ui->pushButton_3);
    checkBoxList.append(ui->staff);
    checkBoxList.append(ui->manageRule);
    checkBoxList.append(ui->purType_2);
    connect(ui->checkBox, &QCheckBox::stateChanged, this, &prutype::selectAll);
    connect(ui->prutype_2, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onTableViewClicked(const QModelIndex&)));
}

prutype::~prutype()
{
    delete ui;
}

//新增类型
void prutype::on_addtype_clicked()
{
    int rowCount = model->rowCount();
    model->insertRow(rowCount);

    // 生成序号，允许用户编辑
    QStandardItem *indexItem = new QStandardItem("");
    model->setItem(rowCount, 0, indexItem);

    // 创建一个空的名称项，允许用户编辑
    QStandardItem *nameItem = new QStandardItem("");
    model->setItem(rowCount, 1, nameItem);

    // 设置QTableView的焦点到新增的行上，第一列（序号）开始编辑
    QModelIndex newIndex = model->index(rowCount, 0);
    ui->prutype_2->setCurrentIndex(newIndex);
    ui->prutype_2->edit(newIndex); // 启动编辑模式
}

//删除类型
void prutype::on_deltype_clicked()
{
    //实现tableview的单行选中功能
    ui->prutype_2->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->prutype_2->setSelectionMode(QAbstractItemView::SingleSelection);

    //实现删除功能
    QModelIndexList selectedIndexes =  ui->prutype_2->selectionModel()->selectedIndexes();
    if (selectedIndexes.isEmpty())
    {
        // 没有选中任何行
        QMessageBox::warning(this, tr("Tips"), tr("你未选中任何信息！"));
        return;
    }

    int rowToDelete = selectedIndexes.first().row();
    // 获取你需要删除的行的唯一标识，如主键值
    QVariant primaryKeyValue =  ui->prutype_2->model()->data( ui->prutype_2->model()->index(rowToDelete, 0));

    QSqlQuery query;
    query.prepare("DELETE FROM Purview WHERE id = :primaryKeyValue");
    query.bindValue(":primaryKeyValue", primaryKeyValue);
    if (query.exec())
    {
        // 删除成功
        ui->prutype_2->model()->removeRow(rowToDelete);
        QMessageBox::information(NULL,"提示","删掉咯~~");
        ui->prutype_2->setModel(ui->prutype_2->model());
    }
    else
    {
        // 删除失败
        QMessageBox::warning(NULL,"提示","失败啦！！！");
    }
    query.exec("SET @auto_id = 0;");
    query.exec("UPDATE Purview SET id = (@auto_id := @auto_id + 1);");
    query.exec("ALTER TABLE Purview AUTO_INCREMENT = 1;");
}

//保存权限--------------------------------------------------最好数据库列表名和复选框名相同
void prutype::on_ptsave_clicked()
{
    // 获取QItemSelectionModel
    QItemSelectionModel *selectionModel = ui->prutype_2->selectionModel();
    // 检查是否有选中的行
    if (selectionModel->hasSelection())
    {
        qDebug()<<"选中行了";
        // 获取选中的模型索引列表
        QModelIndexList selectedIndexes = selectionModel->selectedRows();
        // 获取第一列的内容
        if (!selectedIndexes.isEmpty())
        {
            qDebug()<<"选中行了2";
            QModelIndex firstColumnIndex = selectedIndexes.first();
            QVariant data = firstColumnIndex.data(); // 获取第一列的数据
            int id = data.toInt(); // 转换为字符串

            // 创建一个QMap来存储数据库列和勾选状态的映射
            QMap<QString, int> columnStatus;

            // 遍历复选框列表，检查勾选状态并添加到映射中
            for (QCheckBox *checkBox : checkBoxList)
            {
                QString columnName = checkBox->objectName(); // 假设复选框的objectName属性是数据库列名
                int status = checkBox->isChecked() ? 1 : 0;
                columnStatus[columnName] = status;
                qDebug() << "Column: " << columnName << ", Status: " << status;
            }

            // 更新数据库中的数据
            QSqlQuery query;
            query.prepare("UPDATE Purview SET "
                          "pushButton = :column1, "
                          "PersonInfo = :column2, "
                          "YearInfo = :column3, "
                          "punishInfo = :column4, "
                          "dutycbx= :column5, "
                          "pushButton_2 = :column6, "
                          "reward = :column7, "
                          "report = :column8, "
                          "audit = :column9, "
                          "pushButton_3 = :column10, "
                          "staff = :column11, "
                          "manageRule = :column12, "
                          "purType_2 = :column13, "
                          "purType = :column14 "
                          "WHERE id = :id"); // 添加适当的WHERE条件以更新特定行或所有行

            // 绑定勾选状态到数据库列
//            qDebug() << "Value for InfoCBX: " << columnStatus["InfoCBX"];
            query.bindValue(":column1", columnStatus["pushButton"]);
            query.bindValue(":column2", columnStatus["PersonInfo"]);
            query.bindValue(":column3", columnStatus["YearInfo"]);
            query.bindValue(":column4", columnStatus["punishInfo"]);
            query.bindValue(":column5", columnStatus["dutycbx"]);
            query.bindValue(":column6", columnStatus["pushButton_2"]);
            query.bindValue(":column7", columnStatus["reward"]);
            query.bindValue(":column8", columnStatus["report"]);
            query.bindValue(":column9", columnStatus["audit"]);
            query.bindValue(":column10", columnStatus["pushButton_3"]);
            query.bindValue(":column11", columnStatus["staff"]);
            query.bindValue(":column12", columnStatus["manageRule"]);
            query.bindValue(":column13", columnStatus["purType_2"]);
            query.bindValue(":column14", columnStatus["purType"]);
            query.bindValue(":id", id);
            // 绑定其他列的更新值

            if (query.exec())
            {
                // 更新成功，执行您的成功操作
                QMessageBox::information(NULL,"提示","权限保存成功！");
            }
            else
            {
                // 更新失败，执行您的失败操作
                QMessageBox::information(NULL,"提示",query.lastError().text());
                qDebug() << "Update error: " << query.lastError().text();
            }
        }
        else
        {
            qDebug()<<"为空";
        }
    }
    else
    {
        QMessageBox::warning(NULL,"提示","请选择一个用户类型");
    }
}

//取消
void prutype::on_ptcancel_clicked()
{
    close();
}

void prutype::showInfo()
{
    model->setColumnCount(2);
    model->setHorizontalHeaderItem(0, new QStandardItem("序号"));
    model->setHorizontalHeaderItem(1, new QStandardItem("名称"));
    //用户信息显示
    QString test = "SELECT id,name FROM Purview";
    QSqlQuery query(test);

    // 检查查询是否执行成功
    if (query.exec())
    {
        int row = 0;
        // 从查询结果中逐行添加数据到QStandardItemModel
        while (query.next())
        {
            // 从查询结果中逐行添加数据到QStandardItemModel
            QString column1Data = query.value(0).toString(); // 第一列数据
            QString column2Data = query.value(1).toString(); // 第二列数据

            // 创建QStandardItem并设置值
            QStandardItem *item1 = new QStandardItem(column1Data);
            QStandardItem *item2 = new QStandardItem(column2Data);

            // 添加项到模型的相应位置
            model->setItem(row, 0, item1);
            model->setItem(row, 1, item2);
            row++; // 增加行号
        }
    }
    QFont headerFont("song", 12, QFont::Bold);//表头字体修改
    ui->prutype_2->horizontalHeader()->setFont(headerFont);
    ui->prutype_2->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);//表头对齐设置
    ui->prutype_2->horizontalHeader()->setStyleSheet("background-color: lightgray; color: black;");//表头背景色
    QString headerStyle = "QHeaderView::section { background-color: lightblue; }";
    ui->prutype_2->horizontalHeader()->setStyleSheet(headerStyle);
    ui->prutype_2->setModel(model);
    //  ui->tableView->horizontalHeader()->setDefaultSectionSize(100); // 适应列宽
    ui->prutype_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);    //让表格和窗口相等
}

void prutype::handleDataChanged(const QModelIndex &index)
{
    if (index.isValid() && index.column() == 0)
    { // 仅在第一列数据变化时执行
        QString newIndexValue = index.data().toString();

        // 检查用户是否在新增的行按下Enter键
        if (index.row() == model->rowCount() - 1 && !newIndexValue.isEmpty())
        {
            // 将焦点切换到下一列（名称）
            QModelIndex newNameIndex = model->index(index.row(), 1);
            ui->prutype_2->setCurrentIndex(newNameIndex);
            ui->prutype_2->edit(newNameIndex); // 启动编辑模式
        }
    }
    else if (index.isValid() && index.column() == 1)
    { // 仅在第二列数据变化时执行
        QString newName = index.data().toString();

        // 检查用户是否在新增的行按下Enter键
        if (index.row() == model->rowCount() - 1 && !newName.isEmpty())
        {
            // 插入新数据到数据库

            QSqlQuery query;
            query.prepare("INSERT INTO Purview (id, name) VALUES (:id, :name)");
            query.bindValue(":id", model->data(model->index(index.row(), 0)).toInt());
            query.bindValue(":name", newName);

            if (query.exec())
            {
                // 插入成功，执行您的成功操作
                qDebug() << "Insert successful";
            }
            else
            {
                // 插入失败，执行您的失败操作
                QMessageBox::warning(NULL,"失败！","序号唯一不可重复，请查看是否重复序号。");
                qDebug() << "Insert error: " << query.lastError().text();
            }
        }
    }
}

void prutype::selectAll(int state)
{
     qDebug() << "Checkbox State Changed to: " << state;
    if (state == Qt::Checked)
    {
        qDebug() << "Setting checkbox" << ui->checkBox->objectName() << "to true";
        // 第一个复选框被勾选
        // 遍历并设置其他复选框的状态
        QList<QCheckBox*> checkBoxList = findChildren<QCheckBox*>(); // 获取所有复选框
        for (QCheckBox* checkBox : checkBoxList)
        {
            checkBox->setChecked(true);
            qDebug() << "Checkbox ObjectName: " << checkBox->objectName();
        }
    }
    else
    {
        QList<QCheckBox*> checkBoxList = findChildren<QCheckBox*>(); // 获取所有复选框
        for (QCheckBox* checkBox : checkBoxList)
        {
            checkBox->setChecked(false);
        }
    }
}

void prutype::onTableViewClicked(const QModelIndex &index)
{
    // 获取所选行的用户类型
    QString userType = index.data().toString();

    // 查询数据库，获取该用户类型的权限数据
    userPermissions = getUserPermissionsFromDatabase(userType);

    // 根据权限数据更新复选框的状态
    for (QCheckBox* checkBox : checkBoxList)
    {
        QString permissionName = checkBox->objectName();
        int permissionValue = userPermissions.value(permissionName, 0); // 默认为0

        // 根据权限值来设置复选框状态
        checkBox->setChecked(permissionValue == 1);
    }
}

QMap<QString, int> prutype::getUserPermissionsFromDatabase(const QString& userType)
{
    QMap<QString, int> permissions;

    // 执行数据库查询，根据用户类型获取权限数据，然后将其存储在 permissions 中
    QSqlQuery query;
    query.prepare("select pushButton,PersonInfo,YearInfo,punishInfo,dutycbx,pushButton_2,"
                  "reward,report,audit,pushButton_3,staff,manageRule,purType_2,purType "
                  "from Purview where name = :type");
    query.bindValue(":type",userType);
    if (query.exec())
    {
        if (query.next())
        {
            // 读取每个权限列的数值并存储在userPermissions中
            permissions["pushButton"] = query.value(0).toInt();
            permissions["PersonInfo"] = query.value(1).toInt();
            permissions["YearInfo"] = query.value(2).toInt();
            permissions["punishInfo"] = query.value(3).toInt();
            permissions["dutycbx"] = query.value(4).toInt();
            permissions["pushButton_2"] = query.value(5).toInt();
            permissions["reward"] = query.value(6).toInt();
            permissions["report"] = query.value(7).toInt();
            permissions["audit"] = query.value(8).toInt();
            permissions["pushButton_3"] = query.value(9).toInt();
            permissions["staff"] = query.value(10).toInt();
            permissions["manageRule"] = query.value(11).toInt();
            permissions["purType_2"] = query.value(12).toInt();
            permissions["purType"] = query.value(13).toInt();
        }
    }
    else
    {
        // 查询失败，处理错误
        qDebug() << "Error: " << query.lastError().text();
    }

    return permissions;
}
