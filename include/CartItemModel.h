// Copyright © 2024 Mykhailo Stetsiuk
// SPDX-License-Identifier: AFL-3.0

#ifndef CARTITEMMODEL_H
#define CARTITEMMODEL_H

#include <QAbstractTableModel>
#include <qjsvalue.h>

#include "CartItem.h"

class CartItemModel : public QAbstractTableModel {
    Q_OBJECT
    Q_PROPERTY(double totalPrice READ totalPrice NOTIFY totalPriceChanged)
public:
    CartItemModel(QObject* parent = nullptr);
    Q_INVOKABLE QVariantMap getRow(int index) const;
    Q_INVOKABLE void appendRow(const QVariantMap &row);
    Q_INVOKABLE void setRow(int index, const QVariantMap &row);
    Q_INVOKABLE bool removeRow(int index);
    Q_INVOKABLE void clear();
    Q_INVOKABLE int findIndex(const QJSValue &func) const;
    double totalPrice() const;
    int rowCount(const QModelIndex &parent) const override;
    const CartItem& operator[](int index) const;
signals:
    void totalPriceChanged();
protected:
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
private:
    QList<CartItem> m_cartItems;
    static CartItem fromQVariantMap(const QVariantMap &row);
};

#endif //CARTITEMMODEL_H
