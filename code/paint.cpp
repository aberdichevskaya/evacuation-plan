#include "paint.h"
#include "ui_paint.h"
#include <QRect>
#include <QPixmap>
#include <QPainter>
#include <QPalette>

Paint::Paint(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Paint){
    ui->setupUi(this);

    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, Qt::darkCyan);
    this->setAutoFillBackground(true);
    this->setPalette(Pal);
    Pal.setColor(QPalette::Button, Qt::white);
    ui->cancelation->setPalette(Pal);
    ui->save->setPalette(Pal);
    QRect rect(0,0, ui->graphicsView->width() - 20, ui->graphicsView->height() - 20);
    scene = new paintScene(rect);

    ui->graphicsView->setMinimumSize(800, 600);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setScene(scene);
    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &Paint::slotTimer);
    timer->start(100);
}

Paint::~Paint() {
    delete ui;
}

void Paint::slotTimer() {
    timer->stop();
    QRect rect(0,0, ui->graphicsView->width() - 20, ui->graphicsView->height() - 20);
    scene->setSceneRect(rect);
    scene->rect = rect;
}

void Paint::resizeEvent(QResizeEvent *event) {
    timer->start(100);
    QWidget::resizeEvent(event);
}

void Paint::on_cancelation_clicked(){
    scene->canselation();
}

void Paint::on_save_clicked() {
    QPixmap pix = QPixmap::grabWidget(ui->graphicsView);
    img = pix.toImage();
    for(int i = 0; i < img.height(); ++i) {
        img.setPixelColor(0, i, Qt::black);
        img.setPixelColor(1, i, Qt::black);
        img.setPixelColor(img.width() - 1, i, Qt::black);
        img.setPixelColor(img.width() - 2, i, Qt::black);
    }
    for(int i = 0; i < img.width(); ++i) {
        img.setPixelColor(i, 0, Qt::black);
        img.setPixelColor(i, 1, Qt::black);
        img.setPixelColor(i, img.height() - 1, Qt::black);
        img.setPixelColor(i, img.height() - 2, Qt::black);
    }
   // img.save("C:/Users/User/Desktop/хлам/picture.png");
    emit picture_is_ready(img);
    this->hide();
}

