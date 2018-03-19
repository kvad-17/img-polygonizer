#ifndef POLY_H
#define POLY_H

#include <QDebug>
#include <QtGlobal>

struct pix{
    double dx;
    double dy;
    pix(int _x, int _y);
    int x();
    int y();
};

class poly
{
    friend class poly_container;
    friend class MainWindow;
private:
    uchar gray;
    double ax, ay, bx, by, cx, cy;
    bool is_splitted;
    bool is_rectangle;
    poly *P, *Q;
public:
    poly();
    poly(const poly&);
    void operator = (const poly&);
    ~poly();
    bool not_empty();


    //triangle ABC
    poly(double _ax, double _ay, double _bx, double _by, double _cx, double _cy);
    //rectangle ABC+BCD
    poly(double _ax, double _ay, double _bx, double _by,
         double _cx, double _cy, double _dx, double _dy);
    //coords setter
    void set(double _ax, double _ay, double _bx, double _by, double _cx, double _cy);

    void print(QImage &img, bool fill=false, int n=-1, bool abc = false, double s_lim = 0);

    bool has_inside(pix p);

    static void print_tree(poly *P, int level = 0);
    void split_img(int lim, const QImage &img);

    poly getLowestPolyOnPix(pix px);
    QVector<poly*> getLowestPolyVec();
    void split(int n);
    double S();
};


#endif // POLY_H
