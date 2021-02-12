#include "image.h"
#include <math.h>
#include <iostream>
#include <QPair>
#include <QDebug>

Image::Image() {
}

void Image::clear() {
    people.clear();
    exits.clear();
    ways.clear();
    rows.clear();
    newr.clear();
    cols.clear();
    newc.clear();
    mask.clear();
    mark.clear();
    finalX.clear();
    finalY.clear();
    finalExitsCrd.clear();
    finalPeopleCrd.clear();
    edges.clear();
    fromVToCoordinates.clear();
    d.clear();
    pr.clear();
    minfl.clear();
}

int Image::f(int x) {
    return x * 1.0 / pixelsInBlock;
}

void Image::createNewImage() {
    height = newn * pixelsInBlock + rows.size() * 2;
    width = newm * pixelsInBlock + cols.size() * 2;
    final_im = QImage(width, height, QImage::Format_RGB32);
    final_im.fill(Qt::white);
    int x = 0;
    int y = 0;
    for (int i = 0; i < mask.size(); ++i) {
        y = 0;
        int h, w;
        for (int j = 0; j < mask[0].size(); ++j) {
            if (mark[i][j] == 0) {
                h = pixelsInBlock;
                w = pixelsInBlock;
            }
            if (mark[i][j] == 1) {
                h = 2;
                w = pixelsInBlock;
            }
            if (mark[i][j] == 2) {
                h = pixelsInBlock;
                w = 2;
            }
            if (mark[i][j] == 3) {
                h = 2;
                w = 2;
            }
            if (!mask[i][j]) {
                y += w;
                continue;
            }
            for (int i1 = 0; i1 < h; ++i1) {
                for (int j1 = 0; j1 < w; ++j1) {
                    final_im.setPixelColor(y + j1, x + i1, Qt::black);
                }
            }
            y += w;
        }
        x += h;
    }
    finalX.resize(newn);
    int pr = 0;
    int idx = 0;
    for (int i = 0; i < mask.size(); ++i) {
        int s = 2;
        if (mark[i][0] == 0 || mark[i][0] == 2) {
            s = pixelsInBlock;
            finalX[idx] = pr + s / 2;
            ++idx;
        }
        pr += s;
    }
    finalY.resize(newm);
    pr = 0;
    idx = 0;
    for (int i = 0; i < mask[0].size(); ++i) {
        int s = 2;
        if (mark[0][i] == 0 || mark[0][i] == 1) {
            s = pixelsInBlock;
            finalY[idx] = pr + s / 2;
            ++idx;
        }
        pr += s;
    }
}

void Image::rebuildImage(QImage& im) {
    clear();
    // Уменьшим размер
    im = im.scaledToWidth(700);
    // Подгоним размер, чтобы можно было разбить на ровное число блоков
    pixelsInBlock = sqrt(im.width() * im.height() / 500.0);
    int n = int(im.height() * 1.0 / pixelsInBlock + 0.5) * pixelsInBlock;
    int m = int(im.width() * 1.0 / pixelsInBlock + 0.5) * pixelsInBlock;
    im = im.scaled(m, n);
    // Найдем все стены - вертикальные и горизонтальные черные линии
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (im.pixelColor(j, i).black() > 200 && j+5 < m && im.pixelColor(j+5, i).black() > 200) {
                rows.push_back(i);
                i += 5;
                break;
            }
        }
    }
    for (int j = 0; j < m; ++j) {
        for (int i = 0; i < n; ++i) {
            if (im.pixelColor(j, i).black() > 200 && i+5 < n && im.pixelColor(j, i+5).black() > 200) {
                cols.push_back(j);
                j += 5;
                break;
            }
        }
    }
    // Найдем индексы стен на новой уменьшенной картинке
    int usedr = 0, usedc = 0;
    for (int i = 0; i < rows.size(); ++i) {
        int idx = rows[i] * 1.0 / pixelsInBlock + 0.5 + usedr;
        newr.push_back(idx);
        ++usedr;
    }
    for (int i = 0; i < cols.size(); ++i) {
        int idx = cols[i] * 1.0 / pixelsInBlock + 0.5 + usedc;
        newc.push_back(idx);
        ++usedc;
    }
    // Новые размеры картинки без учета стен
    newn = n / pixelsInBlock;
    newm = m / pixelsInBlock;
    // Заполним матрицу меток: 0 - постое пространство, 1 - столбец для вертикальной стены,
    // 2 - строка для горизонтальной стены, 3 - угол, пересчение стен
    mark.resize(newn + rows.size());
    for (int i = 0; i < mark.size(); ++i) {
        mark[i].resize(newm + cols.size());
        mark[i].fill(0);
    }
    for (int i = 0; i< newr.size(); ++i) {
        for (int j = 0; j < mark[0].size(); ++j) {
            mark[newr[i]][j] += 1;
        }
    }
    for (int j = 0; j < newc.size(); ++j) {
        for (int i = 0; i < mark.size(); ++i) {
            mark[i][newc[j]] += 2;
        }
    }
    // Новая картинка и матрица для нее
    mask.resize(newn + rows.size());
    for (int i = 0; i < mask.size(); ++i) {
        mask[i].resize(newm + cols.size());
        mask[i].fill(false);
    }
    // Переносим стены на новую картинку и в матрицу
    // Пройдемся по всем строкам, содержащим горизонтальные стены
    for (int i = 0; i < rows.size(); ++i) {
        int idx = newr[i];
        int idx_in_cols = 0;
        for (int j = 0; j < newm; ++j) {
            // Для правильного определения нового индекса столбца учтем пройденные вертикальные стены
            if (idx_in_cols < newc.size() && j + idx_in_cols >= newc[idx_in_cols]) {
                ++idx_in_cols;
            }
            int len = 0;
            // Рассчитаем длину стены внутри блока
            for (int j1 = j * pixelsInBlock; j1 < (j + 1) * pixelsInBlock; ++j1) {
                if (im.pixelColor(j1, rows[i]).black() > 200 || (rows[i]+1 < n && im.pixelColor(j1, rows[i] + 1).black() > 200)) {
                    ++len;
                }
            }
            // Если длина стены больше половины длины блока, то в новой матрице так же расположим здесь стену
            if (len * 2 >= pixelsInBlock) {
                mask[idx][j + idx_in_cols] = true;
            }

        }
    }
    // Сделаем аналогичные действия но по столбцам с горизонтальными участками стен
    for (int j = 0; j < cols.size(); ++j) {
        int jdx = newc[j];
        int idx_in_rows = 0;
        for (int i = 0; i < newn; ++i) {
            if (idx_in_rows < newr.size() && i + idx_in_rows >= newr[idx_in_rows]) {
                ++idx_in_rows;
            }
            int len = 0;
            for (int i1 = i * pixelsInBlock; i1 < (i + 1) * pixelsInBlock; ++i1) {
                if (im.pixelColor(cols[j], i1).black() > 200 || (cols[j]+1 < n && im.pixelColor(cols[j] + 1, i1).black() > 200)) {
                    ++len;
                }
            }
            if (len * 2 >= pixelsInBlock) {
                mask[i + idx_in_rows][jdx] = true;
            }
        }
    }
    // Заполним пустоты  на пересечении стен
    for (int i = 0; i < newr.size(); ++i) {
        for (int j = 0; j < newc.size(); ++j) {
            int i1 = newr[i], j1 = newc[j];
            int cnt = 0;
            if (i1 - 1 >= 0 && mask[i1-1][j1])
                ++cnt;
            if (i1 + 1 < mask.size() && mask[i1+1][j1])
                ++cnt;
            if (j1 - 1 >= 0 && mask[i1][j1-1])
                ++cnt;
            if (j1 + 1 < mask[0].size() && mask[i1][j1+1])
                ++cnt;
            if (cnt >= 2) {
                mask[i1][j1] = true;
            }
        }
    }

    for (int i = 0; i<mark.size(); ++i) {
        for (int j = 0; j < mark[i].size(); ++j) {
            std::cout<<mark[i][j] <<  ' ';
        }
        std::cout<<std::endl;
    }
    createNewImage();
    emit finished();
}

void Image::rebuildCoordinates() {
    for (int i = 0; i < people.size(); ++i) {
        int x = people[i].y();
        int y = people[i].x();
        int pix = final_im.height() / newn;
        if (final_im.height() % newn) {
            ++pix;
        }
        people[i].setX(x * 1.0 / pix + 0.5);
        pix = final_im.width() / newm;
        if (final_im.width() % newm) {
            ++pix;
        }
        people[i].setY(y * 1.0 / pix + 0.5);
    }
    for (int i = 0; i < exits.size(); ++i) {
        int x = exits[i].first.y();
        int y = exits[i].first.x();
        int pix = final_im.height() / newn;
        if (final_im.height() % newn) {
            ++pix;
        }
        exits[i].first.setX(x * 1.0 / pix + 0.5);
        pix = final_im.width() / newm;
        if (final_im.width() % newm) {
            ++pix;
        }
        exits[i].first.setY(y * 1.0 / pix + 0.5);
    }
}

void Image::addEdge(int fr, int to, int cost, int cap) {
    edges.push_back(Edge(fr, to, cost, cap));
    edges.push_back(Edge(to, fr, -cost));
}

int Image::getV(int i, int j) {
    return i * newm + j;
}

void Image::buildGraph() {
    rebuildCoordinates();
    int idx = 0, jdx = 0;
    for (int i = 0; i < mask.size(); ++i) {
        if (mark[i][0] != 0 && mark[i][0] != 2) {
            continue;
        }
        jdx = 0;
        for (int j = 0; j < mask[0].size(); ++j) {
            if (mark[i][j] != 0) {
                continue;
            }
            int fr = getV(idx, jdx);
            fromVToCoordinates[fr] = QPoint(idx, jdx);
            if (i - 1 >= 0 && idx - 1 >= 0 && !mask[i-1][j]) {
                int to = getV(idx - 1, jdx);
                addEdge(fr, to, 1, INF);
            }
            if (j + 1 < mask[0].size() && jdx + 1 < newm && !mask[i][j+1]) {
                int to = getV(idx, jdx + 1);
                addEdge(fr, to, 1, INF);
            }
            if (i + 1 < mask.size() && idx + 1 < newn && !mask[i + 1][j]) {
                int to = getV(idx + 1, jdx);
                addEdge(fr, to, 1, INF);
            }
            if (j - 1 >= 0 && jdx - 1 >= 0 && !mask[i][j-1]) {
                int to = getV(idx, jdx - 1);
                addEdge(fr, to, 1, INF);
            }
            ++jdx;
        }
        ++idx;
    }
    in = newn * newm;
    out = in + 1;
    for (int i = 0; i < people.size(); ++i) {
        int to = getV(people[i].x(), people[i].y());
        addEdge(in, to, 0, 1);
    }
    int sum_cap = 0;
    for (int i = 0; i < exits.size(); ++i) {
        sum_cap += exits[i].second;
    }
    int num_ppl = people.size();
    int k = num_ppl / sum_cap;
    if (num_ppl % sum_cap) {
        ++k;
    }
    time = k + 1;
    for (int i = 0; i < exits.size(); ++i) {
        int fr = getV(exits[i].first.x(), exits[i].first.y());
        addEdge(fr, out, 0, exits[i].second * k);
    }
}

void Image::bf(const int cur_flow) {
    d[in] = 0;
    bool flag = true;
    while (flag) {
        flag = false;
        for (int i = 0; i < edges.size(); ++i) {
            int to = edges[i].to;
            int fr = edges[i].from;
            int len = d[fr] + edges[i].cost;
            if (d[fr] < INF && len < d[to] && edges[i].flow < edges[i].capacity) {
                d[to] = len;
                pr[to] = i;
                minfl[to] = std::min(minfl[fr], edges[i].capacity - edges[i].flow);
                flag = true;
            }
        }
    }
    minfl[out] = std::min(minfl[out], people.size() - cur_flow);
}

int Image::bfWays() {
    d[in] = 0;
    bool flag = true;
    int flow = INF;
    while (flag) {
        flag = false;
        for (int i = 0; i < edges.size(); ++i) {
            int to = edges[i].to;
            int fr = edges[i].from;
            int len = d[fr] + edges[i].cost;
            if (d[fr] < INF && len < d[to] && edges[i].flow > 0) {
                d[to] = len;
                pr[to] = i;
                flow = std::min(flow, edges[i].flow);
                flag = true;
            }
        }
    }
    return flow;
}

void Image::minCostMaxFlow() {
    buildGraph();
    d.resize(newn * newm + 2);
    pr.resize(newn * newm + 2);
    minfl.resize(newn * newm + 2);
    int cur_flow = 0;
    while (cur_flow < people.size()) {
        d.fill(INF);
        pr.fill(-1);
        minfl.fill(INF);
        bf(cur_flow);
        if (d[out] == INF) {
            break;
        }
        cur_flow += minfl[out];
        int v = out;
        QVector<QPoint> w;
        while (true) {
            if (v == in) {
                break;
            }
            edges[pr[v]].flow += minfl[out];
            edges[pr[v] ^ 1].flow -= minfl[out];
            v = edges[pr[v]].from;
        }
    }
    findWays();
}

void Image::findWays() {
    for (int i = 0; i < people.size(); ++i) {
        d.fill(INF);
        pr.fill(-1);
        int flow = bfWays();
        int v = out;
        QVector<QPoint> way;
        while (true) {
            if (v == in) {
                break;
            }
            edges[pr[v]].flow -= flow;
            v = edges[pr[v]].from;
            if (v != in) {
                way.push_back(getFinalCoordinates(v));
            }
        }
        ways.push_back(way);
    }
    updateCoordinates();
}

QPoint Image::getFinalCoordinates(int v) {
    QPoint p = fromVToCoordinates[v];
    p.setX(finalX[p.x()]);
    p.setY(finalY[p.y()]);
    return p;
}

void Image::updateCoordinates() {
    finalPeopleCrd.resize(people.size());
    for (int i = 0; i < people.size(); ++i) {
        QPoint p = getFinalCoordinates(getV(people[i].x(), people[i].y()));
        finalPeopleCrd[i] = p;

    }
    finalExitsCrd.resize(exits.size());
    for (int i = 0; i < exits.size(); ++i) {
        QPoint p = getFinalCoordinates(getV(exits[i].first.x(), exits[i].first.y()));
        finalExitsCrd[i] = p;
    }
    for (int i = 0; i < ways.size(); ++i) {
        for (int j = 0; j < ways[i].size(); ++j) {
            ways[i][j] = ways[i][j].transposed();
        }
    }
}
