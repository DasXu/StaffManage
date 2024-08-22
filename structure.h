#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <QDialog>

namespace Ui {
class structure;
}

class structure : public QDialog
{
    Q_OBJECT

public:
    explicit structure(QWidget *parent = nullptr);
    ~structure();

private:
    Ui::structure *ui;
};

#endif // STRUCTURE_H
