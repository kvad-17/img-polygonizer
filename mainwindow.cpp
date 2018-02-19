#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "poly.cpp"
#include <QMouseEvent>

xyLabel::xyLabel( QWidget * parent ):QLabel(parent) {}

extern MainWindow *p;
void xyLabel::mousePressEvent ( QMouseEvent * event )
{
    emit xyLabel_clicked(this, event);
}

//void MainWindow::drawPoly(int x, int y)
//{
//    qDebug() << x << ' ' << y;
//    int depth = ui->horizontalSlider->value();
//    poly sample =
    
//}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    xy = QImage(257,257,QImage::Format_RGB32);
    xy.fill(QColor("white"));
    connect(ui->label_xy, SIGNAL(xyLabel_clicked(xyLabel*, QMouseEvent*)),
            this, SLOT(on_xy_mousePress(xyLabel*,QMouseEvent*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refresh_bmp()
{
    ui->bmpLabel->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::on_pushButton_load_clicked()
{
    QString bmpfile;
    while(!bmpfile.length())
    {
       bmpfile = QFileDialog::getOpenFileName(this, "Открытие изображения",
                                           "D:\\w10profile\\Desktop\\faxmepic",
                                           "Images (*.bmp *.png *.jpg *.jpeg *.tiff *.gif)");
    }

    img.load(bmpfile);

    bool warn = false;
    QString warn_text;

    if(!img.allGray())
    {
        warn = true;
        warn_text += "  - image is not grayscale\n";
    }

    if(img.size() != QSize(256,256))
    {
        warn = true;
        warn_text += "  - size converted " +
                        QString::number(img.size().width()) + "x" +
                        QString::number(img.size().height()) +" -> 256x256\n";
        img = img.copy(0,0,256,256);
    }

    if(img.format() != QImage::Format_RGB32)
    {
        warn = true;
        warn_text += "  - format converted QImageFormat/" + QString::number(img.format()) + " -> Format_RGB32\n";
        img = img.convertToFormat(QImage::Format_RGB32);
    }

    if(warn) QMessageBox::warning(this, "Image load warning", warn_text);

    refresh_bmp();
}

void MainWindow::on_pushButton_drawPolyClean_clicked()
{
    xy.fill(QColor("white"));
    ui->label_xy->setPixmap(QPixmap::fromImage(xy));
}

void MainWindow::on_pushButton_polySplit_clicked()
{
    double AX = ui->lineEdit_polyAX->text().toDouble(),
           BX = ui->lineEdit_polyBX->text().toDouble(),
           CX = ui->lineEdit_polyCX->text().toDouble(),
           DX = ui->lineEdit_polyDX->text().toDouble(),
           AY = ui->lineEdit_polyAY->text().toDouble(),
           BY = ui->lineEdit_polyBY->text().toDouble(),
           CY = ui->lineEdit_polyCY->text().toDouble(),
           DY = ui->lineEdit_polyDY->text().toDouble();

    bool triangle =  (ui->lineEdit_polyDX->text() == "-")||
                     (ui->lineEdit_polyDY->text() == "-");

    QVector<QImage> imgv (257, QImage(257,257,QImage::Format_RGB32));
    QVector<poly> polv (257, triangle?poly(AX,AY,BX,BY,CX,CY):poly(AX,AY,BX,BY,CX,CY,DX,DY));

    ui->progressBar->setMaximum(256);
    for(int depth = 0; depth <= 256; depth++)
    {
        ui->progressBar->setValue(depth);
        polv[depth].split_img(depth, img);
        imgv[depth].fill(QColor("white"));
        polv[depth].print(imgv[depth], -1);
    }
    imgvec = imgv;
    polvec = polv;
    ui->horizontalSlider->setValue(128);
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    ui->label_progress->setText(QString::number(value));
    ui->label_xy->setPixmap(QPixmap::fromImage(imgvec[value]));
}

void MainWindow::on_xy_mousePress(xyLabel *clicked, QMouseEvent *event)
{
    qDebug() << event->x() << ' ' << event->y();
}

void MainWindow::on_pushButton_drawPolyABCD_clicked()
{
    double AX = ui->lineEdit_polyAX->text().toDouble(),
           BX = ui->lineEdit_polyBX->text().toDouble(),
           CX = ui->lineEdit_polyCX->text().toDouble(),
           DX = ui->lineEdit_polyDX->text().toDouble(),
           AY = ui->lineEdit_polyAY->text().toDouble(),
           BY = ui->lineEdit_polyBY->text().toDouble(),
           CY = ui->lineEdit_polyCY->text().toDouble(),
           DY = ui->lineEdit_polyDY->text().toDouble();
    poly a(AX,AY,BX,BY,CX,CY,DX,DY);
    a.split(ui->lineEdit_drawPolyABCDdepth->text().toInt());
    a.print(xy, -1);
     ui->label_xy->setPixmap(QPixmap::fromImage(xy));
}

void MainWindow::on_pushButton_polySplitOnce_clicked()
{
    QImage i(257,257,QImage::Format_RGB32);
    i.fill(QColor("white"));
    double AX = ui->lineEdit_polyAX->text().toDouble(),
           BX = ui->lineEdit_polyBX->text().toDouble(),
           CX = ui->lineEdit_polyCX->text().toDouble(),
           DX = ui->lineEdit_polyDX->text().toDouble(),
           AY = ui->lineEdit_polyAY->text().toDouble(),
           BY = ui->lineEdit_polyBY->text().toDouble(),
           CY = ui->lineEdit_polyCY->text().toDouble(),
           DY = ui->lineEdit_polyDY->text().toDouble();
    int depth = ui->lineEdit_polyDepth->text().toInt();
    poly a(AX,AY,BX,BY,CX,CY,DX,DY);

    a.split_img(depth, img);
    a.print(i, -1);

    ui->label_xy->setPixmap(QPixmap::fromImage(i));
}

void MainWindow::on_pushButton_polySplitPreset0_clicked()
{
    ui->lineEdit_polyAX->setText("0");
    ui->lineEdit_polyBX->setText("256");
    ui->lineEdit_polyCX->setText("0");
    ui->lineEdit_polyDX->setText("256");
    ui->lineEdit_polyAY->setText("0");
    ui->lineEdit_polyBY->setText("0");
    ui->lineEdit_polyCY->setText("256");
    ui->lineEdit_polyDY->setText("256");
}

void MainWindow::on_pushButton_polySplitPreset1_clicked()
{
    ui->lineEdit_polyAX->setText("96");
    ui->lineEdit_polyBX->setText("210");
    ui->lineEdit_polyCX->setText("40");
    ui->lineEdit_polyDX->setText("160");
    ui->lineEdit_polyAY->setText("96");
    ui->lineEdit_polyBY->setText("64");
    ui->lineEdit_polyCY->setText("192");
    ui->lineEdit_polyDY->setText("160");
}

void MainWindow::on_pushButton_polySplitPreset2_clicked()
{
    ui->lineEdit_polyAX->setText("0");
    ui->lineEdit_polyBX->setText("256");
    ui->lineEdit_polyCX->setText("0");
    ui->lineEdit_polyDX->setText("256");
    ui->lineEdit_polyAY->setText("0");
    ui->lineEdit_polyBY->setText("0");
    ui->lineEdit_polyCY->setText("256");
    ui->lineEdit_polyDY->setText("256");
}

