#ifndef XYLABEL_H
#define XYLABEL_H

#include <QLabel>

namespace Ui {
class xyLabel;
}

class xyLabel : public QLabel
{
    Q_OBJECT
public:
    xyLabel( const QString & text, QWidget * parent = 0 );
    xyLabel( QWidget * parent = 0 );
    ~xyLabel(){}

signals:
    void xyLabel_clicked(xyLabel *clicked, QMouseEvent * event );

protected:
    void mousePressEvent ( QMouseEvent * event ) ;

};

#endif // XYLABEL_H
