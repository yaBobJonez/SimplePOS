// Copyright © 2024 Mykhailo Stetsiuk
// SPDX-License-Identifier: AFL-3.0

#include "CartItemModel.h"

#include <iostream>
#include <qjsengine.h>

CartItemModel::CartItemModel(QObject *parent)
    : QAbstractTableModel(parent){}

QVariantMap CartItemModel::getRow(const int index) const {
    return m_cartItems.at(index).toVariantMap();
}

void CartItemModel::appendRow(const QVariantMap& row) {
    beginInsertRows(QModelIndex(), m_cartItems.size(), m_cartItems.size());
    m_cartItems.append(fromQVariantMap(row));
    endInsertRows();
    emit totalPriceChanged();
}

void CartItemModel::setRow(const int index, const QVariantMap &row) {
    m_cartItems.replace(index, fromQVariantMap(row));
    emit dataChanged(this->index(index, 0), this->index(index, 2));
    emit totalPriceChanged();
}

bool CartItemModel::removeRow(const int index) {
    if (index < 0 || index >= m_cartItems.size()) return false;
    beginRemoveRows(QModelIndex(), index, index);
    m_cartItems.removeAt(index);
    endRemoveRows();
    emit totalPriceChanged();
    return true;
}

void CartItemModel::clear() {
    beginResetModel();
    m_cartItems.clear();
    endResetModel();
    emit totalPriceChanged();
}

int CartItemModel::findIndex(const QJSValue &func) const {
    if (!func.isCallable()) return -1;
    QJSEngine* engine = qjsEngine(this);
    for (int i = 0; i < m_cartItems.size(); i++) {
        QJSValue object = engine->toScriptValue(m_cartItems.at(i).toVariantMap());
        QJSValue result = func.call(QJSValueList{ object });
        if (result.isBool() && result.toBool()) return i;
    } return -1;
}

double CartItemModel::totalPrice() const {
    double sum = 0.0;
    for (const CartItem& item : m_cartItems)
        sum += item.price() * item.quantity();
    return sum;
}

int CartItemModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return m_cartItems.size();
}

const CartItem& CartItemModel::operator[](const int index) const {
    return m_cartItems.at(index);
}

int CartItemModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return 3;
}

QVariant CartItemModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) return {};
    const CartItem &item = m_cartItems.at(index.row());
    switch (index.column()) {
        case 0: return item.name();
        case 1: return QString::number(item.quantity(), 'f', 3);
        case 2: return QString::number(item.price() * item.quantity(), 'f', 2);
    } return {};
}

QVariant CartItemModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole) return {};
    if (orientation == Qt::Horizontal) switch (section) {
        case 0: return QString("Name");
        case 1: return QString("Qty");
        case 2: return QString("Price");
    } else if (Qt::Vertical) return section + 1;
    return {};
}

CartItem CartItemModel::fromQVariantMap(const QVariantMap &row) {
    int code = row["code"].toInt();
    QString name = row["name"].toString();
    double quantity = row["quantity"].toDouble();
    double price = row["price"].toDouble();
    return {code, name, quantity, price};
}
