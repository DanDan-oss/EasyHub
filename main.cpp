#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>
#include <QQuickStyle>

#include "service/AuthService.h"

int main(int argc, char *argv[])
{
    QQuickStyle::setStyle("Basic");
    QGuiApplication app(argc, argv);

    // 加载中文翻译
    QTranslator translator;
    if(translator.load(":/i18n/EasyHub_zh_CN.qm"))
    {
        app.installTranslator(&translator);
        qDebug() << "Chinese translator load Success.";
    }else
    {
        qDebug() << "Chinese translator load Failed.";
    }

    QQmlApplicationEngine engine;

    AuthService authService;
    engine.rootContext()->setContextProperty("authService", &authService);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("EasyHub", "Main");

    return app.exec();
}
