#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>
#include <QQuickStyle>

#include "service/AuthService.h"
#include "service/MRService.h"
#include "service/AccountManager.h"
#include "model/ProviderListModel.h"


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
    MRService mrService;
    AccountManager accountManager;
    ProviderListModel providerListModel(&accountManager);
    AuthService authService(&accountManager);

    mrService.loadTestData();
    engine.rootContext()->setContextProperty("authService", &authService);
    engine.rootContext()->setContextProperty("providerModel", &providerListModel);
    engine.rootContext()->setContextProperty("mrService", &mrService);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("EasyHub", "Main");

    return app.exec();
}
