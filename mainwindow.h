#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QPicture>
#include <QPainter>

#include "poly.h"
#include "poly_container.h"
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

    void on_checkBox_gridonly_toggled(bool checked);

    void on_pushButton_compress_clicked();

    void on_pushButton_decompress_clicked();

    void on_pushButton_compress_save_clicked();

    void on_pushButton_compress_load_clicked();

private:
    Ui::MainWindow *ui;
    QImage img;
    QImage xy;

    QVector<poly_container> pcvec;
    poly_container pc_depth;
    poly_container pc_file;

    poly_container* pc_work;

    void refresh_work_poly(poly_container *new_pc_work);
};

#endif // MAINWINDOW_H
