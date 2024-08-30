// Copyright © 2024 Mykhailo Stetsiuk
// SPDX-License-Identifier: AFL-3.0

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQml.Models
import Qt.labs.qmlmodels
import QtQuick.Dialogs
import QtQuick.LocalStorage

RowLayout {
    id: root
    Layout.fillWidth: true
    Layout.fillHeight: true

    signal goBack()
    signal proceedToSummary()
    signal requestCardPayment(amount: double)
    required property var db
    Connections {
        target: server
        function onItemCodeScanned(code, quantity) {
            root.addItemByCode(code, quantity);
        }
        function onFinishTransaction(receiptUrl){
            myTable.model.clear();
            payBtn.visible = true
            paymentTypeBtns.visible = false
            cashDepositEdit.text = ""
        }
    }

    function addItemByCode(code: int, quantity: double) {
        let item = myGrid.model.model.find(o => o.code === code);
        if (!item) noItemWithIdPopup.popup(code);
        else {
            let inCartIndex = myTable.model.findIndex(o => o.code === code);
            if (inCartIndex === -1) {
                item.quantity = quantity;
                myTable.model.appendRow(item);
            } else {
                item = myTable.model.getRow(inCartIndex);
                item.quantity += quantity;
                myTable.model.setRow(inCartIndex, item);
            }
        }
    }
    function reloadItems(){
        myGrid.model.model = [];
        root.db.transaction(tx => {
            let allRows = tx.executeSql('SELECT * FROM Items').rows;
            for (let i = 0; i < allRows.length; i++) {
                myGrid.model.model.push({
                    "code": allRows.item(i).code,
                    "name": allRows.item(i).name,
                    "price": allRows.item(i).price
                });
            }
        });
    }

    Popup {
        id: noItemWithIdPopup
        property string itemId
        function popup(itemId) {
            noItemWithIdPopup.itemId = itemId;
            open();
        }
        anchors.centerIn: parent
        padding: 5
        contentItem: Text {
            text: qsTr("No item with ID %1 exists.").arg(noItemWithIdPopup.itemId)
        }
    }
    Dialog {
        id: cashPaymentDialog
        anchors.centerIn: parent
        modal: true
        title: qsTr("Cash payment")
        standardButtons: Dialog.Ok | Dialog.Cancel
        Component.onCompleted: {
            cashPaymentDialog.standardButton(Dialog.Ok).enabled = false;
        }
        GridLayout {
            columns: 2
            Text { text: qsTr("Deposit:") }
            TextField {
                id: cashDepositEdit
                Layout.fillWidth: true
            }
            Text { text: qsTr("Change:") }
            Text {
                id: cashChangeLabel
                Layout.fillWidth: true
                text: (+cashDepositEdit.text - +totalDue.text).toFixed(2)
                onTextChanged: cashPaymentDialog.standardButton(Dialog.Ok).enabled = (+cashChangeLabel.text >= 0)
            }
        }
        onAccepted: {
            let receiptUrl = server.printCashReceipt(+cashDepositEdit.text, +cashChangeLabel.text);
            server.finishTransaction(receiptUrl);
            root.proceedToSummary();
        }
    }
    ColumnLayout {
        Layout.fillWidth: true
        Layout.preferredWidth: 2
        RowLayout {
            Layout.margins: 3
            Button {
                icon.name: "document-edit"
                onClicked: root.goBack()
            }
            TextField {
                Layout.fillWidth: true
                placeholderText: qsTr("Search")
                onTextChanged: {
                    let items = myGrid.model.items;
                    for (let i = 0; i < items.count; i++) {
                        let item = items.get(i);
                        item.inVisible = item.model.modelData.name.includes(text);
                    }
                }
            }
            ToolSeparator {}
            TextField {
                id: addByIdEdit
                Layout.fillWidth: true
                inputMethodHints: Qt.ImhDigitsOnly
                placeholderText: qsTr("Item ID")
            }
            Button {
                text: qsTr("Add Item")
                onClicked: {
                    if (!addByIdEdit.text) return;
                    root.addItemByCode(parseInt(addByIdEdit.text), 1.0);
                }
            }
        }
        GridView {
            id: myGrid
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.margins: 3
            cellWidth: 85
            cellHeight: 85
            model: DelegateModel {
                groups: DelegateModelGroup {
                    name: "visible"
                    includeByDefault: true
                }
                filterOnGroup: "visible"
                delegate: Rectangle {
                    implicitWidth: 80
                    implicitHeight: 80
                    radius: 5
                    color: "lightblue"
                    Text {
                        anchors.fill: parent
                        text: modelData.name
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        wrapMode: Text.Wrap
                        elide: Text.ElideRight
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: root.addItemByCode(modelData.code, 1.0)
                    }
                }
            }

        }
    }
    ColumnLayout {
        Layout.fillWidth: true
        Layout.preferredWidth: 1
        HorizontalHeaderView {
            syncView: myTable
            clip: true
        }
        TableView {
            id: myTable
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.maximumWidth: parent.width
            clip: true
            columnSpacing: 1
            rowSpacing: 1
            model: cartItemModel
            selectionModel: ItemSelectionModel {}
            selectionBehavior: TableView.SelectRows
            delegate: Rectangle {
                implicitWidth: 100//myTable.width / myTable.columns
                implicitHeight: 30
                color: selected ? "lightblue" : "lightgray"

                required property bool selected

                Text {
                    anchors.centerIn: parent
                    text: display
                }
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.RightButton
                    onClicked: {
                        myTable.selectionModel.select(myTable.model.index(row, column),
                            ItemSelectionModel.ClearAndSelect | ItemSelectionModel.Rows);
                        receiptContextMenu.popup();
                    }
                }
            }
            Menu {
                id: receiptContextMenu
                function deleteFromCart(quantity) {
                    if (! myTable.selectionModel.hasSelection) return;
                    let row = myTable.selectionModel.selectedIndexes[0].row;
                    let item = myTable.model.getRow(row);
                    if (item.quantity <= quantity)
                        myTable.model.removeRow(row);
                    else {
                        item.quantity -= quantity;
                        myTable.model.setRow(row, item);
                    }
                }
                MenuItem {
                    text: qsTr("Delete 1")
                    onTriggered: receiptContextMenu.deleteFromCart(1);
                }
                MenuItem {
                    text: qsTr("Delete 5")
                    onTriggered: receiptContextMenu.deleteFromCart(5);
                }
                MenuItem {
                    text: qsTr("Delete 20")
                    onTriggered: receiptContextMenu.deleteFromCart(20);
                }
                MenuItem {
                    text: qsTr("Delete all")
                    onTriggered: receiptContextMenu.deleteFromCart(Infinity);
                }
            }
        }
        GridLayout {
            columns: 2
            Layout.margins: 5
            Layout.fillWidth: true
            Text {
                Layout.fillWidth: true
                text: qsTr("Subtotal")
            }
            Text {
                text: (totalDue.text - taxDue.text).toFixed(2)
            }
            Text {
                Layout.fillWidth: true
                text: qsTr("Tax")
            }
            Text {
                id: taxDue
                text: (totalDue.text * 0.2).toFixed(2)
            }
            Text {
                Layout.fillWidth: true
                text: "<b>" + qsTr("Total") + "</b>"
            }
            Text {
                id: totalDue
                text: myTable.model.totalPrice.toFixed(2)
            }
        }
        Button {
            id: payBtn
            Layout.fillWidth: true
            text: "Gib me ur money"
            background: Rectangle {
                color: payBtn.down? "green" : "lightgreen"
            }
            onClicked: {
                paymentTypeBtns.visible = true
                payBtn.visible = false
            }
        }
        RowLayout {
            id: paymentTypeBtns
            Layout.fillWidth: true
            visible: false
            Button {
                id: backPTBtn
                Layout.fillWidth: true
                Layout.preferredWidth: 1
                text: "Back"
                background: Rectangle {
                    color: backPTBtn.down? "lightblue" : "lightgray"
                }
                onClicked: {
                    payBtn.visible = true
                    paymentTypeBtns.visible = false
                }
            }
            Button {
                id: cashPTBtn
                Layout.fillWidth: true
                Layout.preferredWidth: 2
                text: "Cash"
                background: Rectangle {
                    color: cashPTBtn.down? "orange" : "yellow"
                }
                onClicked: cashPaymentDialog.open();
            }
            Button {
                id: cardPTBtn
                Layout.fillWidth: true
                Layout.preferredWidth: 2
                text: "Card"
                background: Rectangle {
                    color: cardPTBtn.down? "orange" : "yellow"
                }
                onClicked: {
                    server.requestCardPayment(totalDue.text);
                    root.proceedToSummary();
                }
            }
        }
    }
}