#include <mainwindow.h>
#include <QDebug>
#include <QtGlobal>

double sideval(double ax, double ay,
               double bx, double by,
               double x, double y)
{
    return (ax-x)*(by-ay)-(ay-y)*(bx-ax);
}

poly::poly()
{
    ax = ay = bx = by = cx = cy = 0;
    is_rectangle = is_splitted = false;
}

poly::poly(const poly &x)
{
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

void poly::print(QImage &img, int n)
{
    QPainter painter(&img);
    if(is_rectangle)        //  AB
    {                       //  CD
        painter.drawLine(P->ax,P->ay,P->bx,P->by);//AB
        painter.drawLine(Q->ax,Q->ay,Q->cx,Q->cy);//BD
        painter.drawLine(Q->cx,Q->cy,Q->bx,Q->by);//DC
        painter.drawLine(P->cx,P->cy,P->ax,P->ay);//CA
    }
    else
    {
        painter.drawLine(ax,ay,bx,by);//AB
        painter.drawLine(ax,ay,cx,cy);//AC
        painter.drawLine(bx,by,cx,cy);//BC
    }
    painter.end();
    if(n && is_splitted) {P->print(img, n-1); Q->print(img, n-1);}
}
bool poly::is_inside(double x, double y)
{
    double sab, sbc, scd, sda;
    if(is_rectangle)
    {
        sab = sideval(P->ax,P->ay,P->bx,P->by, x, y);//AB
        sbc = sideval(Q->ax,Q->ay,Q->cx,Q->cy, x, y);//BD
        scd = sideval(Q->cx,Q->cy,Q->bx,Q->by, x, y);//DC
        sda = sideval(P->cx,P->cy,P->ax,P->ay, x, y);//CA
    }
    else
    {
        sab = sideval(ax, ay, bx, by, x, y);
        sbc = sideval(bx, by, cx, cy, x, y);
        scd = sideval(cx, cy, ax, ay, x, y);
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

    int c = 0, g, g_max = 0, g_min = 255;
    double mgx = 0.46756456, mgy = 0.53224456;

    for(int x = minX; x < maxX; x++)
        for(int y = minY; y < maxY; y++)
            if (is_inside(x+mgx, y+mgy))
            {
                c++;
                g = qGray(img.pixel(x, y));
                if (g > g_max) g_max = g;
                if (g < g_min) g_min = g;
            }

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

//poly getPoly(double x, double y, poly& parent)
//    {
//        if(parent.not_empty())
//        {
        
//        }else return poly();
//    }
