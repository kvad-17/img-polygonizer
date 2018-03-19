#include "poly.h"
#include <QDebug>
#include <QtGlobal>
#include <QStack>
#include <QVector>
#include <QPainter>

double sideval(double ax, double ay,
               double bx, double by,
               double x, double y)
{
    return (ax-x)*(by-ay)-(ay-y)*(bx-ax);
}

poly::poly()
{
    gray = ax = ay = bx = by = cx = cy = 0;
    is_rectangle = is_splitted = false;
}

poly::poly(const poly &x)
{
    gray = x.gray;
    ax = x.ax;
    ay = x.ay;
    bx = x.bx;
    by = x.by;
    cx = x.cx;
    cy = x.cy;
    is_rectangle = x.is_rectangle;
    is_splitted = x.is_splitted;

    if(is_rectangle || is_splitted)
    {
        Q = new poly(*(x.Q));
        P = new poly(*(x.P));
    }
}
void poly::operator = (const poly &x)
{
    if(is_rectangle || is_splitted)
    {
        delete Q;
        delete P;
    }
    gray = x.gray;
    ax = x.ax;
    ay = x.ay;
    bx = x.bx;
    by = x.by;
    cx = x.cx;
    cy = x.cy;
    is_rectangle = x.is_rectangle;
    is_splitted = x.is_splitted;

    if(is_rectangle || is_splitted)
    {
        Q = new poly(*(x.Q));
        P = new poly(*(x.P));
    }
}

poly::~poly()
{
    if(is_splitted || is_rectangle)
    {
        delete Q;
        delete P;
    }
    is_splitted = false;
    is_rectangle = false;
}
bool poly::not_empty()
{
    return ax && ay && bx && by && cx && cy;
}

void poly::set(double _ax, double _ay,
               double _bx, double _by,
               double _cx, double _cy) {
    ax = _ax;
    ay = _ay;
    bx = _bx;
    by = _by;
    cx = _cx;
    cy = _cy;
    is_splitted = is_rectangle = false;
    gray = 0;
}
poly::poly(double _ax, double _ay,
           double _bx, double _by,
           double _cx, double _cy) {
    set(_ax, _ay, _bx, _by, _cx, _cy);
}
poly::poly(double _ax, double _ay,
           double _bx, double _by,
           double _cx, double _cy,
           double _dx, double _dy)
{
    poly();
    is_rectangle = true;
    is_splitted = false;
    Q = new poly(_ax, _ay, _bx, _by, _cx, _cy); //ABC
    P = new poly(_bx, _by, _cx, _cy, _dx, _dy); //BCD
}

void poly::print(QImage &img, bool fill, int n, bool abc)
{
    QPainter painter(&img);
    if(fill)
    {
        painter.setBrush(QBrush(QColor(gray,gray,gray)));
        painter.setPen(Qt::NoPen);
    }
    if(is_rectangle || !is_splitted)
    if(is_rectangle)        //  AB
    {                       //  CD
        QPolygon p;
        p << QPoint(Q->ax,Q->ay) << QPoint(Q->bx,Q->by) << QPoint(P->cx,P->cy) << QPoint(P->bx,P->by);
        painter.drawPolygon(p);
    }
    else
    {
        QPolygon p;
        p << QPoint(ax,ay) << QPoint(bx,by) << QPoint(cx,cy);
        painter.drawPolygon(p);
        if(abc && 0)
        {
            painter.setPen(Qt::SolidLine);
            painter.drawText(QPoint(ax,ay), "A");
            painter.drawText(QPoint(bx,by), "B");
            painter.drawText(QPoint(cx,cy), "C");
        }
    }
    painter.end();
    if(n && is_splitted) {Q->print(img, fill, n-1); P->print(img, fill, n-1);}
}

#define mgx 0.46756456
#define mgy 0.53224456

pix::pix(int _x, int _y){
    dx = _x + mgx;
    dy = _y + mgy;
}

int pix::x()
{
    return round(dx-mgx);
}
int pix::y()
{
    return round(dy-mgy);
}

void poly::print_tree(poly *P, int level)
{
    QString out;
    for(int i = 0; i < level; i++) out+=' ';
    out+=QString::number(P->is_splitted);
    qDebug() << out;

    if(P->is_splitted)
    {
        print_tree(P->Q, level+1);
        print_tree(P->P, level+1);
    }
}

bool poly::has_inside(pix p)
{
    double sab, sbc, scd, sda;
    if(is_rectangle)
    {
        sab = sideval(Q->ax,Q->ay,Q->bx,Q->by, p.dx, p.dy);//AB
        sbc = sideval(P->ax,P->ay,P->cx,P->cy, p.dx, p.dy);//BD
        scd = sideval(P->cx,P->cy,P->bx,P->by, p.dx, p.dy);//DC
        sda = sideval(Q->cx,Q->cy,Q->ax,Q->ay, p.dx, p.dy);//CA
    }
    else
    {
        sab = sideval(ax, ay, bx, by, p.dx, p.dy);
        sbc = sideval(bx, by, cx, cy, p.dx, p.dy);
        scd = sideval(cx, cy, ax, ay, p.dx, p.dy);
        sda = scd;
    }

    return (sab < 0 && sbc < 0 && scd < 0 && sda < 0) ||
           (sab > 0 && sbc > 0 && scd > 0 && sda > 0);
}
void poly::split(int n)
{
    is_splitted = true;
    if(!is_rectangle)
    {
        double lab = sqrt(pow(ax-bx,2)+pow(ay-by,2));
        double lac = sqrt(pow(ax-cx,2)+pow(ay-cy,2));
        double lbc = sqrt(pow(bx-cx,2)+pow(by-cy,2));

        //без sideval'ов можно обойтись, они нужны лишь для человекочитаемости сжатых данных
        //почему в условиях нужно было поставить '<0' для меня загадка, по экспериментам, условие должно быть обратным
        //возможно, разгадка кроется не в этой функции, но пока пусть работает так
        //FIX::разгадка найдена. В экранных координатах ось Y отражена обратно относительно математических координат.
        //Поэтому результаты sideval следует трактовать наоборот, из-за зеркального отражения всего изображения.

        double tx, ty;
        if(lab > lac && lab > lbc){
            tx = (ax+bx)/2; ty = (ay+by)/2;
            if( sideval(tx,ty,cx,cy,ax,ay)<0 )//Точка A находится слева от вектора TC?
            {
//                qDebug() << "sab " << tx << ' ' << ty << ' ' << cx << ' ' << cy << ' ' << ax << ' ' << ay
//                            << ' ' << sideval(tx,ty,cx,cy,ax,ay);
//                qDebug() << "Q " << cx << ' ' << cy << ' ' << ax << ' ' << ay << ' ' << tx << ' ' << ty;
//                qDebug() << "P " << cx << ' ' << cy << ' ' << bx << ' ' << by << ' ' << tx << ' ' << ty;
                Q = new poly(cx, cy, ax, ay, tx, ty);
                P = new poly(cx, cy, bx, by, tx, ty);
            }else{
//                qDebug() << "s " << tx << ' ' << ty << ' ' << cx << ' ' << cy << ' ' << ax << ' ' << ay
//                            << ' ' << sideval(tx,ty,cx,cy,ax,ay);
//                qDebug() << "P " << cx << ' ' << cy << ' ' << ax << ' ' << ay << ' ' << tx << ' ' << ty;
//                qDebug() << "Q " << cx << ' ' << cy << ' ' << bx << ' ' << by << ' ' << tx << ' ' << ty;
                Q = new poly(cx, cy, bx, by, tx, ty);
                P = new poly(cx, cy, ax, ay, tx, ty);
            }
        } else
        if(lac > lab && lac > lbc){
            tx = (ax+cx)/2; ty = (ay+cy)/2;
            if( sideval(tx,ty,bx,by,ax,ay)<0 )//Точка A находится слева от вектора TB?
            {
                Q = new poly(bx, by, ax, ay, tx, ty);
                P = new poly(bx, by, cx, cy, tx, ty);
            }else{
                Q = new poly(bx, by, cx, cy, tx, ty);
                P = new poly(bx, by, ax, ay, tx, ty);
            }
        } else
        if(lbc > lab && lbc > lac){
            tx = (cx+bx)/2; ty = (cy+by)/2;
            if( sideval(tx,ty,ax,ay,bx,by)<0 )//Точка B находится слева от вектора TA?
            {
                Q = new poly(ax, ay, bx, by, tx, ty);
                P = new poly(ax, ay, cx, cy, tx, ty);
            }else{
                Q = new poly(ax, ay, cx, cy, tx, ty);
                P = new poly(ax, ay, bx, by, tx, ty);
            }
        }
    }
    if(n) {Q->split(n-1); P->split(n-1);}
}

void poly::split_img(int lim, const QImage &img)
{
    int minX, minY, maxX, maxY;
    if(is_rectangle)
    {
        minX = floor(qMin(qMin(Q->ax, Q->bx), qMin(P->bx, P->cx)));
        minY = floor(qMin(qMin(Q->ay, Q->by), qMin(P->by, P->cy)));
        maxX =  ceil(qMax(qMax(Q->ax, Q->bx), qMax(P->bx, P->cx)));
        maxY =  ceil(qMax(qMax(Q->ay, Q->by), qMax(P->by, P->cy)));
    }
    else
    {
        minX = floor(qMin(ax, qMin(bx, cx)));
        minY = floor(qMin(ay, qMin(by, cy)));
        maxX =  ceil(qMax(ax, qMax(bx, cx)));
        maxY =  ceil(qMax(ay, qMax(by, cy)));
    }

    int sum = 0, c = 0, g, g_max = 0, g_min = 255;

    for(int x = minX; x < maxX; x++)
        for(int y = minY; y < maxY; y++)
            if (has_inside(pix(x,y)))
            {
                g = qGray(img.pixel(x, y));
                c++; sum+= g;
                if (g > g_max) g_max = g;
                if (g < g_min) g_min = g;
            }
   // qDebug() << c;
    gray = sum/(c);

    if(g_max-g_min >= lim)
    {
        if(c == 0) qDebug() << "0-poly found";
        else if(c != 1)
        {
            split(0);
            Q->split_img(lim, img);
            P->split_img(lim, img);
        }
    }
}

poly poly::getLowestPolyOnPix(pix px)
{
    poly *p;
    if(this->has_inside(px))
    {
        p = this;
        while(p->is_splitted)
        {
           if(p->Q->has_inside(px))
               p = p->Q;
           else if(p->P->has_inside(px))
               p = p->P;
           else
           {
               qDebug() << "lost px at " << px.x() << ":" << px.y();
               return poly();
           }
        }
        return *p;

    }else return poly();
}

QVector<poly*> poly::getLowestPolyVec()
{
    QStack<poly*> s;
    QVector<poly*> v;
    poly *t;
    s.push(this);
    while(!s.empty())
    {
        t = s.pop();
        if(t->is_splitted)
        {
            s.push(t->Q);
            s.push(t->P);
        }else v.append(t);
    }
    return v;
}
