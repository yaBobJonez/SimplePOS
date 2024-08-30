// Copyright © 2024 Mykhailo Stetsiuk
// SPDX-License-Identifier: AFL-3.0

#include "CartItem.h"

CartItem::CartItem(QObject* parent)
        : QObject(parent)
        , m_code(0), m_name("Item name"), m_price(0), m_quantity(1){}
CartItem::CartItem(const CartItem &other)
    : QObject(other.parent())
    , m_code(other.m_code), m_name(other.m_name), m_price(other.m_price), m_quantity(other.m_quantity){}
CartItem::CartItem(int code, const QString &name, double quantity, double price, QObject *parent)
    : QObject(parent)
    , m_code(code), m_name(name), m_price(price), m_quantity(quantity){}

CartItem& CartItem::operator=(const CartItem &other) {
    if (this != &other) {
        m_code = other.m_code;
        m_name = other.m_name;
        m_quantity = other.m_quantity;
        m_price = other.m_price;
    } return *this;
}

void CartItem::setCode(const int code) {
    if (code == m_code) return;
    m_code = code;
    emit codeChanged();
}
void CartItem::setName(const QString &name) {
    if (name == m_name) return;
    m_name = name;
    emit nameChanged();
}
void CartItem::setPrice(const double price) {
    if (price == m_price) return;
    m_price = price;
    emit priceChanged();
}
void CartItem::setQuantity(const double quantity) {
    if (quantity == m_quantity) return;
    m_quantity = quantity;
    emit quantityChanged();
}

int CartItem::code() const { return m_code; }
QString CartItem::name() const { return m_name; }
double CartItem::price() const { return m_price; }
double CartItem::quantity() const { return m_quantity; }

QVariantMap CartItem::toVariantMap() const {
    QVariantMap map;
    map["code"] = code();
    map["name"] = name();
    map["quantity"] = quantity();
    map["price"] = price();
    return map;
}
