// Copyright © 2024 Mykhailo Stetsiuk
// SPDX-License-Identifier: AFL-3.0

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQml.Models
import Qt.labs.qmlmodels
import QtQuick.Dialogs
import QtQuick.LocalStorage
import QtQuick.Pdf

ColumnLayout {
    id: root
    Layout.fillWidth: true
    Layout.fillHeight: true

    property var status: "await"
    signal goBack()
    Connections {
        target: server
        function onFinishTransaction(receiptUrl){
            var timestamp = new Date().getTime();
            preview.document.source = receiptUrl + "?timestamp=" + timestamp;
            root.status = "success";
        }
    }

    RowLayout {
        ColumnLayout {
            Layout.margins: 5
            Text {
                id: statusLabel
                Layout.fillWidth: true
                Layout.fillHeight: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: root.status === "await" ? qsTr("Awaiting for payment…") : qsTr("√ Success")
                color: root.status === "await" ? "black" : "darkgreen"
                font.pointSize: Qt.application.font.pointSize * 2
            }
            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                Button {
                    text: qsTr("Cancel")
                    visible: root.status === "await"
                    onClicked: {
                        server.cancelCardPayment();
                        root.goBack();
                    }
                }
                Button {
                    text: qsTr("New transaction")
                    visible: root.status === "success"
                    onClicked: {
                        root.status = "await";
                        root.goBack()
                    }
                }
            }
        }
        ColumnLayout {
            Layout.margins: 5
            RowLayout {
                ToolButton {
                    text: qsTr("Zoom In")
                    onClicked: preview.renderScale *= 1.1;
                }
                ToolButton {
                    text: qsTr("Zoom Out")
                    onClicked: preview.renderScale /= 1.1;
                }
            }
            PdfMultiPageView {
                id: preview
                Layout.margins: 10
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                document: PdfDocument {}
            }
        }
    }
}