// Copyright © 2024 Mykhailo Stetsiuk
// SPDX-License-Identifier: AFL-3.0

#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <QTcpServer>

#include "CartItemModel.h"

class Server : public QTcpServer {
    Q_OBJECT
    CartItemModel& cartModel;
public:
    Server(CartItemModel& cartModel);
    QList<QTcpSocket*> scanners;
    QTcpSocket* terminal;
signals:
    void itemCodeScanned(int code, double quantity);
    void finishTransaction(QString receiptUrl);
public slots:
    void onNewConnection();
    void connect_device();
    void disconnect_scanner();
    void parseScannedItem();
    void requestCardPayment(const double &amount) const;
    void cancelCardPayment() const;
    void processTerminalResponse();
    QString printCardReceipt(const QVariantMap& terminalData) const;
    QString printCashReceipt(double deposit, double change) const;
};

#endif //SERVER_H
