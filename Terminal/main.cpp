// Copyright © 2024 Mykhailo Stetsiuk
// SPDX-License-Identifier: AFL-3.0

#include <iostream>
#include <QQmlApplicationEngine>
#include <QTcpSocket>
#include <QQuickView>
#include <QQmlComponent>
#include <QQmlContext>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QRandomGenerator>

class Client : public QTcpSocket
{
    Q_OBJECT
    QObject* root;
public:
    Client(QObject* root) : root(root){}
signals:
    void requestPayment(double amount);
    void cancelPaymentRequest();
public slots:
    void connectToPOS() {
        this->connectToHost(QHostAddress::LocalHost, 43000);
        connect(this, &QTcpSocket::connected, [&] {
            this->write("terminal");
            root->setProperty("isConnected", QVariant(true));
            connect(this, &QTcpSocket::readyRead, this, &Client::process_request);
        });
        connect(this, &QTcpSocket::disconnected, [&] {
            root->setProperty("isConnected", QVariant(false));
        });
    }
    void process_request() {
        QJsonObject data = QJsonDocument::fromJson(readAll()).object();
        QString op = data["operation"].toString();
        if (op == "charge") {
            double amount = data["amount"].toDouble();
            emit requestPayment(amount);
        } else if (op == "cancel") {
            emit cancelPaymentRequest();
        }
    }
    void send_response(const QString &reply) {
        QJsonObject data;
        data["status"] = reply;
        data["cardType"] = "Visa Credit";
        const QString cardNumberRandPart = QString::number(QRandomGenerator::global()->bounded(1000, 10000));
        data["cardNumber"] = QString("XXXXXXXXXXXX").append(cardNumberRandPart);
        data["approvalCode"] = QRandomGenerator::global()->bounded(10000, 100000);
        this->write(QJsonDocument(data).toJson(QJsonDocument::Compact));
    }
};

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    const QUrl url(QStringLiteral("qrc:/qt/qml/testTerminal/Main.qml"));
    QQmlEngine engine;
    QQmlComponent component(&engine, url);
    QObject* root = component.create();

    Client client(root);
    engine.rootContext()->setContextProperty("client", &client);

    return app.exec();
}

#include "main.moc"
