#ifndef poly_container_H
#define poly_container_H

#include <QVector>
#include <QQueue>
#include <QDebug>
#include <QImage>
#include <QFile>

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
    void render_grid();
    void render_gray();
    void save_compress(QString filename);
    void load_compress(QString filename);
    // load_compress_multi(int pos);
    poly get_poly();
    QImage& grid_img();
    QImage& gray_img();
    QVector<uchar> get_compressed_data();
    void print_compressed_data();

};




#endif // poly_container_H
