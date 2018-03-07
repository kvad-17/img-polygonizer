#ifndef STASHCODE_H
#define STASHCODE_H

#endif // STASHCODE_H

void MainWindow::on_pushButton_checkall_clicked()
{
    QImage i(257,257, QImage::Format_RGB32);
    i.fill(QColor("white"));

    for(int t = 0; t < 256; t++)
        for(int m = 0, n = t; m <= t; m++, n--)
        {
            poly x = polvec[ui->horizontalSlider->value()].getLowestPolyOnPix(pix(m, n));
            x.print(i);
            x = polvec[ui->horizontalSlider->value()].getLowestPolyOnPix(pix(255-m, 255-n));
            x.print(i);
            ui->outLabel->setPixmap(QPixmap::fromImage(i));
            QApplication::processEvents();
        }


    ui->outLabel->setPixmap(QPixmap::fromImage(i));
}

void MainWindow::on_pushButton_stackprint_clicked()
{
    QImage i(257,257, QImage::Format_RGB32);
    i.fill(QColor("white"));

    QVector<poly*> vp = polvec[ui->horizontalSlider->value()].getLowestPolyVec();
    foreach(poly *p, vp)
    {
        p->print(i, true);
        ui->outLabel_stack->setPixmap(QPixmap::fromImage(i));
        QApplication::processEvents();
    }
}

void MainWindow::on_pushButton_compress_show_clicked()
{
    QImage i(257,257, QImage::Format_RGB32);
    i.fill(QColor("white"));
    PC.decompress()->print(i, true);
    ui->label_xy->setPixmap(QPixmap::fromImage(i));
}
