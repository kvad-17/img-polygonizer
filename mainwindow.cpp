#include <QMouseEvent>
#include <QVector>
#include "mainwindow.h"
#include "poly.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QBuffer>

xyLabel::xyLabel( QWidget * parent ):QLabel(parent) {}

extern MainWindow *p;
void xyLabel::mousePressEvent ( QMouseEvent * event )
{
    emit xyLabel_clicked(this, event);
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    img = pc_file.grid_img();

    connect(ui->labelxy_img_work, SIGNAL(xyLabel_clicked(xyLabel*, QMouseEvent*)),
            this, SLOT(on_xy_mousePress(xyLabel*,QMouseEvent*)));
    ui->label_img_origin->setPixmap(QPixmap::fromImage(pc_file.grid_img()));
    ui->label_img_poly->setPixmap(QPixmap::fromImage(pc_file.grid_img()));
    refresh_work_poly(&pc_file);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::refresh_work_poly(poly_container* new_pc_work)
{
    QImage i = ui->checkBox_gridonly->checkState()?
                new_pc_work->grid_img():new_pc_work->gray_img();
    pc_work = new_pc_work;
    ui->labelxy_img_work->setPixmap(QPixmap::fromImage(i));
    pc_work->compress();

    QByteArray iba;
    QBuffer buff (&iba);
    img.save(&buff, "BMP");
    double bmp_sz = buff.size();
    img.save(&buff, "JPG");
    double jpg_sz = buff.size()-bmp_sz;
    double poly_sz = pc_work->get_compressed_data().size();


    qDebug() << "BMP: " << bmp_sz;
    qDebug() << "JPG: " << jpg_sz;
    qDebug() << "PLY: " << poly_sz;

    double s = 0;
    for(int x = 0; x < 256; x++)
        for(int y = 0; y < 256; y++)
            s += pow(qGray(img.pixel(x,y)) - qGray(i.pixel(x,y)), 2);
    s = sqrt(s/(256*256));

    ui->label_sko->setText(QString::number(s));
    ui->label_kbmp->setText(QString::number(bmp_sz/poly_sz));
    ui->label_kjpg->setText(QString::number(jpg_sz/poly_sz));
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

    if(img.format() != QImage::Format_Grayscale8)
    {
        warn = true;
        warn_text += "  - format converted QImageFormat/" + QString::number(img.format()) + " -> Format_Grayscale8\n";
        img = img.convertToFormat(QImage::Format_Grayscale8);
    }

    //if(warn) QMessageBox::warning(this, "Image load warning", warn_text);

    ui->label_img_origin->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::on_pushButton_drawPolyClean_clicked()
{
    QImage i(257,257,QImage::Format_Grayscale8);
    i.fill(QColor("white"));
    ui->labelxy_img_work->setPixmap(QPixmap::fromImage(i));
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

    pcvec = QVector<poly_container> (257, triangle?poly(AX,AY,BX,BY,CX,CY):poly(AX,AY,BX,BY,CX,CY,DX,DY));

    ui->progressBar->setMaximum(256);
    for(int depth = 0; depth <= 256; depth++)
    {
        ui->progressBar->setValue(depth);   QApplication::processEvents();
        pcvec[depth].construct_image(img, depth);  QApplication::processEvents();
        pcvec[depth].render_grid(); QApplication::processEvents();
        pcvec[depth].render_gray(); QApplication::processEvents();
    }

    ui->horizontalSlider->setValue(128);
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    ui->label_progress->setText(QString::number(value));
    refresh_work_poly(&pcvec[value]);
}

void MainWindow::on_xy_mousePress(xyLabel *clicked, QMouseEvent *event)
{
    poly x = pc_work->get_poly().getLowestPolyOnPix(pix(event->x(), event->y()));
    QImage i(257,257, QImage::Format_Grayscale8);
    i.fill(QColor("white"));
    x.print(i, true, -1, true);
    ui->label_img_poly->setPixmap(QPixmap::fromImage(i));
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
    bool triangle =  (ui->lineEdit_polyDX->text() == "-")||
                     (ui->lineEdit_polyDY->text() == "-");

    poly_container a(triangle?poly(AX,AY,BX,BY,CX,CY):poly(AX,AY,BX,BY,CX,CY,DX,DY));
    pc_depth = a;
    pc_depth.poly_split(ui->lineEdit_drawPolyABCDdepth->text().toInt());
    refresh_work_poly(&pc_depth);
}

void MainWindow::on_pushButton_polySplitOnce_clicked()
{
    double AX = ui->lineEdit_polyAX->text().toDouble(),
           BX = ui->lineEdit_polyBX->text().toDouble(),
           CX = ui->lineEdit_polyCX->text().toDouble(),
           DX = ui->lineEdit_polyDX->text().toDouble(),
           AY = ui->lineEdit_polyAY->text().toDouble(),
           BY = ui->lineEdit_polyBY->text().toDouble(),
           CY = ui->lineEdit_polyCY->text().toDouble(),
           DY = ui->lineEdit_polyDY->text().toDouble();
    int depth = ui->lineEdit_polyDepth->text().toInt();
    bool triangle =  (ui->lineEdit_polyDX->text() == "-")||
                     (ui->lineEdit_polyDY->text() == "-");
    pc_depth = poly_container(triangle?poly(AX,AY,BX,BY,CX,CY):poly(AX,AY,BX,BY,CX,CY,DX,DY));
    pc_depth.construct_image(img, depth);
    refresh_work_poly(&pc_depth);
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

void MainWindow::on_checkBox_gridonly_toggled(bool)
{
    refresh_work_poly(pc_work);;
}

void MainWindow::on_pushButton_compress_save_clicked()
{
    QString savfile;
    while(!savfile.length())
    {
       savfile = QFileDialog::getSaveFileName(this, "Сохранение изображения",
                                           "D:\\w10profile\\Desktop\\faxmepic",
                                           "PolyCompressed Image (*.pc);;All files (*)");
    }
    pc_work->compress();
    pc_work->save_compress(savfile);
}

void MainWindow::on_pushButton_compress_load_clicked()
{
    QString savfile;
    while(!savfile.length())
    {
       savfile = QFileDialog::getOpenFileName(this, "Открытие изображения",
                                           "D:\\w10profile\\Desktop\\faxmepic",
                                           "PolyCompressed Image (*.pc);;All files (*)");
    }
    pc_file.load_compress(savfile);
    pc_file.decompress();
    refresh_work_poly(&pc_file);
}

void MainWindow::on_pushButton_conv_clicked()
{
    for(int x = 0; x < 256; x++)
        for(int y = 0; y < 256; y++)
        {
            int g = qGray(img.pixel(x,y));
            img.setPixel(x,y,QColor::fromRgb(g,g,g).rgb());
        }
    ui->label_img_origin->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::on_pushButton_save_orig_bmp_clicked()
{
    QString savfile = QFileDialog::getSaveFileName(this, "Сохранение изображения",
                                           "",
                                           "Bitmap Image (*.bmp)");
    if(savfile.length()) img.save(savfile);
}

void MainWindow::on_pushButton_save_orig_jpg_clicked()
{
    QString savfile = QFileDialog::getSaveFileName(this, "Сохранение изображения",
                                           "",
                                           "JPEG Image (*.jpg)");
    if(savfile.length()) img.save(savfile);
}

void MainWindow::on_pushButton_save_work_bmp_clicked()
{
    QString savfile = QFileDialog::getSaveFileName(this, "Сохранение изображения",
                                           "",
                                           "Bitmap Image (*.bmp)");
    if(savfile.length())
        ui->labelxy_img_work->pixmap()->toImage().save(savfile);
}

void MainWindow::on_pushButton_save_work_jpg_clicked()
{
    QString savfile = QFileDialog::getSaveFileName(this, "Сохранение изображения",
                                           "",
                                           "JPEG Image (*.jpg)");
    if(savfile.length())
        ui->labelxy_img_work->pixmap()->toImage().save(savfile);
}

void MainWindow::on_pushButton_compress_save_render_clicked()
{
    QString savfile = QFileDialog::getSaveFileName(this, "Сохранение рендера",
                                           "",
                                           "PolyCompressed Render (*.pcr)");
    if(savfile.length())
        poly_container::save_compress_multi(savfile, pcvec);
}

void MainWindow::on_pushButton_compress_load_render_clicked()
{
    QString savfile = QFileDialog::getOpenFileName(this, "Загрузка рендера",
                                           "",
                                           "PolyCompressed Render (*.pcr)");
    if(savfile.length())
        pcvec = poly_container::load_compress_multi(savfile);
    refresh_work_poly(&pcvec[ui->horizontalSlider->value()]);
}
