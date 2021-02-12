#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QPen>
#include <QPainter>
#include "paint.h"
#include "mainscene.h"
#include "setthroughput.h"
#include "image.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_paint_clicked();
    void setUpPicture();

    void on_addPeople_clicked();

    void on_addEnter_clicked();

    void on_generatePeople_clicked();

    void on_upLoad_clicked();

    void on_evacuation_clicked();

    void on_save_clicked();

private:
    Ui::MainWindow *ui;
    mainScene scene;
    Image* image;
};

#endif // MAINWINDOW_H
