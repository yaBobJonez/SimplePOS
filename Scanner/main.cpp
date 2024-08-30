// Copyright © 2024 Mykhailo Stetsiuk
// SPDX-License-Identifier: AFL-3.0

#include <iostream>
#include <QQmlApplicationEngine>
#include <QTcpSocket>
#include <QQuickView>
#include <QQmlComponent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

class Client : public QTcpSocket
{
    Q_OBJECT
    QObject* root;
public:
    Client(QObject* root) : root(root) {
        connect(this, &QTcpSocket::connected, [&] {
            this->write("scanner");
            this->root->setProperty("isConnected", QVariant(true));
        });
        connect(this, &QTcpSocket::disconnected, [&] {
            this->root->setProperty("isConnected", QVariant(false));
        });
        connect(this->root, SIGNAL(sendBarcodeToPOS(int, double)), this, SLOT(sendBarcodeToPOS(int, double)));
        connect(this->root, SIGNAL(requestConnectionToPOS()), this, SLOT(tryToConnectToPOS()));
    }
public slots:
    void tryToConnectToPOS() {
        this->connectToHost(QHostAddress::LocalHost, 43000);
    }
    void sendBarcodeToPOS(const int &code, const double &quantity) {
        QJsonObject data;
        data["code"] = code;
        data["quantity"] = quantity;
        this->write(QJsonDocument(data).toJson(QJsonDocument::Compact));
    }
};

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    const QUrl url(QStringLiteral("qrc:/qt/qml/testScanner/Main.qml"));
    QQmlEngine engine;
    QQmlComponent component(&engine, url);
    QObject* root = component.create();

    Client client(root);

    return app.exec();
}

#include "main.moc"
