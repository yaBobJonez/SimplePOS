// Copyright © 2024 Mykhailo Stetsiuk
// SPDX-License-Identifier: AFL-3.0

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQml.Models
import Qt.labs.qmlmodels
import QtQuick.Dialogs
import QtQuick.LocalStorage

ColumnLayout {
    id: root
    Layout.fillWidth: true
    Layout.fillHeight: true

    signal goBack()
    required property var db
    function reloadItems() {
        tableView.model.rows = [];
        root.db.transaction(tx => {
            let allRows = tx.executeSql('SELECT rowid, * FROM Items').rows;
            for (let i = 0; i < allRows.length; i++) {
                tableView.model.appendRow({
                    "id": allRows.item(i).rowid,
                    "code": allRows.item(i).code,
                    "name": allRows.item(i).name,
                    "price": allRows.item(i).price
                });
            }
        });
    }

    MerchantInfoDialog {
        id: merchantInfoDialog
        anchors.centerIn: parent
    }
    TaxConfigDialog {
        id: taxConfigDialog
        anchors.centerIn: parent
    }

    HorizontalHeaderView {
        syncView: tableView
        model: ["ID", "Code", "Name", "Price"]
        clip: true
    }
    TableView {
        id: tableView
        clip: true
        Layout.fillWidth: true
        Layout.fillHeight: true

        columnWidthProvider: column => (column === 0)? 0 : -1
        model: TableModel {
            TableModelColumn { display: "id" }
            TableModelColumn { display: "code" }
            TableModelColumn { display: "name" }
            TableModelColumn { display: "price" }
        }

        selectionModel: ItemSelectionModel {}

        delegate: Rectangle {
            implicitWidth: 100
            implicitHeight: 50
            border.width: current? 1 : 0
            color: selected? "lightblue" : "lightgray"

            required property bool selected
            required property bool current

            Text {
                anchors.centerIn: parent
                text: display
            }

            TableView.editDelegate: TextField {
                anchors.fill: parent
                text: display
                horizontalAlignment: TextInput.AlignHCenter
                verticalAlignment: TextInput.AlignVCenter
                Component.onCompleted: selectAll()
                TableView.onCommit: {
                    root.db.transaction(tx => {
                        tx.executeSql(`UPDATE Items
                            SET ${tableView.model.columns[column].display} = ?
                            WHERE rowid = ${tableView.model.rows[row].id}`,
                        text);
                    });
                    display = text;
                }
            }
        }

        SelectionRectangle {
            target: tableView
        }
        selectionBehavior: TableView.SelectRows
    }
    RowLayout {
        Layout.margins: 3
        Button {
            text: qsTr("Return to Checkout")
            onClicked: root.goBack()
        }
        ToolSeparator {}
        Button {
            text: qsTr("New item")
            onClicked: {
                root.db.transaction(tx => {
                    let itemId = tx.executeSql(`INSERT INTO Items (name, price)
                                                VALUES ("Item name", 0.01)`).insertId;
                    tableView.model.appendRow({
                        "id": itemId,
                        "code": 0,
                        "name": "Item name",
                        "price": 0.01
                    });
                });
            }
        }
        Button {
            text: qsTr("Delete selected")
            onClicked: {
                if (tableView.selectionModel.hasSelection) {
                    let indexes = [];
                    for (let i of tableView.selectionModel.selectedRows(0)) indexes.push(i.row);
                    indexes.sort().reverse();
                    root.db.transaction(tx => {
                        for (let i of indexes) {
                            tx.executeSql(`DELETE FROM Items
                                WHERE rowid = ${tableView.model.rows[i].id}`);
                            tableView.model.removeRow(i);
                        }
                    });
                }
            }
        }
        ToolSeparator {}
        Button {
            text: qsTr("Merchant info")
            onClicked: merchantInfoDialog.open()
        }
        Button {
            text: qsTr("Tax config")
            onClicked: taxConfigDialog.open()
        }
    }
}