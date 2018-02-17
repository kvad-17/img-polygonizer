#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QPicture>
#include <QPainter>

#include "poly.h"
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
    void on_pushButton_clicked();

    void on_pushButton_load_clicked();

    void on_pushButton_process_clicked();

    void on_pushButton_getpixel_clicked();

    void on_pushButton_checkgray_clicked();

    void on_pushButton_format_clicked();

    void on_pushButton_drawPoly_clicked();

    void on_pushButton_drawPolyClean_clicked();

    void on_pushButton_drawPolyClean_2_clicked();

    void on_pushButton_polySplit_clicked();

    void on_horizontalSlider_valueChanged(int value);

    void on_xy_mousePress(xyLabel *clicked, QMouseEvent * event);
private:
    Ui::MainWindow *ui;
    QImage img;
    QImage img_orig;
    QImage img_work;

    QImage img_spoly;
    
    QVector<QImage> imvec;
    QVector<poly> pvec;

    QImage xy;
    QPainter painter;

    void drawPoint(int x, int y);
    void refresh_bmp();

};

#endif // MAINWINDOW_H
