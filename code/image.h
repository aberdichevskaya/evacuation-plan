#ifndef IMAGE_H
#define IMAGE_H

#include <QObject>
#include <QImage>
#include <QString>
#include <QVector>
#include <QPoint>
#include "edge.h"
#include <QMap>

class Image : public QObject {
    Q_OBJECT
public:
    Image();
    void createNewImage();
    void buildGraph();
    void rebuildCoordinates();
    QPoint getFinalCoordinates(int v);
    void updateCoordinates();
    void minCostMaxFlow();

    QImage big_im;
    QImage final_im;
    QVector<QPoint> people;
    QVector<QPair<QPoint, int>> exits;
    QVector<QVector<QPoint>> ways;
    QVector<QPoint> finalPeopleCrd;
    QVector<QPoint> finalExitsCrd;
    int time;

public slots:
    void rebuildImage(QImage& im);

signals:
    void finished();

private:
    int f(int x);
    QPoint getIdx(QPoint& p);
    void addEdge(int fr, int to, int cost, int cap);
    int getV(int i, int j);
    void bf(const int cur_flow);
    int bfWays();
    void findWays();
    void clear();

    int pixelsInBlock;
    QVector<int> rows;
    QVector<int> newr;
    QVector<int> cols;
    QVector<int> newc;
    int newn;
    int newm;
    int height;
    int width;
    QVector<QVector<bool>> mask;
    QVector<QVector<int>> mark;
    QVector<int> finalX;
    QVector<int> finalY;
    QVector<Edge> edges;
    QMap<int, QPoint> fromVToCoordinates;
    QVector<int> d;
    QVector<int> pr;
    QVector<int> minfl;
    int in;
    int out;
    const int INF = 100000;
};

#endif // IMAGE_H
