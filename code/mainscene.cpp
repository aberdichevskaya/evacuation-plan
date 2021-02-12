#include "mainscene.h"
#include <QColor>
#include <math.h>
#include <QTimer>
#include <QEventLoop>
#include <QDebug>

mainScene::mainScene(QObject *parent) : QGraphicsScene(parent)  {

}

void mainScene::setPixMap(QPixmap pixmap) {
    if (m_item != nullptr) {
        this->removeItem(m_item);
    }
    people.clear();
    enters.clear();
    m_item = this->addPixmap(pixmap);
}


void mainScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    int x = (int)(event->scenePos().x()+0.5);
    int y = (int)(event->scenePos().y()+0.5);
    if(painting == "enter") {
        painting = "";
        addRect(x-5,y-5, 10, 10,QPen(Qt::NoPen), QBrush(Qt::darkGreen));
        enters.push_back({QPoint(x, y), throughput});
    }
    if(painting == "person") {
        painting = "";
        addEllipse(x-5,y-5, 10, 10,QPen(Qt::NoPen), QBrush(QColor(Qt::yellow).darker(120)));
        people.push_back(QPoint(x,y));
    }
}


void mainScene::setPaintingState(QString state, int th) {
    painting = state;
    throughput = th;
}

void mainScene::paintRandomPeople(QVector<QPoint> &pos) {
    int n = pos.size();
    for(int i = 0; i < n; ++i) {
        int x = pos[i].x();
        int y = pos[i].y();
        if(QColor(m_item->pixmap().toImage().pixel(x,y)) != QColor(Qt::white)) {
            x += 20;
            y += 20;
        }
        addEllipse(x-5,y-5, 10, 10,QPen(Qt::NoPen), QBrush(QColor(Qt::yellow).darker(120)));
        people.push_back(QPoint(x,y));
    }
}

QVector<QPair<QPoint, int>>& mainScene::getEnters(){
    return enters;
}



QVector<QPoint>& mainScene::getPeople() {
    return people;
}


double distation(QPoint a, QPoint b) {
    double rez = (b.x() - a.x())*(b.x() - a.x()) + (b.y() - a.y())*(b.y() - a.y());
    return sqrt(rez);
}

void mainScene::drawArrows(QVector<QVector<QPoint> >& coordinates) {
    int n = coordinates.size(); //количество людей
    int m = 0;
    for(int i = 0; i < n; ++i) {
        std::reverse(coordinates[i].begin(), coordinates[i].end());
        if(coordinates[i].size() > m)
            m = coordinates[i].size();
    }
    QSet<int> ignored;
    for(int i = 0; i < n; ++i) {
        if(coordinates[i].size() < 2) {
            ignored.insert(i);
            continue;
        }
    }

    QVector<QVector<QPointF>> heads(n, {QPointF(0, 0), QPointF(0, 0), QPointF(0, 0)});

    for(int j = 0; j < m-1; ++j) {

        for(int i = 0; i < n; ++i) {

            if(coordinates[i].size() <= j+1) {
                ignored.insert(i);
                continue;
            }
            if(ignored.find(i) != ignored.end()) {
                continue;
            }

            QPointF t0 = heads[i][0];
            QPointF t1 = heads[i][1];
            QPointF p1 = heads[i][2];
            addLine(t0.x(), t0.y(),
                    p1.x(), p1.y(),
                    QPen(Qt::white,3,Qt::SolidLine,Qt::RoundCap));
            addLine(t1.x(), t1.y(),
                    p1.x(), p1.y(),
                    QPen(Qt::white,3,Qt::SolidLine,Qt::RoundCap));
        }
        for(int i = 0; i < n; ++i) {
            if(coordinates[i].size() <= j+1) {
                ignored.insert(i);
                continue;
            }
            if(ignored.find(i) != ignored.end()) {
                continue;
            }

            QPoint p0 = coordinates[i][j];

            QPoint p1 = coordinates[i][j+1];


            addLine(p0.x(), p0.y(),
                    p1.x(), p1.y(),
                    QPen(Qt::darkGreen,3,Qt::SolidLine,Qt::RoundCap));
            QPointF direction = (p1 - p0)/distation(p0, p1);
            QPointF normal(-direction.y(), direction.x());
            QPointF arrow_middle = p1 - direction * 5;
            QPointF t0 = arrow_middle + normal * 5;
            QPointF t1 = arrow_middle - normal * 5;
            addLine(t0.x(), t0.y(),
                    p1.x(), p1.y(),
                    QPen(Qt::darkGreen,3,Qt::SolidLine,Qt::RoundCap));
            addLine(t1.x(), t1.y(),
                    p1.x(), p1.y(),
                    QPen(Qt::darkGreen,3,Qt::SolidLine,Qt::RoundCap));

            heads[i] = {t0, t1, p1};
        }
        QEventLoop loop;
        QTimer timer;
        timer.setInterval(300);
        connect (&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
        timer.start();
        loop.exec();
    }
}

void mainScene::painPoints(QVector<QPoint>& v, const QString& what) {
    for(int i = 0; i < v.size(); ++i) {
        int x = v[i].y();
        int y = v[i].x();
        QColor c;
        if(what == "person")
            c = QColor(Qt::yellow).darker(120);
        if(what == "enter")
            c = Qt::darkGreen;
        addRect(x-5,y-5, 10, 10,QPen(Qt::NoPen), QBrush(c));
    }
}
