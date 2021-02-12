#include "paintscene.h"
#include <QErrorMessage>
#include <QMessageBox>
#include <math.h>

paintScene::paintScene(QRect rect, QObject *parent) : QGraphicsScene(parent) {
    this->rect = rect;
    QPixmap m(rect.size());
    m.fill();
    m_item = this->addPixmap(m);
}

paintScene::~paintScene() {

}

void paintScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {

    if(!started) {
        addEllipse((int) (event->scenePos().x() - 1 + 0.5),
                   (int)(event->scenePos().y() - 1 + 0.5),
                   2,
                   2,
                   QPen(Qt::NoPen),
                   QBrush(Qt::black));
        begin = event->scenePos();
        cancel.push_back({begin, begin});
        started = 1;
    }
    else {
        end = event->scenePos();
        if(fabs(begin.x() - end.x()) > 50 && fabs(begin.y() - end.y()) > 50) {

            QMessageBox::warning(0, "Information",
                        "Линии должны быть горизонтальные либо вертикальные");
        }
        else {
        addEllipse((int) (begin.x() - 1 + 0.5),
                   (int)(begin.y() - 1 + 0.5),
                   2,
                   2,
                   QPen(Qt::NoPen),
                   QBrush(Qt::white));
        started = 0;


        int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
        if(abs(begin.x() - end.x()) < abs(begin.y() - end.y())) {
            x1 = (int)( std::min(begin.x(), end.x()) + fabs(begin.x() - end.x())/2. + 0.5);
            x2 = x1;
            y1 = (int)(begin.y()+0.5);
            y2 = (int)(end.y() + 0.5);
        }
        else {
            y1 = (int)( std::min(begin.y(), end.y()) + fabs(begin.y() - end.y())/2. + 0.5);
            y2 = y1;
            x1 = (int)(begin.x()+0.5);
            x2 = (int)(end.x() + 0.5);
        }
        addLine(x1, y1, x2, y2, QPen(Qt::black,2,Qt::SolidLine,Qt::RoundCap));
        cancel.pop_back();
        cancel.push_back({QPointF(x1, y1), QPointF(x2, y2)});
    }
}

}

void paintScene::canselation() {
    if(cancel.size() > 0) {
    QPair<QPointF, QPointF> pair = cancel[cancel.size()-1];
    cancel.pop_back();
    if(pair.first == pair.second) {
        started = 0;
        addEllipse((int) (pair.first.x() - 1 + 0.5),
                   (int)(pair.first.y() - 1 + 0.5),
                   2,
                   2,
                   QPen(Qt::NoPen),
                   QBrush(Qt::white));
    }
    else {
        addLine(pair.first.x(), pair.first.y(), pair.second.x(), pair.second.y(), QPen(Qt::white,2,Qt::SolidLine,Qt::RoundCap));
    }
    for(const auto& item : cancel) {
        if(item.first == item.second) {
            started = 0;
            addEllipse((int) (item.first.x() - 1 + 0.5),
                       (int)(item.first.y() - 1 + 0.5),
                       2,
                       2,
                       QPen(Qt::NoPen),
                       QBrush(Qt::black));
        }
        else {
            addLine(item.first.x(), item.first.y(), item.second.x(), item.second.y(), QPen(Qt::black,2,Qt::SolidLine,Qt::RoundCap));
        }
    }
    }
}
