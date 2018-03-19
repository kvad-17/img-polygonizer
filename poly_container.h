#ifndef poly_container_H
#define poly_container_H

#include <QVector>
#include <QQueue>
#include <QDebug>
#include <QImage>
#include <QFile>
#include <QProgressBar>
#include <QApplication>

#include "poly.h"

class poly_container{

private:
    QByteArray compressed_data;
    poly P;
    QImage grid;
    QImage gray;

    bool poly_compressed;
    bool grid_rendered;
    bool gray_rendered;


public:
    poly_container();
    void clear_compress();

    void operator= (const poly_container&);
    poly_container (const poly_container&);
    poly_container (const poly&);
    poly_container (const QVector<uchar> cdata);
    void construct_image(const QImage& img, int depth);
    void poly_split (int depth);

    void compress();
    void decompress();
    void render_grid(double s_lim = 0);
    void render_gray(double s_lim = 0);
    void save_compress(QString filename);
    void load_compress(QString filename);
    static void save_compress_multi(QString filename, QVector<poly_container> &pv);
    static QVector<poly_container> load_compress_multi(QString filename, QProgressBar *pbar=nullptr);
    
    poly get_poly();
    QImage& grid_img(double s_lim = 0);
    QImage& gray_img(double s_lim = 0);
    QByteArray& get_compressed_data();
    void print_compressed_data();

};




#endif // poly_container_H
