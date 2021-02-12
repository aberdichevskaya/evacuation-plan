#ifndef SETTHROUGHPUT_H
#define SETTHROUGHPUT_H

#include <QDialog>
#include <QString>

namespace Ui {
class setThroughput;
}

class setThroughput : public QDialog
{
    Q_OBJECT

public:
    explicit setThroughput(QString r, QWidget *parent = nullptr);
    ~setThroughput();
    int getThroughput();

private slots:
    void on_ok_clicked();


private:
    Ui::setThroughput *ui;
    int throughput = 0;
    QString reason = "";
};

#endif // SETTHROUGHPUT_H
