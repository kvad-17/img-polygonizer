#include <mainwindow.h>
#include <QDebug>
#include <QtGlobal>

double sideval(double ax, double ay, double bx, double by, double x, double y)
{
    return (ax-x)*(by-ay)-(ay-y)*(bx-ax);
}

poly::poly(){
    ax = ay = bx = by = cx = cy = 0;
    is_splitted = false;
}

bool poly::not_empty()
{
    return ax && ay && bx && by && cx && cy;
}

poly::~poly(){
    if(is_splitted){
        delete P;
        delete Q;
    }
}

poly::poly(double _ax, double _ay, double _bx, double _by, double _cx, double _cy) {
    set(_ax, _ay, _bx, _by, _cx, _cy);
}

void poly::print(QImage &img, int n)
{
    QPainter painter(&img);
    painter.drawLine(ax,ay,bx,by);
    painter.drawLine(ax,ay,cx,cy);
    painter.drawLine(bx,by,cx,cy);
    painter.end();
    if(n && is_splitted) {P->print(img, n-1); Q->print(img, n-1);}
}

void poly::set(double _ax, double _ay, double _bx, double _by, double _cx, double _cy) {
    ax = _ax;
    ay = _ay;
    bx = _bx;
    by = _by;
    cx = _cx;
    cy = _cy;
    is_splitted = false;
}


bool poly::is_inside(double x, double y)
{
    double sab = sideval(ax, ay, bx, by, x, y);
    double sbc = sideval(bx, by, cx, cy, x, y);
    double sca = sideval(cx, cy, ax, ay, x, y);
    //qDebug() << "inside";
    return (sab < 0 && sbc < 0 && sca < 0) ||
        (sab > 0 && sbc > 0 && sca > 0);
}

void poly::split_img(int lim, QImage &img)
{
    int minX = floor(qMin(ax, qMin(bx, cx)));
    int minY = floor(qMin(ay, qMin(by, cy)));
    int maxX =  ceil(qMax(ax, qMax(bx, cx)));
    int maxY =  ceil(qMax(ay, qMax(by, cy)));

    int c = 0, e = 0, g, g_max = 0, g_min = 255;

    //qDebug() << minX << ' ' << maxX << ' ' << minY << ' ' << maxY;

    for(int x = minX; x < maxX; x++)
        for(int y = minY; y < maxY; y++)
            if (is_inside(x+0.46456, y+0.54456))//(x+0.46456, y+0.54456)
            {//qDebug() << c;
                c++;
                g = qGray(img.pixel(x, y));
                if (g > g_max) g_max = g;
                if (g < g_min) g_min = g;
            }//else qDebug() << e++ << ' ' << x << ' ' << y;
   // qDebug() << c;

    if(g_max-g_min >= lim)
    {
        if(c == 0) qDebug() << "0-poly found";
        else if(c != 1)
        {
            split(0);
            P->split_img(lim, img);
            Q->split_img(lim, img);
        }
    }
}

poly getPoly(double x, double y, poly& parent)
    {
        if(parent.not_empty())
        {
        
        }else return poly();
    }

void poly::split(int n)
{
    is_splitted = true;
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

    if(n) {P->split(n-1); Q->split(n-1);}
}
