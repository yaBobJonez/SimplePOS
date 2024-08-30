// Copyright © 2024 Mykhailo Stetsiuk
// SPDX-License-Identifier: AFL-3.0

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQml.Models
import Qt.labs.qmlmodels
import QtQuick.Dialogs
import QtQuick.LocalStorage

Window {
    id: root
    property var db
    width: 640
    height: 480
    visible: true
    title: qsTr("SimplePOS")
    Component.onCompleted: {
        root.db = LocalStorage.openDatabaseSync("POSItemsDB", "1.0", "TestPOS Items Database", 1000000);
        root.db.transaction(tx => {
            tx.executeSql(`CREATE TABLE IF NOT EXISTS Items (
                code INTEGER UNIQUE,
                name TEXT NOT NULL,
                price REAL NOT NULL )`);
        });
        itemsDatabasePage.reloadItems();
        checkoutPage.reloadItems();
    }
    StackLayout {
        id: pager
        anchors.fill: parent
        onCurrentIndexChanged: {
            itemsDatabasePage.reloadItems();
            checkoutPage.reloadItems();
        }
        Checkout {
            id: checkoutPage
            db: root.db
            onGoBack: pager.currentIndex = 2
            onProceedToSummary: pager.currentIndex = 1;
        }
        CheckoutSummary {
            id: summaryPage
            onGoBack: pager.currentIndex = 0
        }
        ItemsDatabase {
            id: itemsDatabasePage
            db: root.db
            onGoBack: pager.currentIndex = 0
        }
    }
}
