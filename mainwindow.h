#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QPicture>
#include <QPainter>

#include "poly.h"
#include "poly_compress.h"
#include "xylabel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void drawPoly(int x, int y); //костыли

private slots:
    void on_pushButton_load_clicked();

    void on_pushButton_drawPolyClean_clicked();

    void on_pushButton_polySplit_clicked();

    void on_horizontalSlider_valueChanged(int value);

    void on_xy_mousePress(xyLabel *clicked, QMouseEvent * event);
    void on_pushButton_drawPolyABCD_clicked();

    void on_pushButton_polySplitOnce_clicked();

    void on_pushButton_polySplitPreset0_clicked();

    void on_pushButton_polySplitPreset1_clicked();

    void on_pushButton_polySplitPreset2_clicked();

    void on_pushButton_checkall_clicked();

    void on_pushButton_stackprint_clicked();

    void on_pushButton_compress_clicked();

    void on_pushButton_compress_print_clicked();

    void on_pushButton_compress_show_clicked();

    void on_checkBox_gridonly_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    QImage img;
    QImage img_orig;
    QImage img_work;

    QImage img_spoly;
    
    QVector<QImage> imgvec;
    QVector<QImage> imgcvec;

    QVector<poly> polvec;

    QImage xy;

    poly poly_depth;

    poly_compress PC;

    void refresh_bmp();

};

#endif // MAINWINDOW_H
