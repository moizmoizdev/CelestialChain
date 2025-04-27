#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char **argv) {
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    // Loads qml/App.qml from the resource system
    engine.load(QUrl(QStringLiteral("qrc:/qml/App.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
