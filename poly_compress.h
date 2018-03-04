#ifndef POLY_COMPRESS_H
#define POLY_COMPRESS_H

#include <QVector>
#include <QQueue>
#include <QDebug>

#include "poly.h"

class poly_compress{

private:
    QVector<bool> tree_struct;
    QVector<uchar> tree_data;

public:
    poly_compress();
    poly_compress (const poly&);
    poly_compress (const QVector<uchar> data);
    poly* decompress();

    void print();


};

#endif // POLY_COMPRESS_H
