#include <mainwindow.h>
#include <QDebug>
#include <QtGlobal>
#include <QStack>
#include <QVector>

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
        P = new poly(*(x.P));
        Q = new poly(*(x.Q));
    }
}
poly::~poly()
{
    if(is_splitted || is_rectangle)
    {
        delete P;
        delete Q;
    }
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
    P = new poly(_ax, _ay, _bx, _by, _cx, _cy); //ABC
    Q = new poly(_bx, _by, _cx, _cy, _dx, _dy); //BCD
}

void poly::print(QImage &img, bool fill = false, int n = -1)
{
    QPainter painter(&img);
    if(fill)
    {
        painter.setBrush(QBrush(QColor(gray,gray,gray)));
        painter.setPen(Qt::NoPen);
    }
    if(is_rectangle)        //  AB
    {                       //  CD
        QPolygon p;
        p << QPoint(P->ax,P->ay) << QPoint(P->bx,P->by) << QPoint(Q->cx,Q->cy) << QPoint(Q->bx,Q->by);
        painter.drawPolygon(p);
    }
    else
    {
        QPolygon p;
        p << QPoint(ax,ay) << QPoint(bx,by) << QPoint(cx,cy);
        painter.drawPolygon(p);
    }
    painter.end();
    if(n && is_splitted) {P->print(img, fill, n-1); Q->print(img, fill, n-1);}
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

bool poly::has_inside(pix p)
{
    double sab, sbc, scd, sda;
    if(is_rectangle)
    {
        sab = sideval(P->ax,P->ay,P->bx,P->by, p.dx, p.dy);//AB
        sbc = sideval(Q->ax,Q->ay,Q->cx,Q->cy, p.dx, p.dy);//BD
        scd = sideval(Q->cx,Q->cy,Q->bx,Q->by, p.dx, p.dy);//DC
        sda = sideval(P->cx,P->cy,P->ax,P->ay, p.dx, p.dy);//CA
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
      //  qDebug() << lab << ' ' << lac << ' ' << lbc << ' ';

        double tx, ty;
        if(lab > lac && lab > lbc){
            tx = (ax+bx)/2; ty = (ay+by)/2;
            P = new poly(cx, cy, ax, ay, tx, ty);
            Q = new poly(cx, cy, bx, by, tx, ty);
           // qDebug() << "lab";
        } else
        if(lac > lab && lac > lbc){
            tx = (ax+cx)/2; ty = (ay+cy)/2;
            P = new poly(bx, by, ax, ay, tx, ty);
            Q = new poly(bx, by, cx, cy, tx, ty);
           // qDebug() << "lac";
        } else
        if(lbc > lab && lbc > lac){
            tx = (cx+bx)/2; ty = (cy+by)/2;
            P = new poly(ax, ay, bx, by, tx, ty);
            Q = new poly(ax, ay, cx, cy, tx, ty);
            //qDebug() << "lbc";
        }
    }
    if(n) {P->split(n-1); Q->split(n-1);}
}

void poly::split_img(int lim, QImage &img)
{   //qDebug() << "split_img start";
    int minX, minY, maxX, maxY;
    if(is_rectangle)
    {
        minX = floor(qMin(qMin(P->ax, P->bx), qMin(Q->bx, Q->cx)));
        minY = floor(qMin(qMin(P->ay, P->by), qMin(Q->by, Q->cy)));
        maxX =  ceil(qMax(qMax(P->ax, P->bx), qMax(Q->bx, Q->cx)));
        maxY =  ceil(qMax(qMax(P->ay, P->by), qMax(Q->by, Q->cy)));
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

    gray = sum/c;

    if(g_max-g_min >= lim)
    {
        if(c == 0) qDebug() << "0-poly found";
        else if(c != 1)
        {
            split(0);
            P->split_img(lim, img);
            Q->split_img(lim, img);
        }
    }//qDebug() << "split_img end";
}

poly poly::getLowestPolyOnPix(pix px)
{
    poly *p;
    if(this->has_inside(px))
    {
        p = this;
        while(p->is_splitted)
        {
           if(p->P->has_inside(px))
               p = p->P;
           else if(p->Q->has_inside(px))
               p = p->Q;
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
            s.push(t->P);
            s.push(t->Q);
        }else v.append(t);
    }
    return v;
}
