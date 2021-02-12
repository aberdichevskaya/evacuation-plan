#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "paint.h"
#include "setthroughput.h"

#include <QDebug>
#include <QPixmap>
#include <QVector>
#include <QPair>
#include <QFileDialog>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    image = new Image();
    ui->setupUi(this);
    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, Qt::darkCyan);
    this->setAutoFillBackground(true);
    this->setPalette(Pal);
    ui->addEnter->setEnabled(false);
    ui->addPeople->setEnabled(false);
    ui->generatePeople->setEnabled(false);
}

MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::on_paint_clicked() {
    Paint* p = new Paint(this);
    connect(p, SIGNAL(picture_is_ready(QImage&)), image, SLOT(rebuildImage(QImage&)));
    connect(image, SIGNAL(finished()), this, SLOT(setUpPicture()));
    p->show();
}

void MainWindow::setUpPicture() {
    ui->addEnter->setEnabled(true);
    ui->addPeople->setEnabled(true);
    ui->generatePeople->setEnabled(true);
    QPixmap pixmap;
    pixmap.convertFromImage(image->final_im);
    scene.setPixMap(pixmap);
    ui->graphicsView->setScene(&scene);
}

void MainWindow::on_addPeople_clicked() {
    scene.setPaintingState("person");
}

void MainWindow::on_addEnter_clicked() {
    setThroughput* th = new setThroughput("Пропускная способность выхода", this);
    th->exec();
    int throughput = th->getThroughput();
    scene.setPaintingState("enter", throughput);
}

void MainWindow::on_generatePeople_clicked() {
    setThroughput* th = new setThroughput("Количество людей", this);
    th->exec();
    int amount = th->getThroughput();
    srand(unsigned(time(0)));
    QVector<QPoint> rand_people(amount);
    int w = image->final_im.width();
    int h = image->final_im.height();
    for(int i = 0; i < amount; ++i) {
        int a = rand() % (w-40) + 20;
        int b = rand() % (h-40) + 20;
        rand_people[i] = QPoint(a, b);
    }
    scene.paintRandomPeople(rand_people);
}

void MainWindow::on_upLoad_clicked() {
    QString path = QFileDialog::getOpenFileName(this, "Выберите файл", "C:", "*.jpg;; *.jpeg;; *.png");
    if (path == "") {
        return;
    }
    QImage img;
    img.load(path);
    connect(image, SIGNAL(finished()), this, SLOT(setUpPicture()));
    image->rebuildImage(img);
}

void MainWindow::on_evacuation_clicked() {
    image->people = scene.getPeople();
    image->exits = scene.getEnters();
    image->minCostMaxFlow();
    setUpPicture();
    scene.painPoints(image->finalPeopleCrd, "person");
    scene.painPoints(image->finalExitsCrd, "enter");
    scene.drawArrows(image->ways);
    int t = image->time;
    QString str;
        str.setNum(t);
        if(t % 10 == 1) {
            str += " минуту";
        }
        else if(t%10 > 1 && t%10 < 5) {
            str += " минуты";
        }
        else {
            str += " минут";
        }
    QMessageBox::about(this, "Время эвакуации", "Время эвакуации составило " + str);
}

void MainWindow::on_save_clicked() {
    QPixmap pix = QPixmap::grabWidget(ui->graphicsView);
        QImage img = pix.toImage();
        QString path = QFileDialog::getSaveFileName(this,
                                        "Сохранить план эвакуации",
                                        "",
                                        "*.png ;; *.jpg ;; *.bmp"
                                        );
        img.save(path);
}
