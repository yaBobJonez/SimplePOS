// Copyright © 2024 Mykhailo Stetsiuk
// SPDX-License-Identifier: AFL-3.0

#ifndef CARTITEM_H
#define CARTITEM_H

#include <QVariant>

class CartItem : public QObject {
    Q_OBJECT
    Q_PROPERTY(int code READ code WRITE setCode NOTIFY codeChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(double quantity READ quantity WRITE setQuantity NOTIFY quantityChanged)
    Q_PROPERTY(double price READ price WRITE setPrice NOTIFY priceChanged)
public:
    CartItem(QObject* parent = nullptr);
    CartItem(const CartItem &other);
    CartItem(int code, const QString &name, double quantity, double price, QObject *parent = nullptr);
    CartItem& operator=(const CartItem &other);
    void setCode(int code);
    void setName(const QString &name);
    void setQuantity(double quantity);
    void setPrice(double price);
    int code() const;
    QString name() const;
    double quantity() const;
    double price() const;
    QVariantMap toVariantMap() const;
signals:
    void codeChanged();
    void nameChanged();
    void quantityChanged();
    void priceChanged();
private:
    int m_code;
    QString m_name;
    double m_quantity;
    double m_price;
};

Q_DECLARE_METATYPE(CartItem)

#endif //CARTITEM_H
