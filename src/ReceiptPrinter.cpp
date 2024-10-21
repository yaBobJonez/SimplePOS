// Copyright © 2024 Mykhailo Stetsiuk
// SPDX-License-Identifier: AFL-3.0

#include "ReceiptPrinter.h"

#include <iostream>
#include <QPrinterInfo>
#include <QSettings>
#include <QDir>
#include <QPrintDialog>

ReceiptPrinter::ReceiptPrinter()
    : y(10)
    , pageWidth(pageRect(DevicePixel).width())
    , pageHeight(pageRect(DevicePixel).height()) {}

bool ReceiptPrinter::begin() {
    if (QPrinterInfo::availablePrinterNames().empty()) {
        pageWidth = 57.5 / 25.4 * resolution();
        this->setPageSize(QPageSize(QSize(pageWidth, pageRect(Point).height())));
        this->setOutputFormat(PdfFormat);
        const QString dt = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
        if(!QDir().mkpath(QObject::tr("Receipts"))) return false;
        this->filename = QDir(QObject::tr("Receipts")).absoluteFilePath(dt + ".pdf");
        this->setOutputFileName(filename);
    } else {
        this->setOutputFormat(NativeFormat);
        QPrintDialog printDialog(this);
        if (printDialog.exec() != QPrintDialog::Accepted) return false;
    }
    if(!gc.begin(this)) return false;
    lineHeight = gc.fontMetrics().height();
    return true;
}

void ReceiptPrinter::printHeader() {
    QSettings settings;
    settings.beginGroup("Merchant");
    const QString entityName = settings.value("entityName").toString();
    const QStringList address = settings.value("address").toString().split('\n');
    const QString taxpayerID = settings.value("taxpayerID").toString();
    settings.endGroup();
    printText(entityName);
    for (const QString& line : address) printText(line);
    printText(taxpayerID);
    advanceLine();
}

void ReceiptPrinter::printCartItems(const CartItemModel &cartModel) {
    for (int i = 0; i < cartModel.rowCount(QModelIndex()); i++) {
        const CartItem& item = cartModel[i];
        printItem(item.name(), item.quantity(), item.price());
    } advanceLine();
}

void ReceiptPrinter::printSum(double totalDue) {
    QSettings settings;
    settings.beginGroup("Taxation");
    const double vat = settings.value("vat").toDouble();
    settings.endGroup();
    printField(QObject::tr("Total"), totalDue);
    printField(QObject::tr("VAT %1%").arg(vat), totalDue * vat/100);
    advanceLine();
}

void ReceiptPrinter::printItem(const QString &name, const double quantity, const double price) {
    gc.drawText(0, y, pageWidth, lineHeight, Qt::AlignLeft, name);
    gc.drawText(0.7 * pageWidth, y, 0.3 * pageWidth, lineHeight, Qt::AlignLeft,
        QString::number(quantity, 'g', 3));
    gc.drawText(0, y, pageWidth, lineHeight, Qt::AlignRight,
        QString::number(price, 'f', 2));
    advanceLine();
}

void ReceiptPrinter::printField(const QString &key, const QString &value) {
    gc.drawText(0, y, pageWidth, lineHeight, Qt::AlignLeft, key);
    gc.drawText(0, y, pageWidth, lineHeight, Qt::AlignRight, value);
    advanceLine();
}

void ReceiptPrinter::printField(const QString &key, const double value) {
    gc.drawText(0, y, pageWidth, lineHeight, Qt::AlignLeft, key);
    gc.drawText(0, y, pageWidth, lineHeight, Qt::AlignRight,
        QString::number(value, 'f', 2));
    advanceLine();
}

void ReceiptPrinter::printText(const QString &text, const int flags) {
    gc.drawText(0, y, pageWidth, lineHeight, flags, text);
    advanceLine();
}

void ReceiptPrinter::advanceLine() {
    if (y >= pageHeight) {
        this->newPage();
        y = 10;
    } else y += lineHeight;
}
