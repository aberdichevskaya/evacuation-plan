#ifndef EDGE_H
#define EDGE_H


class Edge {
public:
    Edge(int fr, int to, int cost)
        :from(fr), to(to), cost(cost), capacity(0), flow(0) {}
    Edge(int fr, int to, int cost, int cap)
        :from(fr), to(to), cost(cost), capacity(cap), flow(0) {}
    int from;
    int to;
    int cost;
    int capacity;
    int flow;
};

#endif // EDGE_H
