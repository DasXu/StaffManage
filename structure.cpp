#include "structure.h"
#include "ui_structure.h"

structure::structure(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::structure)
{
    ui->setupUi(this);
}

structure::~structure()
{
    delete ui;
}
