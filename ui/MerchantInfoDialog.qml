// Copyright © 2024 Mykhailo Stetsiuk
// SPDX-License-Identifier: AFL-3.0

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQml.Models
import Qt.labs.qmlmodels
import QtQuick.Dialogs
import QtQuick.LocalStorage
import QtCore

Dialog {
    id: root
    modal: true
    title: qsTr("Merchant")
    standardButtons: Dialog.Ok

    Settings {
        id: settings
        category: "Merchant"
        property alias entityName: entityEdit.text
        property alias address: addressEdit.text
        property alias taxpayerID: taxpayerEdit.text
    }

    GridLayout {
        columns: 2
        Text { text: qsTr("Legal entity:") }
        TextField {
            id: entityEdit
            Layout.fillWidth: true
        }
        Text { text: qsTr("Address:") }
        TextArea {
            id: addressEdit
            Layout.fillWidth: true
        }
        Text { text: qsTr("Taxpayer info:") }
        TextField {
            id: taxpayerEdit
            Layout.fillWidth: true
        }
    }
}