// Copyright © 2024 Mykhailo Stetsiuk
// SPDX-License-Identifier: AFL-3.0

#include <iostream>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QTcpSocket>
#include <QQmlContext>
#include <QJsonObject>
#include <QSettings>

#include "include/CartItemModel.h"
#include "include/Server.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setOrganizationName("yaBobJonez");
    app.setApplicationName("SimplePOS");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/qt/qml/app_SimplePOS/ui/Main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    CartItemModel cartItemModel;
    engine.rootContext()->setContextProperty("cartItemModel", &cartItemModel);
    Server server(cartItemModel);
    engine.rootContext()->setContextProperty("server", &server);
    engine.load(url);

    return app.exec();
}
