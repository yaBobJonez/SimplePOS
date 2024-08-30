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
    title: qsTr("Taxation")
    standardButtons: Dialog.Ok

    Settings {
        id: settings
        category: "Taxation"
        property alias vat: vatEdit.text
    }

    GridLayout {
        columns: 2
        Text { text: qsTr("VAT:") }
        TextField {
            id: vatEdit
            Layout.fillWidth: true
            inputMethodHints: Qt.ImhFormattedNumbersOnly
        }
    }
}