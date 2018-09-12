#include <QApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    qDebug()<<engine.importPathList();
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}

