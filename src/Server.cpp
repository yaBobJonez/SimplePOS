// Copyright © 2024 Mykhailo Stetsiuk
// SPDX-License-Identifier: AFL-3.0

#include "Server.h"
#include "ReceiptPrinter.h"

#include <QSettings>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>


Server::Server(CartItemModel& cartModel) : cartModel(cartModel), terminal(nullptr) {
    if (!this->listen(QHostAddress::Any, 43000))
        std::cerr << "[ERROR] Cannot start server.";
    connect(this, &Server::newConnection, this, &Server::onNewConnection);
}

void Server::onNewConnection() {
    QTcpSocket* client = nextPendingConnection();
    client->write("Provide your device type.");
    connect(client, &QTcpSocket::readyRead, this, &Server::connect_device);
}

void Server::connect_device() {
    auto awaitingDevice = reinterpret_cast<QTcpSocket*>(sender());
    disconnect(awaitingDevice, &QTcpSocket::readyRead, nullptr, nullptr);
    QString res = awaitingDevice->readAll();
    if (res == "scanner") {
        QTcpSocket* scanner = awaitingDevice;
        connect(scanner, &QTcpSocket::readyRead, this, &Server::parseScannedItem);
        connect(scanner, &QTcpSocket::disconnected, this, &Server::disconnect_scanner);
        this->scanners.append(scanner);
    } else if (res == "terminal") {
        this->terminal = awaitingDevice;
        connect(this->terminal, &QTcpSocket::readyRead, this, &Server::processTerminalResponse);
    } else {
        awaitingDevice->write("Disconnected: device type not supported.");
        awaitingDevice->disconnectFromHost();
    }
}

void Server::disconnect_scanner() {
    auto scanner = reinterpret_cast<QTcpSocket*>(sender());
    this->scanners.removeOne(scanner);
    scanner->deleteLater();
}

void Server::parseScannedItem() {
    auto scanner = reinterpret_cast<QTcpSocket*>(sender());
    QJsonObject data = QJsonDocument::fromJson(scanner->readAll()).object();
    const int code = data["code"].toInt();
    const double quantity = data["quantity"].toDouble();
    emit itemCodeScanned(code, quantity);
}

void Server::requestCardPayment(const double &amount) const {
    if (!this->terminal) return;
    QJsonObject data;
    data["operation"] = "charge";
    data["amount"] = amount;
    this->terminal->write(QJsonDocument(data).toJson(QJsonDocument::Compact));
}

void Server::cancelCardPayment() const {
    if (!this->terminal) return;
    QJsonObject data;
    data["operation"] = "cancel";
    this->terminal->write(QJsonDocument(data).toJson(QJsonDocument::Compact));
}

void Server::processTerminalResponse() {
    QJsonObject data = QJsonDocument::fromJson(this->terminal->readAll()).object();
    if (data["status"].toString() == "success") {
        const QString receiptUrl = printCardReceipt(data.toVariantMap());
        emit finishTransaction(receiptUrl);
    }
}

QString Server::printCardReceipt(const QVariantMap& terminalData) const {
    const QString cardNumber = terminalData.value("cardNumber").toString();
    const QString cardType = terminalData.value("cardType").toString();
    const QString approvalCode = terminalData.value("approvalCode").toString();

    ReceiptPrinter gc;
    gc.setOutputFormat(QPrinter::PdfFormat);
    gc.setOutputFileName("/home/michael/testWriter.pdf");
    gc.printHeader();
    gc.printCartItems(cartModel);
    gc.printField(tr("Terminal ID"), "#00001");
    gc.printText(tr("Purchase"));
    gc.printField(tr("EPS"), cardNumber);
    gc.printField(tr("Payment system"), cardType);
    gc.printField(tr("Approval code"), approvalCode);
    gc.advanceLine();
    gc.printSum(cartModel.totalPrice());
    gc.printFooter();

    return "file:///home/michael/testWriter.pdf";
}

QString Server::printCashReceipt(const double deposit, const double change) const {
    ReceiptPrinter gc;
    gc.setOutputFormat(QPrinter::PdfFormat);
    gc.setOutputFileName("/home/michael/testWriter.pdf");
    gc.printHeader();
    gc.printCartItems(cartModel);
    gc.printText(tr("Purchase"));
    gc.printField(tr("Cash"), deposit);
    gc.printField(tr("Change"), change);
    gc.advanceLine();
    gc.printSum(cartModel.totalPrice());
    gc.printFooter();
    return "file:///home/michael/testWriter.pdf";
}
