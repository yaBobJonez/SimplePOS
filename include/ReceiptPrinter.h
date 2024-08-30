// Copyright © 2024 Mykhailo Stetsiuk
// SPDX-License-Identifier: AFL-3.0

#ifndef RECEIPTPRINTER_H
#define RECEIPTPRINTER_H

#include <QPrinter>
#include <QPainter>
#include <QDateTime>

#include "CartItemModel.h"

class ReceiptPrinter : public QPrinter {
public:
    ReceiptPrinter();
    bool begin();
    bool end();
    void printHeader();
    void printCartItems(const CartItemModel& cartModel);
    void printSum(double total);
    void printFooter();
    void printItem(const QString& name, double quantity, double price);
    void printField(const QString& key, const QString& value);
    void printField(const QString& key, double value);
    void printText(const QString& text, int flags = Qt::AlignCenter);
    void advanceLine();
protected:
    QPainter gc;
    int y;
    int pageWidth;
    int pageHeight;
    int lineHeight;
};

inline bool ReceiptPrinter::end() {
    return gc.end();
}

inline void ReceiptPrinter::printFooter() {
    printText(QDateTime::currentDateTime().toString(Qt::ISODate));
    this->end();
}

#endif //RECEIPTPRINTER_H
