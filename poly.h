#ifndef POLY_H
#define POLY_H

#include <mainwindow.h>
#include <QDebug>
#include <QtGlobal>


class poly
{
private:
    double ax, ay, bx, by, cx, cy;
    bool is_splitted;
    poly *P, *Q;
public:
    poly();
    ~poly();
    bool not_empty();
    poly(double _ax, double _ay, double _bx, double _by, double _cx, double _cy);

    void print(QImage &img, int n);
    void set(double _ax, double _ay, double _bx, double _by, double _cx, double _cy);
    bool is_inside(double x, double y);


    void split_img(int lim, QImage &img);

    poly getPoly(double x, double y);

    void split(int n);
};


#endif // POLY_H
