#include <RWidget.h>

#include <QApplication>
#include <QScreen>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    auto screens = QGuiApplication::screens();
    QHash<QScreen *, QImage> imgs;
    for (QScreen *screen : screens) {
        QPixmap pxm = screen->grabWindow(0);

        imgs.insert(screen, pxm.toImage());
    }

    RWidget w(imgs);
    QObject::connect(&w, &RWidget::complete, &app, &QCoreApplication::quit, Qt::QueuedConnection);
    w.show();

    return app.exec();
}
