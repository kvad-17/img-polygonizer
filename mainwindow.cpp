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

    xy = QImage(256,256,QImage::Format_RGB32);
    xy.fill(QColor("white"));
    connect(ui->label_xy, SIGNAL(xyLabel_clicked(xyLabel*, QMouseEvent*)),
            this, SLOT(on_xy_mousePress(xyLabel*,QMouseEvent*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::drawPoint(int x, int y)
{
    painter.drawPoint(x,y);
}

void MainWindow::refresh_bmp()
{
    ui->bmpLabel->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::on_pushButton_clicked()
{
    static bool flag = true;
    QImage b (0,0);
    ui->bmpLabel->setScaledContents(true);
    if(flag)
    {ui->bmpLabel->setPixmap(QPixmap::fromImage(img)); flag = false;}
    else
    {ui->bmpLabel->setPixmap(QPixmap::fromImage(b)); flag = true;}
    //"D:\\w10profile\\Desktop\\d053950f15f07d715d16ccbb432b625f.png");
            //setPixmap(QPixmap::load
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

void MainWindow::on_pushButton_process_clicked()
{
    QRgb t;
    for(int x = 0; x < 256; x++)
       for(int y = 0; y < 256; y++)
       {
           t = img.pixel(x, y);
           if(qRed(t) <128)
               img.setPixel(x,y, 257);

           else img.setPixel(x,y, 0);

       }
    refresh_bmp();
}



void MainWindow::on_pushButton_getpixel_clicked()
{
    QRgb t = img.pixel(ui->lineEdit_pixelX->text().toInt(), ui->lineEdit_pixelY->text().toInt());
    int i = img.pixelIndex(ui->lineEdit_pixelX->text().toInt(), ui->lineEdit_pixelY->text().toInt());
    ui->label_pixelout->setText(QString::number(t,16).prepend("0x"));
    ui->label_blue->setText(QString::number(qBlue(t)));
    ui->label_red->setText(QString::number(qRed(t)));
    ui->label_green->setText(QString::number(qGreen(t)));
     ui->label_index->setText(QString::number(i));
}

void MainWindow::on_pushButton_checkgray_clicked()
{
    QRgb t;
    for(int x = 0; x < 256; x++)
       for(int y = 0; y < 256; y++)
       {
           t = img.pixel(x, y);
           img.pixel(x, y);
           img.setPixel(x,y, QColor(255-qRed(t),255-qGreen(t),255-qBlue(t)).rgba64());
//           if(qRed(t) == qBlue(t) && qBlue(t) == qGreen(t))


//           else img.setPixel(x,y, QColor(0,0,0).rgba64());

       }
    refresh_bmp();
}

void MainWindow::on_pushButton_format_clicked()
{
    QMessageBox::warning(this, "","Формат: " + QString::number(img.format()));
}


void MainWindow::on_pushButton_drawPoly_clicked()
{
    poly P(ui->lineEdit_drawAX->text().toDouble(), ui->lineEdit_drawAY->text().toDouble(),
           ui->lineEdit_drawBX->text().toDouble(), ui->lineEdit_drawBY->text().toDouble(),
           ui->lineEdit_drawCX->text().toDouble(), ui->lineEdit_drawCY->text().toDouble()
           );
    qDebug() << "HAHA";
    P.split(ui->lineEdit_drawDepth->text().toInt());
    P.print(xy, ui->lineEdit_drawDepth->text().toInt());
    ui->label_xy->setPixmap(QPixmap::fromImage(xy));
}

void MainWindow::on_pushButton_drawPolyClean_clicked()
{
    xy.fill(QColor("white"));
    ui->label_xy->setPixmap(QPixmap::fromImage(xy));
}

void MainWindow::on_pushButton_drawPolyClean_2_clicked()
{
    xy.fill(QColor("white"));
    ui->label_xy->setPixmap(QPixmap::fromImage(xy));
}



void MainWindow::on_pushButton_polySplit_clicked()
{
    QVector<QImage> imgv (256, QImage(256,256,QImage::Format_RGB32));
    double AX = ui->lineEdit_polyAX->text().toDouble(),
           BX = ui->lineEdit_polyBX->text().toDouble(),
           CX = ui->lineEdit_polyCX->text().toDouble(),
           DX = ui->lineEdit_polyDX->text().toDouble(),
           AY = ui->lineEdit_polyAY->text().toDouble(),
           BY = ui->lineEdit_polyBY->text().toDouble(),
           CY = ui->lineEdit_polyCY->text().toDouble(),
           DY = ui->lineEdit_polyDY->text().toDouble();

    ui->progressBar->setMaximum(255);
    for(int depth = 0; depth < 256; depth++)
    {
        ui->progressBar->setValue(depth);

        QApplication::processEvents();

        poly *P = new poly(AX,AY,BX,BY,CX,CY);
        poly *Q = new poly(BX,BY,CX,CY,DX,DY);

        P->split_img(depth, img);
        Q->split_img(depth, img);

        imgv[depth].fill(QColor("white"));
        P->print(imgv[depth], -1);
        Q->print(imgv[depth], -1);
        delete P; delete Q;
    }

    ui->horizontalSlider->setValue(127);
    ui->label_xy->setPixmap(QPixmap::fromImage(imgv[127]));
    imvec = imgv;
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    ui->label_progress->setText(QString::number(value));
    ui->label_xy->setPixmap(QPixmap::fromImage(imvec[value]));
}

void MainWindow::on_xy_mousePress(xyLabel *clicked, QMouseEvent *event)
{
    qDebug() << event->x() << ' ' << event->y();
}
