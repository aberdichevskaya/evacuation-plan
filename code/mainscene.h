#ifndef MAINSCENE_H
#define MAINSCENE_H

#include <QObject>
#include <QGraphicsScene>
#include <QPixmap>
#include <QString>
#include <QPoint>
#include <QVector>
#include <QPair>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>

class mainScene : public QGraphicsScene {
    Q_OBJECT

public:
    explicit mainScene(QObject *parent = nullptr);
    void setPixMap(QPixmap pixmap);
    void setPaintingState(QString state, int th = 0);
    void paintRandomPeople(QVector<QPoint>& pos);
    QVector<QPair<QPoint, int>>& getEnters();
    QVector<QPoint>& getPeople();
    void drawArrows(QVector<QVector<QPoint>>& coordinates);
    void painPoints(QVector<QPoint>& v, const QString& what);

private:
    QString painting = "";
    int throughput = 0;
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    QVector<QPair<QPoint, int>> enters;
    QVector<QPoint> people;
    QGraphicsPixmapItem * m_item = nullptr;
};

#endif // MAINSCENE_H
