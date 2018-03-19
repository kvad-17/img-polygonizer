#include "poly.h"
#include "poly_container.h"

void poly_container::save_compress_multi(QString filename, QVector<poly_container> &pv)
{
    foreach (poly_container pc, pv) {
        pc.save_compress(filename);
    }
}

QVector<poly_container> poly_container::load_compress_multi(QString filename, QProgressBar* pbar)
{
    QFile f(filename);
    f.open(QIODevice::ReadOnly);
    QByteArray cdata = f.readAll();
    f.close();
    
    QVector<poly_container> pc;
    poly P;
    int pd = 0;
    if(pbar) pbar->setMaximum(cdata.size());
    while(pd < cdata.size())
    {qDebug() << pd;
        P = poly(0,0,256,0,0,256,256,256);
        if((cdata[pd]>>7&1) == 0)
        {
            P.gray = cdata[pd+1];
            pd+=2;
        }
        else
        {
            P.split(0);
            poly *T;
            QQueue<poly*> C, N;
            QQueue<uchar*> G;
            bool ts0, ts1;
            char pb = 6;
            ts0 = (cdata[pd]>>(pb--))&1; if(pb==-1){pd++; pb=7;}
            ts1 = (cdata[pd]>>(pb--))&1; if(pb==-1){pd++; pb=7;}

            C.enqueue(&P);

            while(!C.empty() || !N.empty())
            {
                T = C.dequeue();

                if(ts0)
                {
                    N.enqueue(T->Q);
                    T->Q->split(0);
                }else G.enqueue(&T->Q->gray);
                if(ts1)
                {
                    N.enqueue(T->P);
                    T->P->split(0);
                }else G.enqueue(&T->P->gray);

                if(C.empty()) C.swap(N);
                if(!C.empty())
                {
                    ts0 = (cdata[pd]>>(pb--))&1; if(pb==-1){pd++; pb=7;}
                    ts1 = (cdata[pd]>>(pb--))&1; if(pb==-1){pd++; pb=7;}
                }
            }
            if(pb!=7) pd++;
            while(!G.empty())
            {
                *(G.dequeue()) = cdata[pd++];
            }
        }
        pc.append(poly_container(P));
        pc.last().render_grid();
        pc.last().render_gray();
        if(pbar) pbar->setValue(pd); QApplication::processEvents();
    }
    return pc;
}

void poly_container::save_compress(QString filename)
{
    if(!poly_compressed) compress();
    QFile f(filename);
    f.open(QIODevice::Append);
    f.write(compressed_data, compressed_data.size());
    f.close();
}

void poly_container::load_compress(QString filename)
{
    QFile f(filename);
    f.open(QIODevice::ReadOnly);
    compressed_data = f.readAll();
    f.close();
    poly_compressed = true;
}


void poly_container::decompress()
{
    if(poly_compressed)
    {
        P = poly(0,0,256,0,0,256,256,256);//try magic here
        if((compressed_data[0]>>7&1) == 0)
            P.gray = compressed_data[1];
        else
        {
            P.split(0);
            poly *T;
            QQueue<poly*> C, N;
            QQueue<uchar*> G;
            bool ts0, ts1;
            int pd = 0, pb = 6;
            ts0 = (compressed_data[pd]>>(pb--))&1; if(pb==-1){pd++; pb=7;}
            ts1 = (compressed_data[pd]>>(pb--))&1; if(pb==-1){pd++; pb=7;}

            C.enqueue(&P);

            while(!C.empty() || !N.empty())
            {
                T = C.dequeue();

                if(ts0)
                {
                    N.enqueue(T->Q);
                    T->Q->split(0);
                }else G.enqueue(&T->Q->gray);
                if(ts1)
                {
                    N.enqueue(T->P);
                    T->P->split(0);
                }else G.enqueue(&T->P->gray);

                if(C.empty()) C.swap(N);
                if(!C.empty())
                {
                    ts0 = (compressed_data[pd]>>(pb--))&1; if(pb==-1){pd++; pb=7;}
                    ts1 = (compressed_data[pd]>>(pb--))&1; if(pb==-1){pd++; pb=7;}
                }
            }
            if(pb!=7) pd++;
            while(!G.empty())
            {
                *(G.dequeue()) = compressed_data[pd++];
            }
        }
        gray_rendered = false;
        grid_rendered = false;
        render_gray();
        render_grid();
    }
}

void poly_container::compress()
{
    if(!poly_compressed)//
    {
         int e = 0;
        clear_compress();
        poly_compressed = true;
        QVector<bool> tree_struct;
        QVector<uchar> tree_data;

        tree_struct.append(P.is_splitted);
        if(!tree_struct[0])tree_data.append(P.gray);

        if(tree_struct[0])
        {
            QQueue<poly const*> Q, T;
            T.enqueue(&P);
            poly const* tpoly;
            bool qs, ps;
            while(!T.empty() || !Q.empty())
            {
                if(Q.empty()){
                    Q.swap(T);

                    QString t;
                    while(e < tree_struct.size())
                          t+=QString::number(tree_struct[e++]);
                    if(t.length() < 64) qDebug() << t;
                }
                tpoly = Q.dequeue();
                tree_struct.append( qs = tpoly->Q->is_splitted );
                tree_struct.append( ps = tpoly->P->is_splitted );
                if(qs) T.enqueue(tpoly->Q); else tree_data.append(tpoly->Q->gray);
                if(ps) T.enqueue(tpoly->P); else tree_data.append(tpoly->P->gray);
            }       
        }
        QString t;
        while(e < tree_struct.size())
              t+=QString::number(tree_struct[e++]);
        if(t.length() < 64) qDebug() << t;
        for(int i = 0; i < tree_struct.size(); i+=8 )
        {
            uchar t = 0;
            for(int b = 0; b<8; b++)
                t|= (((i+b < tree_struct.size())?tree_struct[i+b]:0)<<7-b);
            compressed_data.append(t);
        }
        for(int i = 0; i < tree_data.size(); i++)
        {
            compressed_data.append(tree_data[i]);
        }
    }
}


poly_container::poly_container()
{
    P = poly(0,0,256,0,0,256,256,256);
    poly_compressed = false;
    grid_rendered = false;
    gray_rendered = false;
}
void poly_container::clear_compress()
{
    if(poly_compressed)
    {
        compressed_data.clear();
        poly_compressed = false;
    }
}

void poly_container::operator= (const poly_container& PC)
{
    P = PC.P;
    clear_compress();
    if(poly_compressed = PC.poly_compressed)
        compressed_data = PC.compressed_data;
    if(grid_rendered = PC.grid_rendered)
        grid = PC.grid;
    if(gray_rendered = PC.gray_rendered)
        gray = PC.gray;
}
poly_container::poly_container (const poly_container& PC)
{
    P = PC.P;
    if(poly_compressed = PC.poly_compressed)
        compressed_data = PC.compressed_data;
    if(grid_rendered = PC.grid_rendered)
        grid = PC.grid;
    if(gray_rendered = PC.gray_rendered)
        gray = PC.gray;
}

poly_container::poly_container(const poly& A)
{

    P = A;
    poly_compressed = false;
    grid_rendered = false;
    gray_rendered = false;
}

void poly_container::construct_image (const QImage& img, int depth)
{
    P.split_img(depth, img);

    clear_compress();
    grid_rendered = false;
    gray_rendered = false;
}
void poly_container::poly_split (int depth)
{
    P.split(depth);
    clear_compress();
    grid_rendered = false;
    gray_rendered = false;
}



poly poly_container::get_poly()
{
    return P;
}
QImage& poly_container::grid_img(double s_lim)
{
    if(!grid_rendered) render_grid(s_lim);
    return grid;
}
QImage& poly_container::gray_img(double s_lim)
{
    if(!gray_rendered) render_gray(s_lim);
    return gray;
}

void poly_container::render_grid(double s_lim)
{
    if(!grid_rendered)
    {
        grid = QImage(257,257,QImage::Format_Grayscale8);
        grid.fill(QColor("white"));
        P.print(grid, false, -1, false, s_lim);
        grid_rendered = true;
    }
}
void poly_container::render_gray(double s_lim)
{
    if(!gray_rendered)
    {
        gray = QImage(256,256,QImage::Format_Grayscale8);
        gray.fill(QColor("white"));
        P.print(gray, true, -1, false, s_lim);
        gray_rendered = true;
    }
}

QByteArray& poly_container::get_compressed_data()
{
    return compressed_data;
}
