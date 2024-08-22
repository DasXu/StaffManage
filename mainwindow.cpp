#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTextCodec>
    QMap<QString, int> checkPruview(const QString& account);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFont font("楷体", 12, QFont::Light); // 使用楷体字体，大小为 12，加粗
    statusBar()->setFont(font);
    statusBar()->showMessage("当前状态：首页");

    this->setWindowTitle("锻造工厂员工行为规范考核系统V1.0");
    // 设置窗口图标
    QIcon icon(":/picture/5.jpg"); // 替换为您的Logo图片的文件路径
    setWindowIcon(icon);

    ui->frame->setStyleSheet("background-color: rgb(170, 255, 127);");
    ui->frame->setFrameShadow(QFrame::Sunken); // 设置边框阴影效果

    QImage image;
    image.load(":/picture/01.png");
    ui->logo->setPixmap(QPixmap::fromImage(image));
    ui->logo->setScaledContents(true);//让图片适应label
    ui->logo->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    //连接数据库
    db.setDatabaseName("StaffTest");
    db.setHostName("localhost");
    db.setPort(3306);
    db.setUserName("root");
    db.setPassword("123456");
    db.open();

    //显示当前时间
    timer_calendar = new QTimer(this);//new一个QTimer对象
    connect(timer_calendar,SIGNAL(timeout()),this,SLOT(timeUpdate()));//连接槽函数
    timer_calendar->start(1000);//每一秒溢出一次进入槽函数

    //默认个人信息为主窗口
    ui->stackedWidget->setCurrentIndex(0);//默认功能首页
    ui->stackedWidget_2->setCurrentIndex(7);//默认功能首页

    connect(ui->actionAbout,&QAction::triggered, this ,&MainWindow::aboutThis);
    connect(ui->action2,&QAction::triggered, this ,&MainWindow::tuichu);

    //默认初始化奖惩提交界面
    QStringList string;
    QSqlQuery query("SELECT name  FROM ManageRule where type = '处罚' ");
    while (query.next())
    {
        QString goodsno = query.value(0).toString();
        string.append(goodsno);
    }
    ui->punishReason->addItems(string);
    QStringList strings;
    query.exec("SELECT name  FROM ManageRule where type = '奖励' ");
    while (query.next())
    {
        QString goodsno = query.value(0).toString();
        strings.append(goodsno);
    }
    ui->rewardReason->addItems(strings);
    connect(ui->PunKeyWord,SIGNAL(returnPressed()),this,SLOT(checkit()));//按下enter便可查询--奖惩审核
    connect(ui->keyWord,SIGNAL(returnPressed()),this,SLOT(checkit2()));//按下enter便可查询--账号管理
    connect(ui->tab3table, SIGNAL(clicked(const QModelIndex&)), this, SLOT(connectaDetial(const QModelIndex&)));
    ui->dateEdit->setDisplayFormat("yyyy");//只显示年份
    QDate currentDate = QDate::currentDate(); // 获取当前日期
    ui->dateEdit->setDate(currentDate); // 将当前日期设置给 QDateEdit
    connect(ui->dateEdit, &QDateEdit::editingFinished, this, &MainWindow::on_YearInfo_clicked);//年度履历选择年份按下enter更新内容

    //添加按钮到容器
    buttonList.append(ui->pushButton);
    buttonList.append(ui->PersonInfo);
    buttonList.append(ui->YearInfo);
    buttonList.append(ui->punishInfo);
    buttonList.append(ui->pushButton_2);
    buttonList.append(ui->reward);
    buttonList.append(ui->report);
    buttonList.append(ui->audit);
    buttonList.append(ui->pushButton_3);
    buttonList.append(ui->staff);
    buttonList.append(ui->manageRule);
    buttonList.append(ui->purType_2);
    buttonList.append(ui->purType);

    //关联下拉框和日历
    ui->YMD_choose->setDate(currentDate);
    ui->dailyDate->setDate(currentDate);
    QStringList list;
    list<<"  "<<"年"<<"月"<<"日";
    ui->YMD->addItems(list);
    QDate date;
    connect(ui->YMD, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index){
        if(index == 1)
        { // 选中年
            ui->YMD_choose->setDisplayFormat("yyyy"); // 设置只显示年份
            ui->YMD_choose->setCalendarPopup(false); // 关闭日历弹出模式
        }
        else if (index == 2)
        { // 选中月
            ui->YMD_choose->setDisplayFormat("yyyy-MM"); // 设置显示年月
            ui->YMD_choose->setCalendarPopup(true); // 打开日历弹出模式
            ui->YMD_choose->setDateRange(QDate(date.year(), 1, 1), QDate(date.year(), 12, 1)); // 限制日期范围为所选年份的月份范围
        }
        else if (index == 3)
        { // 选中日
            ui->YMD_choose->setDisplayFormat("yyyy-MM-dd"); // 设置显示年月日
            ui->YMD_choose->setCalendarPopup(true); // 打开日历弹出模式
            ui->YMD_choose->setDateRange(QDate(date.year(), date.month(), 1), QDate(date.year(), date.month(), date.daysInMonth())); // 限制日期范围为所选年月的日期范围
        }
    });
    dailyshow();//查看任意日期的个人记录
    YMDshow();//查看任意时间段的工作记录
}

MainWindow::~MainWindow()
{
    delete ui;
}

//时间刷新
void MainWindow::timeUpdate()
{
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd \nhh:mm:ss  dddd");
    ui->clock->setText(str);
}

void MainWindow::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    statusBar()->showMessage("当前状态：信息查询");
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    statusBar()->showMessage("当前状态：奖惩管理");
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    statusBar()->showMessage("当前状态：用户管理 ");
}

void MainWindow::on_back_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    statusBar()->showMessage("当前状态：首页");
    ui->stackedWidget_2->setCurrentIndex(7);//默认功能首页
}

void MainWindow::on_back_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    statusBar()->showMessage("当前状态：首页");
    ui->stackedWidget_2->setCurrentIndex(7);//默认功能首页
}

void MainWindow::on_back_1_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    statusBar()->showMessage("当前状态：首页");
    ui->stackedWidget_2->setCurrentIndex(7);//默认功能首页
}

//1.个人信息
void MainWindow::on_PersonInfo_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(0);//设置为主窗口
    statusBar()->showMessage("当前状态：信息查询-->个人信息");
//    pItable();
}

//1.1修改个人信息
void MainWindow::on_tab1ok_clicked()
{
    modifyP();
    personInfoModify();
    QString path = ui->tab1Line->text();//获取路径准备上传头像
    QFile imageFile(path);
    if (!imageFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open image file.";
        return;
    }
    else
    {
        QByteArray imageData = imageFile.readAll();
        QString id = nowaccount;
        if(imageData.size() <= 1000*1024)
        {
            QSqlQuery query;
            query.prepare("UPDATE Image SET image = :path WHERE account = :recordId");
            query.bindValue(":path",imageData);
            query.bindValue(":recordId",id);
            if(query.exec())
            {
                QMessageBox::information(NULL,"提示","上传头像成功！");
            }
            else
            {
                qDebug()<<"reason is = "<<query.lastError().text();
            }
        }
        else
        {
            QMessageBox::warning(NULL,"提示","请选择小于1MB的图片");
        }
    }
}

//1.2更换头像
void MainWindow::on_tab1choose_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "选择照片", QDir::homePath(), "Image Files (*.png *.jpg *.jpeg *.bmp)");
    if (!filePath.isEmpty())
    {
        ui->tab1Line->setText(filePath);
    }
}

//2.年度履历
void MainWindow::on_YearInfo_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(1);//设置为主窗口
    ui->textEdit->setReadOnly(true);
    yearInitialize();
    statusBar()->showMessage("当前状态：信息查询-->年度履历");
    ThankText();//致谢板块
}

//组织架构
void MainWindow::on_structure_clicked()
{
    QMessageBox::information(NULL,"温馨提示","该功能还在研发中~");
}


//3.奖惩详情
void MainWindow::on_punishInfo_clicked()
{
    QStringList list;
    list<<"  "<<"开心"<<"伤心"<<"生气"<<"抑郁";
    ui->dailyMood->addItems(list);

    QStringList list2;
    list2<<"  "<<"晴朗"<<"多云"<<"阴天";
    ui->dailyWeather->addItems(list2);

    ui->stackedWidget_2->setCurrentIndex(2);//设置为主窗口
    ui->tab3table->setEditTriggers(QAbstractItemView::NoEditTriggers);//设置为只读
    tab3TableShow();//奖惩记录
//    workInfo();//工作情况
    DailyInfo();//日记
    tab3count();//计算显示积分
    ui->startPoint->setReadOnly(true);
    ui->plusPoint->setReadOnly(true);
    ui->minPoint->setReadOnly(true);
    ui->endPoint->setReadOnly(true);
    statusBar()->showMessage("当前状态：信息查询-->工作日历");
}

void MainWindow::on_newAdd_rule_clicked()
{

}

//3.1 提交每日日记
void MainWindow::on_dailyInfo_ok_clicked()
{
    QDate currentDate = QDate::currentDate(); // 获取当前日期
    QDate date = ui->dailyDate->date();
    if(currentDate == date)
    {
        //判断数据库今日有没有信息，有则更新，无则新增
        QSqlQuery query;
        query.prepare("select count(*) from DailyInfo where date = :date");
        query.bindValue(":date",date);
        if(query.exec() && query.first())
        {
            int i = query.value(0).toInt();
            if(i != 0)
            {
                dailyRenew();
            }
            else
            {
                dailyAdd();
            }
        }
    }
    else
    {
        QMessageBox::warning(NULL,"提示","仅可记录/修改当日信息");
    }
}

//4.奖励申请
void MainWindow::on_reward_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(3);//设置为主窗口

    QDateTime currentDateTime = QDateTime::currentDateTime();
    // 设置 QDateTimeEdit 的值为当前日期和时间
    ui->rewardTime->setDateTime(currentDateTime);
    statusBar()->showMessage("当前状态：奖惩管理-->奖励申请");
}

//提交奖励申请
void MainWindow::on_submit_clicked()
{
    submitRewardInfo();
}

//5.检举
void MainWindow::on_report_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(4);//设置为主窗口

    QDateTime currentDateTime = QDateTime::currentDateTime();
    // 设置 QDateTimeEdit 的值为当前日期和时间
    ui->punishTime->setDateTime(currentDateTime);
    statusBar()->showMessage("当前状态：奖惩管理-->检举");
}

//提交检举
void MainWindow::on_submit2_clicked()
{
    submitReportInfo();
}

//奖励证明材料
void MainWindow::on_pushButton_7_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "选择证明材料", QDir::homePath(), "Image Files (*.png *.jpg *.jpeg *.bmp)");
    if (!filePath.isEmpty())
    {
        ui->rewardProve->setText(filePath);
    }
}

//检举证明材料
void MainWindow::on_punishBtn_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "选择证明材料", QDir::homePath(), "Image Files (*.png *.jpg *.jpeg *.bmp)");
    if (!filePath.isEmpty())
    {
        ui->punishProve->setText(filePath);
    }
}

//6.奖惩审核
void MainWindow::on_audit_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(5);//设置为主窗口
    punInfo();
    QAbstractItemModel *tableModel = ui->tableView_7->model();
    int numColumns = tableModel->columnCount();
    for (int column = 0; column < numColumns; ++column)
    {
        QString headerText = tableModel->headerData(column, Qt::Horizontal).toString();
        ui->PunKeyType->addItem(headerText);
    }
    statusBar()->showMessage("当前状态：奖惩管理-->奖惩审核");
}

//6.1查看详情
void MainWindow::on_detial_clicked()
{
    checkInfo();
}


//6.2 导出
void MainWindow::on_more_clicked()
{
    TableExcelout(ui->tableView_7,"奖惩信息审核表");
}

//6.3查询
void MainWindow::on_PunCheck_clicked()
{
    checkit();
//    QString sqlQuery = "SELECT * FROM UserInfo WHERE " + text + " = :variable2";
}

//6.4删除
void MainWindow::on_punishDelete_clicked()
{
    //实现tableview的单行选中功能
    ui->tableView_7->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView_7->setSelectionMode(QAbstractItemView::SingleSelection);

    //实现删除功能
    QModelIndexList selectedIndexes = ui->tableView_7->selectionModel()->selectedIndexes();
        if (selectedIndexes.isEmpty())
        {
            // 没有选中任何行
            return;
        }

        int rowToDelete = selectedIndexes.first().row();
        // 获取你需要删除的行的唯一标识，如主键值
        QVariant primaryKeyValue = ui->tableView_7->model()->data(ui->tableView_7->model()->index(rowToDelete, 0));

        QSqlQuery query;
        query.prepare("DELETE FROM punishInfo WHERE id = :primaryKeyValue");
        query.bindValue(":primaryKeyValue", primaryKeyValue);
        if (query.exec())
        {
            // 删除成功
            ui->tableView_7->model()->removeRow(rowToDelete);

            // 用户点击了保存按钮，更新数据库模型以反映更改
            //自动填补id号
            QSqlQuery query;
            query.exec("SET @auto_id = 0;");
            query.exec("UPDATE punishInfo SET id = (@auto_id := @auto_id + 1);");
            query.exec("ALTER TABLE punishInfo AUTO_INCREMENT = 1;");

            QString setQuer = "SELECT * FROM punishInfo";
            model->setQuery(setQuer,db);

            QMessageBox::information(NULL,"提示","删掉咯~~");
        }
        else
        {
            // 删除失败
            QMessageBox::warning(NULL,"提示","失败啦！！！");
        }
}

//刷新
void MainWindow::on_PunRenew_clicked()
{
     punInfo();
}

//同意申请
void MainWindow::on_aggree_clicked()
{
    aggree();
}
//拒绝申请
void MainWindow::on_refuse_clicked()
{
    refuse();
}

//7.用户管理
void MainWindow::on_staff_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(6);//设置为主窗口
    staffshow();
    QAbstractItemModel *tableModel = ui->tableView->model();
    int numColumns = tableModel->columnCount();
    for (int column = 0; column < numColumns; ++column)
    {
        QString headerText = tableModel->headerData(column, Qt::Horizontal).toString();
        ui->keyType->addItem(headerText);
    }
    statusBar()->showMessage("当前状态：用户管理-->账号管理");
}

//7.1新增
void MainWindow::on_addnew_clicked()
{
    addNewP();
}

//7.2修改
void MainWindow::on_change_clicked()
{
    sendshow();
}

//7.3删除
void MainWindow::on_del_clicked()
{
    deleteuser();
    QString setQuer = "SELECT * FROM UserInfo";
    model->setQuery(setQuer,db);
}

//7.4权限类型
void MainWindow::on_purType_clicked()
{
    if (p == nullptr)
    {
        p = new prutype(this);
        p->setModal(false);
        // 在窗口A关闭时，将其指针设置为 nullptr
        connect(newrule, &QDialog::finished, [=](int result) {
            if (result == QDialog::Accepted || result == QDialog::Rejected)
            {
                newrule = nullptr;
            }
        });
    }
    p->show();
    p->raise();
    statusBar()->showMessage("当前状态：用户管理-->权限类型");
    connect(p, &prutype::rejected, this, &MainWindow::DialogClosed);
}

//7.5导出
void MainWindow::on_out_clicked()
{
   TableExcelout(ui->tableView,"员工信息");
}

//7.6导入
void MainWindow::on_in_clicked()
{
//    QString filePath = QFileDialog::getOpenFileName(this, "选择要导入的表格文件", "", "表格文件 (*.csv *.xlsx *.xls)");

//    if (!filePath.isEmpty())
//    {
//        // 读取Excel表格中的内容
//        QXlsx::Document xlsx(filePath);
//        for (int row = 1; row <= xlsx.dimension().rowCount(); ++row)
//        {
//            QString name = xlsx.read(row, 1).toString();
//            int age = xlsx.read(row, 2).toInt();

//            // 与数据库内容进行比对
//            QSqlQuery query;
//            query.prepare("SELECT COUNT(*) FROM YourTable WHERE Name = :name AND Age = :age");
//            query.bindValue(":name", name);
//            query.bindValue(":age", age);
//            query.exec();
//            query.next();
//            int count = query.value(0).toInt();

//            // 如果数据库中没有找到匹配的条目，则将Excel中的内容添加到数据库中
//            if (count == 0)
//            {
//                QSqlQuery insertQuery;
//                insertQuery.prepare("INSERT INTO YourTable (Name, Age) VALUES (:name, :age)");
//                insertQuery.bindValue(":name", name);
//                insertQuery.bindValue(":age", age);
//                insertQuery.exec();
//            }
//        }

//        // 更新QTableView
//        updateTableView();
//    }
}

void MainWindow::updateTableView()
{

}

bool MainWindow::readAndInsertTableData(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "无法打开文件：" << file.errorString();
        return false;
    }
    QSqlQuery query;
    // 循环读取文件的每一行
    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        // 解析每一行的数据，以适应你的表格结构
        // 例如，假设文件中每一行是逗号分隔的数据
        QStringList data = line.split(",");
        if (data.size() < 2)
        {
            qDebug() << "行数据无效：" << line;
            continue;
        }

        QString data1 = data[0];
        QString data2 = data[1];

        // 插入数据到数据库表格中
        QString insertSql = "INSERT INTO Tset (name, account, password, post, sex, birth, address, phone, tel, status, startY, class, type) "
                            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
        query.prepare(insertSql);
        query.addBindValue(data1);
        query.addBindValue(data2);

        if (!query.exec())
        {
            qDebug() << "插入数据时发生错误：" << query.lastError().text();
            return false;
        }
    }

    // 关闭文件
    file.close();
    // 如果成功执行了所有操作，返回 true
    return true;
}

//7.7查询用户
void MainWindow::on_tab7check_clicked()
{
    checkit2();
}

//7.8刷新用户信息列表
void MainWindow::on_renew_clicked()
{
    staffshow();
}

//管理条例
void MainWindow::on_manageRule_clicked()
{
    if (newrule == nullptr)
    {
        newrule = new rules(this);
        newrule->setModal(false);
        // 在窗口A关闭时，将其指针设置为 nullptr
        connect(newrule, &QDialog::finished, [=](int result) {
            if (result == QDialog::Accepted || result == QDialog::Rejected)
            {
                newrule = nullptr;
            }
        });
    }
    newrule->show();
    newrule->raise();
    statusBar()->showMessage("当前状态：用户管理-->管理条例");
    connect(newrule, &rules::rejected, this, &MainWindow::DialogClosed);
}

//岗位管理
void MainWindow::on_purType_2_clicked()
{
    if (newpost == nullptr)
    {
        newpost = new postScore(this);
        newpost->setModal(false);
        // 在窗口A关闭时，将其指针设置为 nullptr
        connect(newrule, &QDialog::finished, [=](int result) {
            if (result == QDialog::Accepted || result == QDialog::Rejected)
            {
                newrule = nullptr;
            }
        });
    }
    newpost->show();
    newpost->raise();
    statusBar()->showMessage("当前状态：用户管理-->岗位管理");
    connect(newpost, &postScore::rejected, this, &MainWindow::DialogClosed);//关闭窗口后更新状态栏
}

void MainWindow::DialogClosed()
{
     statusBar()->showMessage("当前状态：用户管理");
}

//8.切换账号
void MainWindow::on_accountNo_clicked()
{
    close();
    //重新运行主程序
    QProcess::startDetached(QCoreApplication::applicationFilePath());
}

//年度履历信息导入及表格初始化
void MainWindow::yearInitialize()
{
    QString dateString = ui->dateEdit->text();
    int date = dateString.toInt();

    //设置行列
    ui->yearInfoTable->setRowCount(5);
    ui->yearInfoTable->setColumnCount(2);
    //更改表格表头
    QStringList strlist2;
    strlist2<<"信息类型"<<"信息内容";
    ui->yearInfoTable->setHorizontalHeaderLabels(strlist2);
    QStringList strlist;
    strlist<<"加工零件总数(个)"<<"总合格零件数(个)"<<"使用过设备"<<"有效工作时长(h)"<<"理论工作时长(h)";
    for(int i=0;i<5;i++)
    {
        QTableWidgetItem *item=new QTableWidgetItem(strlist.at(i));
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter); // 居中对齐
        ui->yearInfoTable->setItem(i,0,item);
    }
    ui->yearInfoTable->horizontalHeader()->setFont(QFont("song", 12));
    ui->yearInfoTable->horizontalHeader()->setStyleSheet("QHeaderView::section{background:lightblue;}"); //skyblue设置表头背景色
    //所有单元格的字体  设置成一样
    ui->yearInfoTable->setFont(QFont("song", 10));
    //表格充满布局
    ui->yearInfoTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 禁用编辑功能
    ui->yearInfoTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //往年度履历填充信息
    QString num = nowaccount;
    QSqlQuery query;
    query.prepare("select useDevice,productNumber,quanitityNumber,produceTime from YearInfo where account = :account and YEAR(produceDate) = :date");
    query.bindValue(":account",num);
    query.bindValue(":date",date);
    if(query.exec() && query.next())
    {
        model->setQuery(query);
        int numRows = model->rowCount(); // 获取模型的行数
        int sum1 = 1; // 假设要对第二列（索引为1）进行求和
        int sum2 = 2;
        int sum3 = 3;
        double sum01 = 0.0;
        double sum02 = 0.0;
        double sum03 = 0.0;
        double sumTime = numRows * 8;
        ui->yearInfoTable->setItem(4,1,new QTableWidgetItem(QString::number(sumTime)));

        for (int row = 0; row < numRows; ++row)
        {
            QModelIndex index = model->index(row, sum1); // 获取指定列的索引
            double value = model->data(index).toDouble(); // 获取该单元格的值
            sum01 += value;
        }
        ui->yearInfoTable->setItem(0,1,new QTableWidgetItem(QString::number(sum01)));
        for (int row = 0; row < numRows; ++row)
        {
            QModelIndex index = model->index(row, sum2); // 获取指定列的索引
            double value = model->data(index).toDouble(); // 获取该单元格的值
            sum02 += value;
        }
        ui->yearInfoTable->setItem(1,1,new QTableWidgetItem(QString::number(sum02)));
        for (int row = 0; row < numRows; ++row)
        {
            QModelIndex index = model->index(row, sum3); // 获取指定列的索引
            double value = model->data(index).toDouble(); // 获取该单元格的值
            sum03 += value;
        }
        ui->yearInfoTable->setItem(3,1,new QTableWidgetItem(QString::number(sum03)));

        int device = 0; // 假设要筛选第一列（索引为0）
        QStringList uniqueValues; // 用于存储相同的信息

        // 遍历第一列，找出相同的信息
        for (int row = 0; row < numRows; ++row)
        {
            QModelIndex index = model->index(row, device); // 获取指定列的索引
            QString value = model->data(index).toString(); // 获取该单元格的值

            // 如果相同的信息尚未添加到 uniqueValues，则添加它
            if (!uniqueValues.contains(value))
            {
                uniqueValues << value;
            }
        }
        // 创建一个字符串，将 uniqueValues 中的信息连接起来，用逗号分隔
        QString joinedValues = uniqueValues.join(", ");
        // 将字符串添加到 QTableWidget 的第三行第二列
        ui->yearInfoTable->setItem(2, 1, new QTableWidgetItem(joinedValues));
    }
    else {
        qDebug()<<"reason is = "<<query.lastError().text();
    }
}

void MainWindow::ThankText()
{
    QString num = nowaccount;
    QSqlQuery query;
    QSqlQuery query2;
    //感谢部分
    QDate year1 = ui->dateEdit->date();
//    int year2 = year1.year();
    QString year3 = QString::number(year1.year());
    int year4 = year3.toInt();

    QAbstractItemModel *table = ui->yearInfoTable->model();
    // 获取当前表格的内容
    QModelIndex index = table->index(0, 1);
    QModelIndex index1 = table->index(1, 1);
    QModelIndex index2 = table->index(2, 1);
    QModelIndex index3 = table->index(3, 1);
    QModelIndex index4 = table->index(4, 1);
    int all0 = table->data(index).toInt();
    int quanitity0 = table->data(index1).toInt();
    double scale0 = static_cast<double>(quanitity0) / all0 * 100;
    QString scale = QString::number(scale0);
    QString all = table->data(index).toString();
    QString quanitity = table->data(index1).toString();
    QString device = table->data(index2).toString();
    QString time1 = table->data(index3).toString();
    QString time2 = table->data(index4).toString();

    query.prepare("select name,sex from UserInfo where account = :account");
    query.bindValue(":account",num);
    if(query.exec() && query.first())
    {
        QString name = query.value(0).toString();
        QString sex = query.value(1).toString();
        if(sex == "女")
        {
            query.prepare("select count(*) from punishInfo where account = :account and type = '奖励申请' and status = '已批准' and Year(reviewTime) = :year4");
            query2.prepare("select count(*) from punishInfo where account = :account and type = '检举' and status = '已批准' and Year(reviewTime) = :year4");
            query.bindValue(":account",num);
            query.bindValue(":year4",year4);
            query2.bindValue(":account",num);
            query2.bindValue(":year4",year4);
            if(query.exec() && query2.exec())
            {
                query.first();
                query2.first();
                QString times01 = query.value(0).toString();
                QString times02 = query2.value(0).toString();
                QString sex2 = "女士";
                QString textTemplate = "        {{姓名}}{{性别}}，您{{年份}}年度共使用过{{设备}}设备加工了{{总零件}}个零件，其中合格零件数为{{合格数}}，"
                                       "合格率达到{{合格率}}%，您的有效工作时长为{{时长}}小时。\n        您本年度共被奖励{{奖励次数}}次，因违规被处罚{{检举次数}}次。感谢您为公司所做的贡献，祝您生活愉快！";
                textTemplate.replace("{{姓名}}", name);
                textTemplate.replace("{{性别}}", sex2);
                textTemplate.replace("{{年份}}", year3);
                textTemplate.replace("{{设备}}", device);
                textTemplate.replace("{{总零件}}", all);
                textTemplate.replace("{{合格数}}", quanitity);
                textTemplate.replace("{{合格率}}", scale);
                textTemplate.replace("{{时长}}", time1);
                textTemplate.replace("{{奖励次数}}", times01);
                textTemplate.replace("{{检举次数}}", times02);
                ui->textEdit->setPlainText(textTemplate);
            }
        }
        else
        {
            query.prepare("select count(*) from punishInfo where account = :account and type = '奖励申请' and status = '已批准' and Year(reviewTime) = :year4");
            query2.prepare("select count(*) from punishInfo where account = :account and type = '检举' and status = '已批准' and Year(reviewTime) = :year4");
            query.bindValue(":account",num);
            query.bindValue(":year4",year4);
            query2.bindValue(":account",num);
            query2.bindValue(":year4",year4);
            if(query.exec() && query2.exec())
            {
                query.first();
                query2.first();
                QString times01 = query.value(0).toString();
                QString times02 = query2.value(0).toString();
                QString sex2 = "先生";
                QString textTemplate = "        {{姓名}}{{性别}}，您{{年份}}年度共使用过{{设备}}设备加工了{{总零件}}个零件，其中合格零件数为{{合格数}}，"
                                       "合格率达到{{合格率}}%，您的有效工作时长为{{时长}}小时。\n        您本年度共被奖励{{奖励次数}}次，因违规被处罚{{检举次数}}次。感谢您为公司所做的贡献，祝您生活愉快！";
                textTemplate.replace("{{姓名}}", name);
                textTemplate.replace("{{性别}}", sex2);
                textTemplate.replace("{{年份}}", year3);
                textTemplate.replace("{{设备}}", device);
                textTemplate.replace("{{总零件}}", all);
                textTemplate.replace("{{合格数}}", quanitity);
                textTemplate.replace("{{合格率}}", scale);
                textTemplate.replace("{{时长}}", time1);
                textTemplate.replace("{{奖励次数}}", times01);
                textTemplate.replace("{{检举次数}}", times02);
                ui->textEdit->setPlainText(textTemplate);
            }
        }
    }
}

//用户管理信息
void MainWindow::staffshow()
{
    //用户信息显示
    QString test = "SELECT * FROM UserInfo";
    model->setQuery(test,db);

    //更改表头
    QStringList info;
    info<<"姓名"<<"性别"<<"工号"<<"出生日期"<<"家庭住址"<<"手机"<<"电话"<<"状态"<<"入职年份"<<"班组"<<"技能"<<"岗位"<<"密码"<<"权限类型";
    for (int column = 0; column < model->columnCount(); ++column)
    {
        if (column < info.size())
        {
            model->setHeaderData(column, Qt::Horizontal, info.at(column));
        }
    }
    ui->tableView->viewport()->update();//刷新试图

    QFont headerFont("song", 12, QFont::Bold);//表头字体修改
    ui->tableView->horizontalHeader()->setFont(headerFont);
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);//表头对齐设置
    ui->tableView->horizontalHeader()->setStyleSheet("background-color: lightgray; color: black;");//表头背景色
    QString headerStyle = "QHeaderView::section { background-color: lightblue; }";
    ui->tableView->horizontalHeader()->setStyleSheet(headerStyle);

    ui->tableView->setModel(model);
    //  ui->tableView->horizontalHeader()->setDefaultSectionSize(100); // 适应列宽
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);    //让表格和窗口相等
}

//表格初始化
void MainWindow::pItable()
{
    //设置行列
    ui->tableWidget->setRowCount(11);
    ui->tableWidget->setColumnCount(2);

    //更改表格表头
    QStringList strlist2;
    strlist2<<"信息类型"<<"信息内容";
    ui->tableWidget->setHorizontalHeaderLabels(strlist2);

    QStringList strlist;
    strlist<<"姓   名"<<"性   别"<<"工   号"<<"出生日期"<<"家庭住址"<<"手   机"<<"电   话"<<"状   态"<<"入职年份"<<"班   组"<<"技   能";

    for(int i=0;i<11;i++)
    {
        QTableWidgetItem *item=new QTableWidgetItem(strlist.at(i));
//      item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);//左对齐
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter); // 居中对齐
        ui->tableWidget->setItem(i,0,item);
    }

    //ui->tableWidget->horizontalHeader()->setStretchLastSection(true); //行头自适应表格
    ui->tableWidget->horizontalHeader()->setFont(QFont("song", 12));
    ui->tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:lightblue;}"); //skyblue设置表头背景色

    //所有单元格的字体  设置成一样
    ui->tableWidget->setFont(QFont("song", 10));

    //表格充满布局
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //设置不可编辑
    int columnToMakeReadOnly = 0; // 第二列的索引（索引从0开始）
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row)
    {
        QTableWidgetItem *item = ui->tableWidget->item(row, columnToMakeReadOnly);
        if (item)
        {
            item->setFlags(item->flags() & ~Qt::ItemIsEditable); // 移除可编辑标志
        }
    }
}

//删除用户信息
void MainWindow::deleteuser()
{
    //实现tableview的单行选中功能
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    //实现删除功能
    QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedIndexes();
    if (selectedIndexes.isEmpty())
    {
        // 没有选中任何行
        QMessageBox::warning(this, tr("Tips"), tr("你未选中任何信息！"));
        return;
    }

    int rowToDelete = selectedIndexes.first().row();
    // 获取你需要删除的行的唯一标识，如主键值
    QVariant primaryKeyValue = ui->tableView->model()->data(ui->tableView->model()->index(rowToDelete, 2));

    QSqlQuery query;
    QSqlQuery query2;
    query.prepare("DELETE FROM UserInfo WHERE account = :primaryKeyValue");
    query.bindValue(":primaryKeyValue", primaryKeyValue);
    query2.prepare("DELETE FROM Image WHERE account = :primaryKeyValue");
    query2.bindValue(":primaryKeyValue", primaryKeyValue);
    if (query.exec() && query2.exec())
    {
        // 删除成功
        ui->tableView->model()->removeRow(rowToDelete);
        QMessageBox::information(NULL,"提示","删掉咯~~");
    }
    else
    {
        // 删除失败
        QMessageBox::warning(NULL,"提示","失败啦！！！");
    }
}

//发送用户信息到修改界面
void MainWindow::sendshow()
{
    QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedIndexes();

    if (!selectedIndexes.isEmpty())
    {
            QModelIndex selectedIndex = selectedIndexes.at(0); // Assuming single selection
            int row = selectedIndex.row();

            QSqlQueryModel *model = qobject_cast<QSqlQueryModel*>(ui->tableView->model());

            if (model)
            {
                QVariant name = model->data(model->index(row, 0), Qt::DisplayRole); // Replace 'yourNameColumnIndex' with the actual column index
                QVariant sex = model->data(model->index(row, 1), Qt::DisplayRole); // Replace 'yourEmailColumnIndex' with the actual column index
                QVariant account = model->data(model->index(row, 2), Qt::DisplayRole);
                QVariant birth = model->data(model->index(row, 3), Qt::DisplayRole);
                QVariant address = model->data(model->index(row, 4), Qt::DisplayRole);
                QVariant phone = model->data(model->index(row, 5), Qt::DisplayRole);
                QVariant tel = model->data(model->index(row, 6), Qt::DisplayRole);
                QVariant status = model->data(model->index(row, 7), Qt::DisplayRole);
                QVariant startY = model->data(model->index(row, 8), Qt::DisplayRole);
                QVariant clas = model->data(model->index(row, 9), Qt::DisplayRole);
                QVariant post = model->data(model->index(row, 11), Qt::DisplayRole);
                QVariant password = model->data(model->index(row, 12), Qt::DisplayRole);
                QVariant type = model->data(model->index(row, 13), Qt::DisplayRole);

                // Create a ChangeUser dialog and pass the data
                changeUser *changeUserWindow = new changeUser(name.toString(), sex.toString(),account.toString(),password.toString(),birth.toDate(),
                                                              post.toString(),clas.toString(),startY.toDate(),address.toString(),phone.toString(),tel.toString(),
                                                              status.toString(),type.toString(),this);
                int result = changeUserWindow->exec();
                qDebug()<<"result:" <<result;
                if (result == QDialog::Accepted)
                {
                    qDebug()<<"result:" <<result;
                    // 用户点击了保存按钮，更新数据库模型以反映更改
                    QString setQuer = "SELECT * FROM UserInfo";
                    model->setQuery(setQuer,db);
                 //   model->setQuery("SELECT * FROM UserInfo");
                }
            }
        }
    else
    {
            QMessageBox::warning(this, tr("提示"), tr("请选择想要修改的用户"));
    }
}

//新增用户后刷新
void MainWindow::addNewP()
{
    addnew *a = new addnew();
    a->setModal(false);
    int i = a->exec();
    qDebug()<<"i:" <<i;
    if (i == QDialog::Accepted)
    {
        qDebug()<<"result:" <<i;
        // 用户点击了保存按钮，更新数据库模型以反映更改
        QString setQuer = "SELECT * FROM UserInfo";
        model->setQuery(setQuer,db);
     //   model->setQuery("SELECT * FROM UserInfo");
    }
    else
    {
//        QMessageBox::warning(NULL,"提示","未执行任何操作");
    }
}

//tableView导出功能实现
//第一个参数是页面上的表格，第二个是导出文件的表头数据
void MainWindow::TableExcelout(QTableView *table,QString title)
{
    QString fileName = QFileDialog::getSaveFileName(table, "保存",QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),"Excel 文件(*.xls *.xlsx)");
    if (fileName!="")
    {
        QAxObject *excel = new QAxObject;
        if (excel->setControl("Excel.Application")) //连接Excel控件
        {
            excel->dynamicCall("SetVisible (bool Visible)","false");//不显示窗体
            excel->setProperty("DisplayAlerts", false);//不显示任何警告信息。如果为true那么在关闭是会出现类似“文件已修改，是否保存”的提示
            QAxObject *workbooks = excel->querySubObject("WorkBooks");//获取工作簿集合
            workbooks->dynamicCall("Add");//新建一个工作簿
            QAxObject *workbook = excel->querySubObject("ActiveWorkBook");//获取当前工作簿
            QAxObject *worksheet = workbook->querySubObject("Worksheets(int)", 1);

            //QTableView 获取列数
            int colount=model->columnCount();
            //QTableView 获取行数
            int rowcount=model->rowCount();

            QAxObject *cell,*col;
            //标题行
            cell=worksheet->querySubObject("Cells(int,int)", 1, 1);
            cell->dynamicCall("SetValue(const QString&)", title);
            cell->querySubObject("Font")->setProperty("Size", 18);
            //调整行高
            worksheet->querySubObject("Range(const QString&)", "1:1")->setProperty("RowHeight", 30);
            //合并标题行
            QString cellTitle;
            cellTitle.append("A1:");
            cellTitle.append(QChar(colount - 1 + 'A'));
            cellTitle.append(QString::number(1));
            QAxObject *range = worksheet->querySubObject("Range(const QString&)", cellTitle);
            range->setProperty("WrapText", true);
            range->setProperty("MergeCells", true);
            range->setProperty("HorizontalAlignment", -4108);//xlCenter
            range->setProperty("VerticalAlignment", -4108);//xlCenter
            //列标题
            for(int i=0;i<colount;i++)
            {
                QString columnName;
                columnName.append(QChar(i  + 'A'));
                columnName.append(":");
                columnName.append(QChar(i + 'A'));
                col = worksheet->querySubObject("Columns(const QString&)", columnName);
                col->setProperty("ColumnWidth", table->columnWidth(i)/6);
                cell=worksheet->querySubObject("Cells(int,int)", 2, i+1);
                //QTableWidget 获取表格头部文字信息
                //columnName=table->horizontalHeaderItem(i)->text();
                //QTableView 获取表格头部文字信息
                columnName=model->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString();
                cell->dynamicCall("SetValue(const QString&)", columnName);
                cell->querySubObject("Font")->setProperty("Bold", true);
                cell->querySubObject("Interior")->setProperty("Color",QColor(191, 191, 191));
                cell->setProperty("HorizontalAlignment", -4108);//xlCenter
                cell->setProperty("VerticalAlignment", -4108);//xlCenter
            }

            //数据区
            //QTableView 获取表格数据部分
            for(int i=0;i<rowcount;i++) //行数
            {
                for (int j=0;j<colount;j++)   //列数
                {
                    QModelIndex index = model->index(i, j);
                    QString strdata=model->data(index).toString();
                    worksheet->querySubObject("Cells(int,int)", i+3, j+1)->dynamicCall("SetValue(const QString&)", strdata);
                }
            }

            //画框线
            QString lrange;
            lrange.append("A2:");
            lrange.append(colount - 1 + 'A');
            lrange.append(QString::number(model->rowCount() + 2));
            range = worksheet->querySubObject("Range(const QString&)", lrange);
            range->querySubObject("Borders")->setProperty("LineStyle", QString::number(1));
            range->querySubObject("Borders")->setProperty("Color", QColor(0, 0, 0));
            //调整数据区行高
            QString rowsName;
            rowsName.append("2:");
            rowsName.append(QString::number(model->rowCount() + 2));
            range = worksheet->querySubObject("Range(const QString&)", rowsName);
            range->setProperty("RowHeight", 20);
            workbook->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(fileName));//保存至fileName
            workbook->dynamicCall("Close()");//关闭工作簿
            excel->dynamicCall("Quit()");//关闭excel
            delete excel;
            excel=NULL;
            if (QMessageBox::question(NULL,"完成","文件已经导出，是否现在打开？",QMessageBox::Yes|QMessageBox::No)==QMessageBox::Yes)
            {
                QDesktopServices::openUrl(QUrl("file:///" + QDir::toNativeSeparators(fileName)));
            }
        }
        else
        {
            QMessageBox::warning(NULL,"错误","未能创建 Excel 对象，请安装 Microsoft Excel。",QMessageBox::Apply);
        }
    }
}

//接收登录界面发送的正确账号密码
void MainWindow::getlogin(const QString &account)
{
    nowaccount = account;
    imageshow();//根据账号显示头像
    qDebug()<<"accountID = "<<nowaccount;
    QMap<QString, int> userPermissions = checkPruview(nowaccount); // 获取用户权限数据
    showButton(userPermissions); // 设置按钮的显示状态

    // 在这里使用用户账号进行数据库查询或其他操作
    QSqlQuery query;
    query.prepare("SELECT * FROM UserInfo WHERE account = :account");
    query.bindValue(":account", account);

    if (query.exec() && query.next())
    {
        // 查询成功，获取与用户账号相关的信息
        QString username = query.value("name").toString();
        QString userphone = query.value("phone").toString();
        QString usersex = query.value("sex").toString();
        QString useraccount = query.value("account").toString();
        QString userbirth = query.value("birth").toString();
        QString useraddress = query.value("address").toString();
        QString usertel = query.value("tel").toString();
        QString userstatus = query.value("status").toString();
        QString useryear = query.value("startY").toString();
        QString userclass = query.value("class").toString();
        QString userability = query.value("ability").toString();
        // 使用获取到的信息进行操作，例如在界面上显示用户信息
        pItable();
        ui->tableWidget->setItem(0, 1, new QTableWidgetItem(username));
        ui->tableWidget->setItem(1, 1, new QTableWidgetItem(usersex));
        ui->tableWidget->setItem(2, 1, new QTableWidgetItem(useraccount));
        ui->tableWidget->setItem(3, 1, new QTableWidgetItem(userbirth));
        ui->tableWidget->setItem(4, 1, new QTableWidgetItem(useraddress));
        ui->tableWidget->setItem(5, 1, new QTableWidgetItem(userphone));
        ui->tableWidget->setItem(6, 1, new QTableWidgetItem(usertel));
        ui->tableWidget->setItem(7, 1, new QTableWidgetItem(userstatus));
        ui->tableWidget->setItem(8, 1, new QTableWidgetItem(useryear));
        ui->tableWidget->setItem(9, 1, new QTableWidgetItem(userclass));
        ui->tableWidget->setItem(10, 1, new QTableWidgetItem(userability));

        //所有单元格的字体  设置成一样
        ui->tableWidget->setFont(QFont("song", 12));

        ui->uesrName->setText("用户：" + username);//主界面显示用户名

        //个人信息不可修改部分设置只读
        for(int i=0;i<3;i++)
        {
            QTableWidgetItem *item = ui->tableWidget->item(i, 1);
            if (item)
            {
                item->setFlags(item->flags() & ~Qt::ItemIsEditable); // 移除可编辑标志
            }
        }
        for(int i=7;i<10;i++)
        {
            QTableWidgetItem *item2 = ui->tableWidget->item(i, 1);
            if (item2)
            {
                item2->setFlags(item2->flags() & ~Qt::ItemIsEditable); // 移除可编辑标志
            }
        }
    }
    else
    {
        // 查询失败，处理错误或显示错误消息
        QMessageBox::warning(NULL,"错误","查无该信息");
        qDebug() << "Error querying user information:" << query.lastError().text();
    }
}

//修改个人信息同步到数据库
void MainWindow::personInfoModify()
{
    QString account = nowaccount;
    QTableWidgetItem *item = ui->tableWidget->item(3, 1);
    QTableWidgetItem *item2 = ui->tableWidget->item(4, 1);
    QTableWidgetItem *item3 = ui->tableWidget->item(5, 1);
    QTableWidgetItem *item4 = ui->tableWidget->item(6, 1);
    QTableWidgetItem *item5 = ui->tableWidget->item(10, 1);

    QString birth = item->text();;
    QDate date = QDate::fromString(birth, "yyyy-MM-dd");

    QString address = item2->text();
    QString phone = item3->text();
    QString tel = item4->text();
    QString ability = item5->text();

    QSqlQuery query;
    query.prepare("select birth,address,phone,tel,ability from UserInfo where account = :account");
    query.bindValue(":account",account);
    if(query.exec() && query.next())
    {
        QDate originalDate = query.value(0).toDate();
        QString originalAddress = query.value(1).toString();
        QString originalPhone = query.value(2).toString();
        QString originalTel = query.value(3).toString();
        QString originalAbility = query.value(4).toString();
        if (date != originalDate || address != originalAddress || phone != originalPhone || tel != originalTel || ability != originalAbility)
        {
            // 更新数据库中的记录
            QSqlQuery query;
            query.prepare("UPDATE UserInfo SET birth = :birth, address = :address, phone = :phone, tel = :tel, ability = :ability WHERE account = :account");
            query.bindValue(":birth", date);
            query.bindValue(":address", address);
            query.bindValue(":phone", phone);
            query.bindValue(":tel", tel);
            query.bindValue(":ability", ability);
            query.bindValue(":account", account);
            if (!query.exec())
            {
                QMessageBox::warning(NULL,"Errot","失败了，没更新");
                // 处理更新失败的情况
            }
            else
            {
                QMessageBox::information(NULL,"提示","修改成功");
            }
        }
    }
}

//退出程序
void MainWindow::tuichu()
{
    QCoreApplication::quit();
}

//关于此软件
void MainWindow::aboutThis()
{
//    // 创建一个小窗口以显示关于软件的信息
//    QMessageBox::about(this, "关于软件", "软件名称：员工行为规范考核系统\n版本号：1.0\n作者：Mes课题组\n发布日期：2023年9月");
    QString aboutText = "<html>"
                           "<h2 style='text-align: center;'>关于软件</h2>"
                           "<hr>"
                           "<p><strong>软件名称：</strong> 员工行为规范考核系统</p>"
                           "<p><strong>版本号：</strong> 1.0</p>"
                           "<p><strong>作者：</strong> MES课题组</p>"
                           "<p><strong>发布日期：</strong> 2023-09-01</p>"
                           "<hr>"
                           "<p style='text-align: center;'>完整版正在开发中，敬请稍后~</p>"
                           "</html>";

        // 创建一个小窗口以显示HTML格式的关于信息
        QMessageBox msgBox(QMessageBox::Information, "关于软件", aboutText, QMessageBox::Ok);
        msgBox.setTextFormat(Qt::RichText);
        msgBox.exec();
}

//更改个人密码
void MainWindow::modifyP()
{
    // 获取输入框中的旧密码、新密码和确认新密码
        QString oldP = ui->oldPassword->text();
        QString newP = ui->newPassword->text();
        QString newP2 = ui->newPassword2->text();

        // 检查旧密码是否与数据库中存储的密码匹配（假设您已经获取了当前用户的账号）
        QString currentUserAccount = nowaccount; // 替换为实际获取账号的方法
        QSqlQuery query;
        query.prepare("SELECT password FROM UserInfo WHERE account = :account");
        query.bindValue(":account", currentUserAccount);
        if(oldP.isEmpty())
        {
            return;
        }
        else if (query.exec() && query.next())
        {
            QString storedPassword = query.value(0).toString();
            if (newP.isEmpty())
            {
                // 新密码为空，显示错误消息或者执行其他适当的操作
                QMessageBox::critical(this, tr("错误"), tr("新密码不得为空"));
                return;
            }
            else if(newP2.isEmpty())
            {
                QMessageBox::critical(this, tr("错误"), tr("新密码未确认"));
                return;
            }
            else if (oldP != storedPassword)
            {
                // 旧密码不匹配，显示错误消息
                QMessageBox::critical(this, "错误", "旧密码不正确");
                return;
            }
            else if (newP2 == storedPassword)
            {
                // 旧密码不匹配，显示错误消息
                QMessageBox::critical(this, "错误", "与旧密码相同，不可更改");
                return;
            }
        }
        else
        {
            // 查询失败或用户不存在，处理错误或显示错误消息
            QMessageBox::critical(this, "错误", "无法验证用户信息");
            return;
        }

        // 检查新密码与确认新密码是否匹配
        if (newP != newP2)
        {
            // 新密码不匹配，显示错误消息
            QMessageBox::critical(this, "错误", "新密码与确认新密码不匹配");
            return;
        }

        // 执行数据库更新操作来修改密码
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE UserInfo SET password = :newPassword WHERE account = :account");
        updateQuery.bindValue(":newPassword", newP2);
        updateQuery.bindValue(":account", currentUserAccount);
        if (updateQuery.exec())
        {
            QMessageBox::information(this, "成功", "密码修改成功");
            ui->oldPassword->clear();
            ui->newPassword->clear();
            ui->newPassword2->clear();
        }
        else
        {
            // 更新失败，处理错误或显示错误消息
            QMessageBox::critical(this, "错误", "无法修改密码");
        }
}

//奖惩审核信息导入
void MainWindow::punInfo()
{
    //用户信息显示
    QString test1 = "SELECT * FROM punishInfo";
    model->setQuery(test1,db);

    //更改表头
    QStringList info1;
    info1<<"序号"<<"事件类型"<<"工号"<<"上传时间"<<"地点"<<"理由"<<"管理条例号"<<"证明材料"<<"被执行人"<<"执行人"<<"备注"<<"得分"<<"审批状态"<<"审核人"<<"审批时间";
    for (int column = 0; column < model->columnCount(); ++column)
    {
        if (column < info1.size())
        {
            model->setHeaderData(column, Qt::Horizontal, info1.at(column));
        }
    }
    ui->tableView_7->viewport()->update();//刷新试图

    QFont headerFont("song", 12, QFont::Bold);//表头字体修改
    ui->tableView_7->horizontalHeader()->setFont(headerFont);
    ui->tableView_7->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);//表头对齐设置
    ui->tableView_7->horizontalHeader()->setStyleSheet("background-color: lightgray; color: black;");//表头背景色
    QString headerStyle = "QHeaderView::section { background-color: lightblue; }";
    ui->tableView_7->horizontalHeader()->setStyleSheet(headerStyle);

    ui->tableView_7->setModel(model);
    ui->tableView_7->horizontalHeader()->hideSection(15);//隐藏删除标记列
    ui->tableView_7->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);    //让表格和窗口相等
}

//上传奖励申请至数据库
void MainWindow::submitRewardInfo()
{
    QString Now = nowaccount;
    QString executed;
    QSqlQuery query;
    query.prepare("SELECT name FROM UserInfo WHERE account = :account");
    query.bindValue(":account", Now);
    if (query.exec() && query.next())
    {
            executed = query.value(0).toString(); // 获取申请者的姓名
    }
    else
    {
        QMessageBox::warning(NULL, "提示", "数据库查询错误！");
    }

    QString path = ui->rewardProve->text();//获取路径准备上传头像
    QFile imageFile(path);
    if (!imageFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open image file.";
        return;
    }
    else
    {
        QByteArray imageData = imageFile.readAll();
        if(imageData.size() <= 1000*1024)
        {
            QString reason = ui->rewardReason->currentText();
            //    QString Date = ui->rewardTime->dateTime().toString("yyyy-MM-dd hh:mm:ss");
            QString Date = ui->rewardTime->dateTime().toString("yyyy-MM-dd ");
            QString note = ui->rewardNote->toPlainText();//获取备注说明
            QString type = "奖励申请";
            QString place = "线上";

            query.prepare("select manageID from ManageRule where name = :name");
            query.bindValue(":name", reason);
            if(query.exec() && query.first())
            {
                int id = query.value(0).toInt(); // 获取 manageID
                // 插入数据
                QSqlQuery insertQuery;
                insertQuery.prepare("INSERT INTO punishInfo (reason, prove, time, executed, type, account, note, manageID, place) "
                                    "VALUES (:reason, :prove, :date, :executed, :type, :account, :note, :id, :place)");
                insertQuery.bindValue(":reason", reason);
                insertQuery.bindValue(":prove", imageData);
                insertQuery.bindValue(":date", Date);
                insertQuery.bindValue(":executed", executed);
                insertQuery.bindValue(":type", type);
                insertQuery.bindValue(":account", Now);
                insertQuery.bindValue(":note", note);
                insertQuery.bindValue(":id", id);
                insertQuery.bindValue(":place", place);

                if (insertQuery.exec())
                {
                    QMessageBox::information(NULL,"提示","奖励申请已经提交~~");
                }
                else
                {
                    QMessageBox::warning(NULL,"提示","提交失败！！！");
                    qDebug() << insertQuery.lastError().text();
                }
            }
            else
            {
                QMessageBox::warning(NULL,"提示","图片过大，请选择小于1MB图片");
            }
        }
    }
}

//上传检举内容至数据库
void MainWindow::submitReportInfo()
{
    QString accountNow = nowaccount;
    QSqlQuery query;
    QString execute;
    query.prepare("SELECT name FROM UserInfo WHERE account = :account");
    query.bindValue(":account", accountNow);
    if (query.exec() && query.next())
    {
        execute = query.value("name").toString();
        //       QMessageBox::information(NULL, "提示", "检举成功！");
    }
    else
    {
        QMessageBox::warning(NULL, "提示", "数据库查询错误！");
    }
    QString path = ui->punishProve->text();//获取路径准备上传头像
    QFile imageFile(path);
    if (!imageFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open image file.";
        return;
    }
    else
    {
        QByteArray imageData = imageFile.readAll();
        QString reason = ui->punishReason->currentText();//获取理由，准备捆绑条例号
        QString place = ui->punishWhere->text();
        QString executed = ui->punishWho->text();
        //    QString Date = ui->punishTime->dateTime().toString("yyyy-MM-dd hh:mm:ss");
        QString Date = ui->punishTime->dateTime().toString("yyyy-MM-dd ");
        QString type = "检举";
        QString note = ui->reportNote->toPlainText();//获取备注说明
        QString account = ui->punishAccount->text();

        query.prepare("select manageID from ManageRule where name = :name");
        query.bindValue(":name", reason);
        if(query.exec())
        {
            if(query.first())
            {
                int id = query.value(0).toInt(); // 获取 manageID
                // 插入数据
                QSqlQuery insertQuery;
                if(ui->reporterChoose->checkState())
                {
                    int i = 1;
                    insertQuery.prepare("INSERT INTO punishInfo (reason, prove,place, executed, execute, time, type, note, account, manageID, reporter) "
                                        "VALUES (:reason, :prove, :place, :executed, :execute, :date, :type, :note, :account, :manageID, :reporter)");
                    insertQuery.bindValue(":reason", reason);
                    insertQuery.bindValue(":prove", imageData);
                    insertQuery.bindValue(":place", place);
                    insertQuery.bindValue(":executed", executed);
                    insertQuery.bindValue(":execute", execute);
                    insertQuery.bindValue(":date", Date);
                    insertQuery.bindValue(":type", type);
                    insertQuery.bindValue(":note", note);
                    insertQuery.bindValue(":account", account);
                    insertQuery.bindValue(":manageID", id);
                    insertQuery.bindValue(":reporter", i);

                    if (insertQuery.exec())
                    {
                        QMessageBox::information(NULL,"提示","检举成功！");
                    }
                    else
                    {
                        QMessageBox::warning(NULL,"提示",query.lastError().text());
                    }
                }
                else
                {
                    int i = 0;
                    insertQuery.prepare("INSERT INTO punishInfo (reason, prove,place, executed, execute, time, type, note, account, manageID, reporter) "
                                        "VALUES (:reason, :prove, :place, :executed, :execute, :date, :type, :note, :account, :manageID, :reporter)");
                    insertQuery.bindValue(":reason", reason);
                    insertQuery.bindValue(":prove", imageData);
                    insertQuery.bindValue(":place", place);
                    insertQuery.bindValue(":executed", executed);
                    insertQuery.bindValue(":execute", execute);
                    insertQuery.bindValue(":date", Date);
                    insertQuery.bindValue(":type", type);
                    insertQuery.bindValue(":note", note);
                    insertQuery.bindValue(":account", account);
                    insertQuery.bindValue(":manageID", id);
                    insertQuery.bindValue(":reporter", i);

                    if (insertQuery.exec())
                    {
                        QMessageBox::information(NULL,"提示","检举成功！");
                    }
                    else
                    {
                        QMessageBox::warning(NULL,"提示",query.lastError().text());
                    }
                }

            }
            else
            {
                QMessageBox::warning(NULL,"提示","query.first()");
            }
        }
        else
        {
            QMessageBox::warning(NULL,"提示","nieyunxing");
        }
    }
}

//查看详情功能实现
void MainWindow::checkInfo()
{
    QModelIndexList selectedIndexes = ui->tableView_7->selectionModel()->selectedIndexes();

    if (!selectedIndexes.isEmpty())
    {
        QModelIndex selectedIndex = selectedIndexes.at(0); // Assuming single selection
        int row = selectedIndex.row();

        QSqlQueryModel *model = qobject_cast<QSqlQueryModel*>(ui->tableView_7->model());

        if (model)
        {
            QVariant id = model->data(model->index(row,0), Qt::DisplayRole);
            QVariant time = model->data(model->index(row, 3), Qt::DisplayRole);
            QVariant place = model->data(model->index(row, 4), Qt::DisplayRole);
            QVariant executed = model->data(model->index(row, 8), Qt::DisplayRole);
            QVariant execute = model->data(model->index(row, 9), Qt::DisplayRole);
            QVariant type = model->data(model->index(row, 1), Qt::DisplayRole);
            QVariant reason = model->data(model->index(row, 5), Qt::DisplayRole);
            QVariant proof = model->data(model->index(row, 7), Qt::DisplayRole);
            QVariant note = model->data(model->index(row, 10), Qt::DisplayRole);
            QVariant score = model->data(model->index(row, 11), Qt::DisplayRole);
            QVariant status = model->data(model->index(row, 12), Qt::DisplayRole);
            QVariant reviewer = model->data(model->index(row, 13), Qt::DisplayRole);
            QVariant reviewTime = model->data(model->index(row, 14), Qt::DisplayRole);
            QVariant reporter = model->data(model->index(row,15),Qt::DisplayRole);
            int i = reporter.toInt();
            //创建细节界面
            detial * newDetial = new detial(id.toInt(), time.toDate(), place.toString(),executed.toString(),execute.toString(),type.toString(),reason.toString(),proof.toByteArray(),
                                            note.toString(),score.toInt(),status.toString(),reviewer.toString(),reviewTime.toDate(), i ,this);
            connect(newDetial,&detial::rejected,this,&MainWindow::detrefuse);
            int result = newDetial->exec();
            if (result == QDialog::Accepted)
            {
                detialBack();
                // 用户点击了保存按钮，更新数据库模型以反映更改
                QString setQuer = "SELECT * FROM punishInfo";
                model->setQuery(setQuer,db);
            }
        }
    }
    else
    {
        QMessageBox::warning(this, tr("提示"), tr("请选择想要查看的用户"));
    }
}

//查看详情的返回填充审核人信息
void MainWindow::detialBack()
{
    QString Now = nowaccount;
    QModelIndexList selectedIndexes = ui->tableView_7->selectionModel()->selectedIndexes();//选中当前行

    if (!selectedIndexes.isEmpty())
    {
        QModelIndex selectedIndex = selectedIndexes.at(0); // Assuming single selection
        int row = selectedIndex.row();// 获取选中行的索引

        QString status = "已批准";
        QString reviewer;
        QSqlQuery query;
        //锁定审核时间
        QDateTime currentDateTime = QDateTime::currentDateTime();
        QString Date = currentDateTime.toString("yyyy-MM-dd ");

        //锁定审核长账号
        query.prepare("SELECT name FROM UserInfo WHERE account = :account");
        query.bindValue(":account", Now);
        if (query.exec() && query.next())
        {
            reviewer = query.value(0).toString(); // 获取申请者的姓名
            //锁定分数和事件序号
            QModelIndex index1 = model->index(row, 0);
            // 获取该单元格的数据
            QVariant num = model->data(index1);
            QSqlQuery insertQuery;

            insertQuery.prepare("UPDATE punishInfo set status = :status, reviewer = :reviewer, reviewTime = :reviewTime WHERE id = :id");
            insertQuery.bindValue(":status", status);
            insertQuery.bindValue(":reviewer", reviewer);
            insertQuery.bindValue(":reviewTime", Date);
            insertQuery.bindValue(":id", num);

            if (insertQuery.exec())
            {
                QMessageBox::information(NULL,"提示","已同意该申请！");
                QString setQuer = "SELECT * FROM punishInfo";
                model->setQuery(setQuer,db);
            }
            else
            {
                QMessageBox::warning(NULL,"提示","审核失败！");
            }
        }
    }
}

void MainWindow::detrefuse()
{
    refuse2();
}

//奖惩审核之同意申请
void MainWindow::aggree()
{
    QString Now = nowaccount;
    QModelIndexList selectedIndexes = ui->tableView_7->selectionModel()->selectedIndexes();//选中当前行

    if (!selectedIndexes.isEmpty())
    {
        QModelIndex selectedIndex = selectedIndexes.at(0); // Assuming single selection
        int row = selectedIndex.row();// 获取选中行的索引

        QString status = "已批准";
        QString reviewer;
        QSqlQuery query;
        //锁定审核时间
        QDateTime currentDateTime = QDateTime::currentDateTime();
        QString Date = currentDateTime.toString("yyyy-MM-dd ");

        //锁定审核长账号
        query.prepare("SELECT name FROM UserInfo WHERE account = :account");
        query.bindValue(":account", Now);
        if (query.exec() && query.next())
        {
            reviewer = query.value(0).toString(); // 获取申请者的姓名
            //锁定分数和事件序号
            QModelIndex index1 = model->index(row, 0);
            QModelIndex index = model->index(row, 5); //锁定具体原因，以此查分
            // 获取该单元格的数据
            QVariant data = model->data(index);
            QVariant num = model->data(index1);

            query.prepare("select score from ManageRule where name = :name");
            query.bindValue(":name",data);
            if(query.exec() && query.next())
            {
                int score = query.value(0).toInt();
                QSqlQuery insertQuery;

                insertQuery.prepare("UPDATE punishInfo set status = :status, reviewer = :reviewer, reviewTime = :reviewTime, score = :score WHERE id = :id");
                insertQuery.bindValue(":status", status);
                insertQuery.bindValue(":reviewer", reviewer);
                insertQuery.bindValue(":reviewTime", Date);
                insertQuery.bindValue(":score", score);
                insertQuery.bindValue(":id", num);

                if (insertQuery.exec())
                {
                    QMessageBox::information(NULL,"提示","已同意该申请！");
                    QString setQuer = "SELECT * FROM punishInfo";
                    model->setQuery(setQuer,db);
                }
                else
                {
                    QMessageBox::warning(NULL,"提示","审核失败！");
                }
            }
        }
    }
    else
    {
        // 没有选中行，显示错误消息
        QMessageBox::warning(this, "警告", "未选中任何申请！");
    }
}

void MainWindow::refuse2()
{
    QString Now = nowaccount;
    QModelIndexList selectedIndexes = ui->tableView_7->selectionModel()->selectedIndexes();//选中当前行

    if (!selectedIndexes.isEmpty())
    {
        QModelIndex selectedIndex = selectedIndexes.at(0); // Assuming single selection
        int row = selectedIndex.row();// 获取选中行的索引

        QString status = "驳回";
        QString executed;
        QSqlQuery query;
        //锁定审核时间
        QDateTime currentDateTime = QDateTime::currentDateTime();
        QString Date = currentDateTime.toString("yyyy-MM-dd ");
        //锁定分数和事件序号
        QModelIndex index = model->index(row, 0);
        QVariant num = model->data(index);

        //锁定审核长账号
        query.prepare("SELECT name FROM UserInfo WHERE account = :account");
        query.bindValue(":account", Now);
        if (query.exec() && query.next())
        {
            executed = query.value(0).toString(); // 获取申请者的姓名
            //分数为0
            int score = 0;
            QSqlQuery insertQuery;
            insertQuery.prepare("UPDATE punishInfo set status = :status, reviewer = :reviewer, reviewTime = :reviewTime, score = :score WHERE id = :id");
            insertQuery.bindValue(":status", status);
            insertQuery.bindValue(":reviewer", executed);
            insertQuery.bindValue(":reviewTime", Date);
            insertQuery.bindValue(":score", score);
            insertQuery.bindValue(":id", num);
            if (insertQuery.exec())
            {
                QMessageBox::information(NULL,"提示","已驳回该申请！");
                QString setQuer = "SELECT * FROM punishInfo";
                model->setQuery(setQuer,db);
            }
            else
            {
                QMessageBox::warning(NULL,"提示","审核失败！");
            }
        }
    }
}

//奖惩审核之    jujue拒绝申请
void MainWindow::refuse()
{
    QString Now = nowaccount;
    QModelIndexList selectedIndexes = ui->tableView_7->selectionModel()->selectedIndexes();//选中当前行

    if (!selectedIndexes.isEmpty())
    {
        QModelIndex selectedIndex = selectedIndexes.at(0); // Assuming single selection
        int row = selectedIndex.row();// 获取选中行的索引

        QString status = "驳回";
        QString executed;
        QSqlQuery query;
        //锁定审核时间
        QDateTime currentDateTime = QDateTime::currentDateTime();
        QString Date = currentDateTime.toString("yyyy-MM-dd ");
        //锁定分数和事件序号
        QModelIndex index = model->index(row, 0);
        QVariant num = model->data(index);

        //锁定审核长账号
        query.prepare("SELECT name FROM UserInfo WHERE account = :account");
        query.bindValue(":account", Now);
        if (query.exec() && query.next())
        {
            executed = query.value(0).toString(); // 获取申请者的姓名
            //分数为0
            int score = 0;
            QSqlQuery insertQuery;
            insertQuery.prepare("UPDATE punishInfo set status = :status, reviewer = :reviewer, reviewTime = :reviewTime, score = :score WHERE id = :id");
            insertQuery.bindValue(":status", status);
            insertQuery.bindValue(":reviewer", executed);
            insertQuery.bindValue(":reviewTime", Date);
            insertQuery.bindValue(":score", score);
            insertQuery.bindValue(":id", num);
            if (insertQuery.exec())
            {
                QMessageBox::information(NULL,"提示","已驳回该申请！");
                QString setQuer = "SELECT * FROM punishInfo";
                model->setQuery(setQuer,db);
            }
            else
            {
                QMessageBox::warning(NULL,"提示","审核失败！");
            }
        }
    }
    else
    {
        // 没有选中行，显示错误消息
        QMessageBox::warning(this, "警告", "未选中任何申请！");
    }
}

//tab3奖惩详情显示
void MainWindow::tab3TableShow()
{
    QStandardItemModel *model1 = new QStandardItemModel();
    QStringList info;
    info<<"事件序号"<<"审核时间"<<"原因"<<"加/减分";
    int numColumns = info.size(); // 设置模型的列数
    model1->setColumnCount(numColumns);
    for (int column = 0; column < numColumns; ++column)
    {
        if (column < info.size())
        {
            model1->setHeaderData(column, Qt::Horizontal, info.at(column));
        }
    }
    QString account = nowaccount;
    QSqlQuery query;
    query.prepare("select id,reviewTime,reason,score,status from punishInfo where account = :account");
    query.bindValue(":account",account);
    if(query.exec())
    {
        // 遍历结果集并将数据添加到模型
        while (query.next())
        {
            // 逐个获取列的数据
            int id = query.value(0).toInt();
            QString reason = query.value(2).toString();
            QDate time = query.value(1).toDate();
            QString time2 = time.toString("yyyy-MM-dd");
            int score = query.value(3).toInt();
            QString status = query.value(4).toString();

            if(status != NULL)
            {
                // 创建适当的项并将数据添加到项中
                QStandardItem *item1 = new QStandardItem(QString::number(id));
                QStandardItem *item2 = new QStandardItem(time2);
                QStandardItem *item3 = new QStandardItem(reason);
                QStandardItem *item4 = new QStandardItem(QString::number(score));

                // 将项添加到模型的适当行和列中
                model1->appendRow({item1, item2, item3, item4});
            }
        }
    }

    QFont headerFont("song", 12, QFont::Bold);//表头字体修改
    ui->tab3table->horizontalHeader()->setFont(headerFont);
    ui->tab3table->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);//表头对齐设置
    ui->tab3table->horizontalHeader()->setStyleSheet("background-color: lightgray; color: black;");//表头背景色
    QString headerStyle = "QHeaderView::section { background-color: lightblue; }";
    ui->tab3table->horizontalHeader()->setStyleSheet(headerStyle);

    ui->tab3table->setModel(model1);
    ui->tab3table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);    //让表格和窗口相等
}

//tab3积分显示及计算
void MainWindow::tab3count()
{
    QAbstractItemModel *table = ui->tab3table->model();
    int rowCount = table->rowCount();
    double min = 0.0; // 用于累加分数的变量
    double plus = 0.0;

    for (int row = 0; row < rowCount; ++row)
    {
        // 获取当前行的第四列数据（假设列索引为 3）
        QModelIndex index = table->index(row, 3);
        double score = table->data(index).toDouble();

        // 判断分数是否小于 0
        if (score < 0)
        {
            // 累加小于 0 的分数
            min += score;
        }
        else if (score > 0)
        {
            plus += score;
        }
    }
    ui->minPoint->setText(QString::number(min));
    ui->plusPoint->setText(QString::number(plus));

    QString num = nowaccount;
    QSqlQuery query;
    query.prepare("select post from UserInfo where account = :account");
    query.bindValue(":account",num);
    if(query.exec() && query.first())
    {
        QString post = query.value(0).toString();
        query.prepare("select score from PostScore where post = :post");
        query.bindValue(":post",post);
        if(query.exec() && query.first())
        {
            int score = query.value(0).toInt();
            ui->startPoint->setText(QString::number(score));
            int a = ui->minPoint->text().toInt();
            int b = ui->plusPoint->text().toInt();
            int Point = score + a +b;
            ui->endPoint->setText(QString::number(Point));
        }
    }
}

//奖惩审核查询功能实现
void MainWindow::checkit()
{
    QString text = ui->PunKeyType->currentText(); // 获取ComboBox中的选定列名
    QString variable2 = ui->PunKeyWord->text(); // 假设你有变量2的值
    QSqlQuery query;
    if(text == "序号")
    {
        int id = variable2.toInt();
        QString sqlQuery = "SELECT * FROM punishInfo WHERE id = ?";
        query.prepare(sqlQuery);
        query.addBindValue(id);
        if (query.exec())
        {
            model->setQuery(query);
            ui->tableView_7->setModel(model);
            ui->tableView_7->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
        }
    }
    else if(text == "事件类型")
    {
        QString sqlQuery = "SELECT * FROM punishInfo WHERE  type = ?";
        query.prepare(sqlQuery);
        query.addBindValue(variable2);
        if(query.exec())
        {
            model->setQuery(query);
            ui->tableView_7->setModel(model);
            ui->tableView_7->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
        }
    }
    else if(text == "工号")
    {
        QString sqlQuery = "SELECT * FROM punishInfo WHERE  account = ?";
        query.prepare(sqlQuery);
        query.addBindValue(variable2);
        if(query.exec())
        {
            model->setQuery(query);
            ui->tableView_7->setModel(model);
            ui->tableView_7->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
        }
        else {
            qDebug()<<"reason is : "<<query.lastError().text();

        }
    }
    else if(text == "上传时间")
    {
        QDate time = QDate::fromString(variable2, "yyyy-MM-dd");
        QString sqlQuery = "SELECT * FROM punishInfo WHERE  time = ?";
        query.prepare(sqlQuery);
        query.addBindValue(time.toString("yyyy-MM-dd"));
        if(query.exec())
        {
            model->setQuery(query);
            ui->tableView_7->setModel(model);
            ui->tableView_7->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
        }
    }
    else if(text == "地点")
    {
        QString sqlQuery = "SELECT * FROM punishInfo WHERE  place = ?";
        query.prepare(sqlQuery);
        query.addBindValue(variable2);
        if(query.exec())
        {
            model->setQuery(query);
            ui->tableView_7->setModel(model);
            ui->tableView_7->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
        }
    }
    else if(text == "理由")
    {
        QString sqlQuery = "SELECT * FROM punishInfo WHERE  reason = ?";
        query.prepare(sqlQuery);
        query.addBindValue(variable2);
        if(query.exec())
        {
            model->setQuery(query);
            ui->tableView_7->setModel(model);
            ui->tableView_7->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
        }
    }
    else if(text == "管理条例号")
    {
        int num = variable2.toInt();
        QString sqlQuery = "SELECT * FROM punishInfo WHERE  manageID = ?";
        query.prepare(sqlQuery);
        query.addBindValue(num);
        if(query.exec())
        {
            model->setQuery(query);
            ui->tableView_7->setModel(model);
            ui->tableView_7->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
        }
    }
    else if(text == "被执行人")
    {
        QString sqlQuery = "SELECT * FROM punishInfo WHERE  executed = ?";
        query.prepare(sqlQuery);
        query.addBindValue(variable2);
        if(query.exec())
        {
            model->setQuery(query);
            ui->tableView_7->setModel(model);
            ui->tableView_7->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
        }
    }
    else if(text == "执行人")
    {
        QString sqlQuery = "SELECT * FROM punishInfo WHERE  execute = ?";
        query.prepare(sqlQuery);
        query.addBindValue(variable2);
        if(query.exec())
        {
            model->setQuery(query);
            ui->tableView_7->setModel(model);
            ui->tableView_7->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
        }
    }
    else if(text == "得分")
    {
        int num = variable2.toInt();
        QString sqlQuery = "SELECT * FROM punishInfo WHERE  score = ?";
        query.prepare(sqlQuery);
        query.addBindValue(num);
        if(query.exec())
        {
            model->setQuery(query);
            ui->tableView_7->setModel(model);
            ui->tableView_7->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
        }
    }
    else if(text == "审批状态")
    {
        QString sqlQuery = "SELECT * FROM punishInfo WHERE  status = ?";
        query.prepare(sqlQuery);
        query.addBindValue(variable2);
        if(query.exec())
        {
            model->setQuery(query);
            ui->tableView_7->setModel(model);
            ui->tableView_7->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
        }
    }
    else if(text == "审核人")
    {
        QString sqlQuery = "SELECT * FROM punishInfo WHERE  reviewer = ?";
        query.prepare(sqlQuery);
        query.addBindValue(variable2);
        if(query.exec())
        {
            model->setQuery(query);
            ui->tableView_7->setModel(model);
            ui->tableView_7->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
        }
    }
    else if(text == "审批时间")
    {
        QDate time = QDate::fromString(variable2, "yyyy-MM-dd");
        QString sqlQuery = "SELECT * FROM punishInfo WHERE  time = ?";
        query.prepare(sqlQuery);
        query.addBindValue(time.toString("yyyy-MM-dd"));
        if(query.exec())
        {
            model->setQuery(query);
            ui->tableView_7->setModel(model);
            ui->tableView_7->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
        }
    }
}

//账号管理--查询功能实现
void MainWindow::checkit2()
{
    QString text = ui->keyType->currentText(); // 获取ComboBox中的选定列名
    QString variable2 = ui->keyWord->text(); // 假设你有变量2的值
    QSqlQuery query;
    if(text == "姓名")
    {
        QString sqlQuery = "SELECT * FROM UserInfo WHERE name = ?";
        query.prepare(sqlQuery);
        query.addBindValue(variable2);
        if (query.exec())
        {
            model->setQuery(query);
            ui->tableView_7->setModel(model);
            ui->tableView_7->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
        }
    }
    else if(text == "性别")
    {
        QString sqlQuery = "SELECT * FROM UserInfo WHERE  sex = ?";
        query.prepare(sqlQuery);
        query.addBindValue(variable2);
        if(query.exec())
        {
            model->setQuery(query);
            ui->tableView_7->setModel(model);
            ui->tableView_7->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
        }
    }
    else if(text == "工号")
    {
        QString sqlQuery = "SELECT * FROM UserInfo WHERE  account = ?";
        query.prepare(sqlQuery);
        query.addBindValue(variable2);
        if(query.exec())
        {
            model->setQuery(query);
            ui->tableView_7->setModel(model);
            ui->tableView_7->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
        }
        else {
            qDebug()<<"reason is : "<<query.lastError().text();

        }
    }
    else if(text == "出生日期")
    {
        QDate time = QDate::fromString(variable2, "yyyy-MM-dd");
        QString sqlQuery = "SELECT * FROM UserInfo WHERE  birth = ?";
        query.prepare(sqlQuery);
        query.addBindValue(time.toString("yyyy-MM-dd"));
        if(query.exec())
        {
            model->setQuery(query);
            ui->tableView_7->setModel(model);
            ui->tableView_7->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
        }
    }
    else if(text == "家庭住址")
    {
        QString sqlQuery = "SELECT * FROM UserInfo WHERE  address = ?";
        query.prepare(sqlQuery);
        query.addBindValue(variable2);
        if(query.exec())
        {
            model->setQuery(query);
            ui->tableView_7->setModel(model);
            ui->tableView_7->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
        }
    }
    else if(text == "手机")
    {
        QString sqlQuery = "SELECT * FROM UserInfo WHERE  phone = ?";
        query.prepare(sqlQuery);
        query.addBindValue(variable2);
        if(query.exec())
        {
            model->setQuery(query);
            ui->tableView_7->setModel(model);
            ui->tableView_7->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
        }
    }
    else if(text == "电话")
    {
        QString sqlQuery = "SELECT * FROM UserInfo WHERE  tel = ?";
        query.prepare(sqlQuery);
        query.addBindValue(variable2);
        if(query.exec())
        {
            model->setQuery(query);
            ui->tableView_7->setModel(model);
            ui->tableView_7->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
        }
    }
    else if(text == "状态")
    {
        QString sqlQuery = "SELECT * FROM UserInfo WHERE status = ?";
        query.prepare(sqlQuery);
        query.addBindValue(variable2);
        if(query.exec())
        {
            model->setQuery(query);
            ui->tableView_7->setModel(model);
            ui->tableView_7->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
        }
    }
    else if(text == "入职年份")
    {
        QDate time = QDate::fromString(variable2);
        QString sqlQuery = "SELECT * FROM UserInfo WHERE  startY = ?";
        query.prepare(sqlQuery);
        query.addBindValue(time.toString("yyyy-MM-dd"));
        if(query.exec())
        {
            model->setQuery(query);
            ui->tableView_7->setModel(model);
            ui->tableView_7->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
        }
    }
    else if(text == "班组")
    {
        QString sqlQuery = "SELECT * FROM UserInfo WHERE  class = ?";
        query.prepare(sqlQuery);
        query.addBindValue(variable2);
        if(query.exec())
        {
            model->setQuery(query);
            ui->tableView_7->setModel(model);
            ui->tableView_7->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
        }
    }
    else if(text == "岗位")
    {
        QString sqlQuery = "SELECT * FROM UserInfo WHERE  post = ?";
        query.prepare(sqlQuery);
        query.addBindValue(variable2);
        if(query.exec())
        {
            model->setQuery(query);
            ui->tableView_7->setModel(model);
            ui->tableView_7->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
        }
    }
    else if(text == "权限类型")
    {
        QString sqlQuery = "SELECT * FROM UserInfo WHERE  type = ?";
        query.prepare(sqlQuery);
        query.addBindValue(variable2);
        if(query.exec())
        {
            model->setQuery(query);
            ui->tableView_7->setModel(model);
            ui->tableView_7->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽自适应
        }
    }
}

void MainWindow::imageshow()
{
    QSqlQuery query;
    QString id = nowaccount;
    query.prepare("select image from Image where account = :id");
    query.bindValue(":id",id);
    if(query.exec() && query.first())
    {
        QByteArray imageData = query.value(0).toByteArray();
        if(!imageData.isEmpty())
        {
            QImage image;
            image.loadFromData(imageData);
            ui->image->setPixmap(QPixmap::fromImage(image));
            ui->image->setScaledContents(true);//让图片适应label
            ui->image->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        }
        else
        {
            QImage image;
            image.load(":/picture/2.jpg");
            ui->image->setPixmap(QPixmap::fromImage(image));
            ui->image->setScaledContents(true);//让图片适应label
            ui->image->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        }
    }
}

void MainWindow::connectaDetial(const QModelIndex& index)
{
    QAbstractItemModel *model2 = ui->tab3table->model();
    if (index.isValid() && index.column() == 0)
    {
        int row = index.row();
        int id = model2->data(model2->index(row, 0)).toInt(); // 假设第一列是您要获取的内容
        QSqlQuery query;
        query.prepare("select type,time,place,reason,prove,executed,execute,note,score,status,reviewer,reviewTime,reporter from punishInfo where id =:id");
        query.bindValue(":id", id);
        if(query.exec() && query.next())
        {
            QVariant type = query.value(0).toString();
            QVariant time = query.value(1).toDate();
            QVariant place = query.value(2).toString();
            QVariant reason = query.value(3).toString();
            QVariant prove = query.value(4).toByteArray();
            QVariant executed = query.value(5).toString();
            QVariant note = query.value(7).toString();
            QVariant score = query.value(8).toString();
            QVariant status = query.value(9).toString();
            QVariant reviewer = query.value(10).toString();
            QVariant reviewTime = query.value(11).toString();
            QVariant execute = query.value(6).toString();
            int i = query.value(12).toInt();

            // 创建新界面并传递获取的内容
            detial * newDetial = new detial(id, time.toDate(), place.toString(),executed.toString(),execute.toString(),type.toString(),reason.toString(),prove.toByteArray(),
                                            note.toString(),score.toInt(),status.toString(),reviewer.toString(),reviewTime.toDate(), i,this);
            newDetial->hideButton();
            newDetial->show();
        }
    }
}

//绘制饼状图
//void MainWindow::printPie()
//{
//    customplot = ui->widget_2;
//}

//登录验证权限，并显示对应的功能
QMap<QString, int> checkPruview(const QString& account)
{
    QMap<QString, int> userPermissions;//创建权限容器
    QSqlQuery query;
    query.prepare("select type from UserInfo where account = :account");
    query.bindValue(":account",account);
    if(query.exec() && query.first())
    {
        QString type = query.value(0).toString();
        query.prepare("select pushButton,PersonInfo,YearInfo,punishInfo,dutycbx,pushButton_2,"
                      "reward,report,audit,pushButton_3,staff,manageRule,purType_2,purType "
                      "from Purview where name = :type");
        query.bindValue(":type",type);
        if (query.exec())
        {
            if (query.next())
            {
                // 读取每个权限列的数值并存储在userPermissions中
                userPermissions["pushButton"] = query.value(0).toInt();
                userPermissions["PersonInfo"] = query.value(1).toInt();
                userPermissions["YearInfo"] = query.value(2).toInt();
                userPermissions["punishInfo"] = query.value(3).toInt();
                userPermissions["dutycbx"] = query.value(4).toInt();
                userPermissions["pushButton_2"] = query.value(5).toInt();
                userPermissions["reward"] = query.value(6).toInt();
                userPermissions["report"] = query.value(7).toInt();
                userPermissions["audit"] = query.value(8).toInt();
                userPermissions["pushButton_3"] = query.value(9).toInt();
                userPermissions["staff"] = query.value(10).toInt();
                userPermissions["manageRule"] = query.value(11).toInt();
                userPermissions["purType_2"] = query.value(12).toInt();
                userPermissions["purType"] = query.value(13).toInt();
//                qDebug() << "userPermissions['pushButton'] " << query.value(0).toInt();
            }
        }
        else
        {
            // 查询失败，处理错误
            qDebug() << "Error: " << query.lastError().text();
        }
    }
    return userPermissions;
}

void MainWindow:: showButton(const QMap<QString, int>& userPermissions)
{
    foreach (QPushButton *button, buttonList)
    {
        QString permissionName = button->objectName();

        if (userPermissions.contains(permissionName))
        {
            int permissionValue = userPermissions.value(permissionName);

            if (permissionValue == 1)
            {
                button->show();
            }
            else
            {
                button->hide();
            }
        }
        else
        {
            // 如果数据库中没有相应的权限数据，默认显示按钮
            button->show();
        }
    }
}

//工作日历--每日记录
void MainWindow::DailyInfo()
{
    QString id = nowaccount;
    QDate nowdate = ui->dailyDate->date();
    QString date  = nowdate.toString("yyyy-MM-DD");

    QSqlQuery query;
    query.prepare("SELECT mood,weather,diary FROM DailyInfo where ProduceDate = :date and account = :account");
    query.bindValue(":account",id);
    query.bindValue(":date",date);
    if(query.exec())
    {
        while (query.next())
        {
            QString mood = query.value(0).toString();
            QString weather = query.value(1).toString();
            QString diary = query.value(2).toString();

            ui->dailyMood->setCurrentText(mood);
            ui->dailyWeather->setCurrentText(mood);
            ui->dailyDairy->setText(diary);
        }
    }
}

void MainWindow::dailyAdd()
{
    QString account = nowaccount;
    QDate date1 = ui->dailyDate->date();
    QTime time1 = QTime::currentTime();
    QString mood = ui->dailyMood->currentText();
    QString weather = ui->dailyWeather->currentText();
    QString diary = ui->dailyDairy->toPlainText();

    QSqlQuery query;
    query.prepare("INSERT INTO DailyInfo (account,date,time,mood,weather,diary) VALUES (:account, :date, :time, :mood, :weather, :diary)");
    query.bindValue(":account", account);
    query.bindValue(":date", date1);
    query.bindValue(":time", time1);
    query.bindValue(":mood", mood);
    query.bindValue(":weather", weather);
    query.bindValue(":diary", diary);

    if (query.exec())
    {
        QMessageBox::information(NULL,"提示","今日新增记录成功");
    }
    else
    {
        // 插入失败
        QMessageBox::warning(NULL,"提示",query.lastError().text());
    }
}

void MainWindow::dailyshow()
{
    connect(ui->dailyDate, &QDateEdit::dateChanged, this, [=](const QDate &date){
        QString id = nowaccount;
        QString text = "SELECT time,mood,weather,diary FROM DailyInfo WHERE date = '" + date.toString("yyyy-MM-dd") + "' and account = '" + id + "'";
        QSqlQuery query;
        if(query.exec(text))
        {
            if(query.next())
            {
                QString time01 = query.value(0).toString();
                QString mood = query.value(1).toString();
                QString weather = query.value(2).toString();
                QString diary = query.value(3).toString();

                ui->dailyTime->setText(time01);
                ui->dailyMood->setEditText(mood);
                ui->dailyWeather->setEditText(weather);
                ui->dailyDairy->setText(diary);
            }
            else
            {
                ui->dailyTime->clear();
                ui->dailyMood->clear();
                ui->dailyWeather->clear();
                ui->dailyDairy->clear();
            }
        }
    });
}

void MainWindow::dailyRenew()
{
    QString account = nowaccount;
    QDate date1 = ui->dailyDate->date();
    QTime time1 = QTime::currentTime();
    QString mood = ui->dailyMood->currentText();
    QString weather = ui->dailyWeather->currentText();
    QString diary = ui->dailyDairy->toPlainText();

    QString updateQuery = "UPDATE DailyInfo set time = :time, mood = :mood, weather = :weather, diary = :diary WHERE account = :account and date = :date";
    QSqlQuery query(db);
    query.prepare(updateQuery);
    query.bindValue(":time", time1);
    query.bindValue(":mood", mood);
    query.bindValue(":weather", weather);
    query.bindValue(":diary", diary);
    query.bindValue(":account", account);
    query.bindValue(":date", date1);

    if (query.exec())
    {
        QMessageBox::information(NULL,"提示","更新成功");
    }
    else
    {
        QMessageBox::warning(NULL,"提示",query.lastError().text());
    }
}

//工作日历--工作情况
//void MainWindow::workInfo()
//{
//    QString id = nowaccount;
//    QDate date = QDate::currentDate();
//    //更改表头
//    QStandardItemModel *model1 = new QStandardItemModel();
//    QStringList info;
//    info<<"日期"<<"使用设备"<<"设备编号"<<"加工零件数量"<<"零件合格数量"<<"有效工作时长";
//    int numColumns = info.size(); // 设置模型的列数
//    model1->setColumnCount(numColumns);
//    for (int column = 0; column < numColumns; ++column)
//    {
//        if (column < info.size())
//        {
//            model1->setHeaderData(column, Qt::Horizontal, info.at(column));
//        }
//    }
//    QSqlQuery query;
//    query.prepare("SELECT * FROM YearInfo where ProduceDate = :date and account = :account");
//    query.bindValue(":account",id);
//    query.bindValue(":date",date);
//    if(query.exec())
//    {
//        // 遍历结果集并将数据添加到模型
//        while (query.next())
//        {
//            // 逐个获取列的数据
//            QString date = query.value(2).toString();
//            QString device = query.value(3).toString();
//            QString deviceID = query.value(4).toString();
//            QString Allnumber = query.value(5).toString();
//            QString number = query.value(6).toString();
//            QString Alltime = query.value(7).toString();

//            QStandardItem *item1 = new QStandardItem(date);
//            QStandardItem *item2 = new QStandardItem(device);
//            QStandardItem *item3 = new QStandardItem(deviceID);
//            QStandardItem *item4 = new QStandardItem(Allnumber);
//            QStandardItem *item5 = new QStandardItem(number);
//            QStandardItem *item6 = new QStandardItem(Alltime);

//            // 将项添加到模型的适当行和列中
//            model1->appendRow({item1, item2, item3, item4, item5, item6});
//        }
//    }

//    QFont headerFont("song", 12, QFont::Bold);//表头字体修改
//    ui->YMD_detial->horizontalHeader()->setFont(headerFont);
//    ui->YMD_detial->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);//表头对齐设置
//    ui->YMD_detial->horizontalHeader()->setStyleSheet("background-color: lightgray; color: black;");//表头背景色
//    QString headerStyle = "QHeaderView::section { background-color: lightblue; }";
//    ui->YMD_detial->horizontalHeader()->setStyleSheet(headerStyle);

//    ui->YMD_detial->setModel(model1);
//    ui->YMD_detial->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);    //让表格和窗口相等
//}

void MainWindow::YMDshow()
{
    QStandardItemModel *model2 = new QStandardItemModel(this);
    //具体到天
    connect(ui->YMD_choose, &QDateEdit::dateChanged, this, [=](const QDate &date){
        model2->removeRows(0, model2->rowCount()); // 清除模型中的所有行
        QString YMD = ui->YMD->currentText();//获取时间范围
        //更改表头
        QStringList info;
        info<<"日期"<<"使用设备"<<"设备编号"<<"加工零件数量"<<"零件合格数量"<<"有效工作时长";
        int numColumns = info.size(); // 设置模型的列数
        model2->setColumnCount(numColumns);
        for (int column = 0; column < numColumns; ++column)
        {
            if (column < info.size())
            {
                model2->setHeaderData(column, Qt::Horizontal, info.at(column));
            }
        }
        QString id = nowaccount;
        if(YMD == "年")
        {
            QString text = "SELECT * FROM YearInfo where YEAR(ProduceDate) = '" + date.toString("yyyy") + "' and account = '" + id + "'";
            execute(text,model2);
        }
        else if (YMD == "月")
        {
            QString text = "SELECT * FROM YearInfo where YEAR(ProduceDate) = '" + date.toString("yyyy") + "' and Month(ProduceDate) = '" + date.toString("MM") + "' and account = '" + id + "'";
            execute(text,model2);
        }
        else
        {
           QString text = "SELECT * FROM YearInfo where ProduceDate = '" + date.toString("yyyy-MM-dd") + "' and account = '" + id + "'";
           execute(text,model2);
        }

        QFont headerFont("song", 12, QFont::Bold);//表头字体修改
        ui->YMD_detial->horizontalHeader()->setFont(headerFont);
        ui->YMD_detial->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);//表头对齐设置
        ui->YMD_detial->horizontalHeader()->setStyleSheet("background-color: lightgray; color: black;");//表头背景色
        QString headerStyle = "QHeaderView::section { background-color: lightblue; }";
        ui->YMD_detial->horizontalHeader()->setStyleSheet(headerStyle);
        ui->YMD_detial->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);    //让表格和窗口相等
        ui->YMD_detial->setModel(model2);
    });
}

void MainWindow::execute(QString text,QStandardItemModel *model2)
{
    QSqlQuery query;
    if(query.exec(text))
    {
        while(query.next())
        {
            QDate date = query.value(2).toDate();
            QString device = query.value(3).toString();
            int deviceID = query.value(4).toInt();
            int Allnumber = query.value(5).toInt();
            int number = query.value(6).toInt();
            int Alltime = query.value(7).toInt();

            QStandardItem *item1 = new QStandardItem(date.toString("yyyy-MM-dd"));
            QStandardItem *item2 = new QStandardItem(device);
            QStandardItem *item3 = new QStandardItem(QString::number(deviceID));
            QStandardItem *item4 = new QStandardItem(QString::number(Allnumber));
            QStandardItem *item5 = new QStandardItem(QString::number(number));
            QStandardItem *item6 = new QStandardItem(QString::number(Alltime));

            // 将项添加到模型的适当行和列中
            model2->appendRow({item1, item2, item3, item4, item5, item6});
//                for (int row = 0; row < model2->rowCount(); ++row)
//                {
//                    for (int column = 0; column < model2->columnCount(); ++column)
//                    {
//                        qDebug() << model2->data(model2->index(row, column)).toString();
//                    }
//                }
        }
    }
    else
    {
        qDebug()<<query.lastError().text();
    }
}
