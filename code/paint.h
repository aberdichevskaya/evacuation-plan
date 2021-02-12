#ifndef PAINT_H
#define PAINT_H


#include <QDialog>
#include <QTimer>
#include <QResizeEvent>

#include <paintscene.h>

namespace Ui {
class Paint;
}

class Paint : public QDialog
{
    Q_OBJECT

public:
    explicit Paint(QWidget *parent = nullptr);
    ~Paint();

private:
    Ui::Paint *ui;
    QTimer *timer;
    QImage img;
    paintScene *scene;

private:
    void resizeEvent(QResizeEvent * event);

private slots:
    void slotTimer();
    void on_cancelation_clicked();
    void on_save_clicked();

signals:
    void picture_is_ready(QImage& img);
};

#endif // PAINT_H
