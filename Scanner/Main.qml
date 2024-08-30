// Copyright © 2024 Mykhailo Stetsiuk
// SPDX-License-Identifier: AFL-3.0

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    id: root
    signal requestConnectionToPOS()
    signal sendBarcodeToPOS(itemId: int, quantity: double)
    property bool isConnected: false

    width: 256
    height: 256
    visible: true
    title: qsTr("Example scanner")
    ColumnLayout {
        anchors.fill: parent
        Image {
            Layout.fillWidth: true
            Layout.fillHeight: true
            source: "scanner.png"
            fillMode: Image.PreserveAspectFit
        }
        RowLayout {
            Button {
                Layout.fillWidth: true
                text: qsTr("Connect")
                onClicked: root.requestConnectionToPOS()
                visible: ! root.isConnected
            }
            TextField {
                id: barcodeEdit
                Layout.fillWidth: true
                inputMethodHints: Qt.ImhDigitsOnly
                placeholderText: qsTr("Item ID")
                visible: root.isConnected
            }
            TextField {
                id: quantityEdit
                Layout.fillWidth: true
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                placeholderText: qsTr("Quantity")
                visible: root.isConnected
            }
            Button {
                text: qsTr("Scan")
                onClicked: root.sendBarcodeToPOS(barcodeEdit.text, quantityEdit.text)
                visible: root.isConnected
            }
        }
    }
}
