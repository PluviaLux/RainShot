#ifndef RWIDGET_H
#define RWIDGET_H

#include <QWidget>
#include <QHash>

class QMouseEvent;
class QPainter;

class RWidget : public QWidget {
    Q_OBJECT

public:
    explicit RWidget(QHash<QScreen *, QImage> &images, QWidget *parent = nullptr);

    Q_SIGNAL void complete();

private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *) override;

    QColor backgroundColor;
    QColor strokeColor;
    QPoint mousePos;
    QPoint startPos;
    QRect sel;
    QRect screens;
    QPixmap scrPxm;
    QHash<QScreen *, QImage> imgs;
    bool active = false;
};
#endif // RWIDGET_H
