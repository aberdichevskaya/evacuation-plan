#include "setthroughput.h"
#include "ui_setthroughput.h"
#include <QString>
#include  <QMessageBox>

setThroughput::setThroughput(QString r, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::setThroughput) {
    ui->setupUi(this);
    reason = r;
    ui->label->setText(reason);
}

setThroughput::~setThroughput() {
    delete ui;
}

void setThroughput::on_ok_clicked() {
    throughput = ui->spinBox->value();
    this->hide();
}


int setThroughput::getThroughput() {
    return throughput;
}
