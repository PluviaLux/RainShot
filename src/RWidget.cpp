#include "RWidget.h"
#include "qdebug.h"

#include <QMouseEvent>
#include <QPainter>
#include <QtMath>
#include <QScreen>
#include <QClipboard>
#include <QTimer>
#include <QApplication>

RWidget::RWidget(QHash<QScreen *, QImage> &images, QWidget *parent) :
    QWidget(parent, Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::Popup | Qt::WindowStaysOnTopHint),
    strokeColor(palette().highlight().color()),
    backgroundColor(QColor::fromRgbF(0, 0, 0, 0.30)),
    imgs(images) {

    setMouseTracking(true);
    setCursor(Qt::CrossCursor);
    setAttribute(Qt::WA_StaticContents);

    for (auto i = imgs.constBegin(); i != imgs.constEnd(); ++i) {
        const QScreen *screen = i.key();
        const QImage &screenImage = i.value();

        screens |= QRect(screen->geometry().topLeft(), screenImage.size());
    }

    scrPxm = QPixmap(screens.width(), screens.height());
    QPainter p(&scrPxm);
    for (auto it = imgs.constBegin(); it != imgs.constEnd(); ++it) {
        const QScreen *scr = it.key();
        const QImage &scrImg = it.value();

        p.drawImage(QPoint(scr->geometry().x(), scr->geometry().y()), scrImg);
    }

    setGeometry(screens);

    update();
}

void RWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() & Qt::LeftButton) {
        startPos = event->pos();
        active = true;
        update();
    }
}

void RWidget::mouseMoveEvent(QMouseEvent *event) {
    if (active) {
        mousePos = event->pos();

        bool underX = mousePos.x() >= startPos.x();
        bool underY = mousePos.y() >= startPos.y();

        sel.setRect(underX ? startPos.x() : mousePos.x(),
                underY ? startPos.y() : mousePos.y(),
                qAbs(mousePos.x() - startPos.x()),
                qAbs(mousePos.y() - startPos.y()));
    }

    update();

    event->accept();
}

void RWidget::mouseReleaseEvent(QMouseEvent *event) {
    switch (event->button()) {
    case Qt::LeftButton: {
        QPixmap cpy = scrPxm.copy(sel);
        cpy.save("test.png", "PNG");

        QApplication::clipboard()->setImage(cpy.toImage());
        break;
    }
    case Qt::RightButton:
        sel.setWidth(0);
        sel.setHeight(0);
        break;
    default:
        break;
    }

    hide();
    active = false;    
    event->accept();
    update();
    // TODO: we need an ecm 5.89.0 for kde clipboard manager implementation
    // because x11 disposes clipboard data after widget closing
    // QTimer::singleShot(300, this, &RWidget::complete);
}

void RWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    p.eraseRect(rect());

    for (auto it = imgs.constBegin(); it != imgs.constEnd(); ++it) {
        const QImage &scrImg = it.value();
        const QScreen *scr = it.key();

        QRect rect = scr->geometry().translated(-screens.topLeft());
        const qreal dpr = scrImg.width() / static_cast<qreal>(rect.width());
        const qreal dprI = 1.0 / dpr;

        QBrush brush(scrImg);
        brush.setTransform(QTransform::fromScale(dprI, dprI));

        rect.moveTopLeft(rect.topLeft());

        p.setBrushOrigin(rect.topLeft());
        p.fillRect(rect, brush);
    }

    if (!sel.size().isEmpty() && active) {
        QRectF innSel = sel.adjusted(1, 1, -1, -1);
        if (innSel.width() > 0 && innSel.height() > 0) {
            p.setPen(strokeColor);
            p.drawLine(sel.topLeft(), sel.topRight());
            p.drawLine(sel.bottomRight(), sel.topRight());
            p.drawLine(sel.bottomRight(), sel.bottomLeft());
            p.drawLine(sel.bottomLeft(), sel.topLeft());
        }

        QRectF top(0, 0, width(), sel.top());
        QRectF right(sel.right(), sel.top(), width() - sel.right(), sel.height());
        QRectF bottom(0, sel.bottom() + 1, width(), height() - sel.bottom());
        QRectF left(0, sel.top(), sel.left(), sel.height());

        p.fillRect(top, backgroundColor);
        p.fillRect(right, backgroundColor);
        p.fillRect(bottom, backgroundColor);
        p.fillRect(left, backgroundColor);
    } else {
        p.fillRect(rect(), backgroundColor);
    }
}
