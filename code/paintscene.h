#ifndef PAINTSCENE_H
#define PAINTSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QTimer>
#include <QDebug>
#include <QVector>
#include <QPair>
#include <QGraphicsPixmapItem>

class paintScene : public QGraphicsScene
{

    Q_OBJECT

public:
    explicit paintScene(QRect rect, QObject *parent = 0);
    ~paintScene();
    void canselation();

    QRect rect;

private:
    QPointF begin;
    QPointF end;
    bool started = 0;
    QVector<QPair<QPointF, QPointF>> cancel;
    QGraphicsPixmapItem * m_item;


    void mousePressEvent(QGraphicsSceneMouseEvent * event);


};
#endif // PAINTSCENE_H
