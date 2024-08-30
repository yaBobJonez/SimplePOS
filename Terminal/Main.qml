// Copyright © 2024 Mykhailo Stetsiuk
// SPDX-License-Identifier: AFL-3.0

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    id: root

    property bool isConnected: false
    property var awaitingPayment: false
    Connections {
        target: client
        function onRequestPayment(amount) {
            root.awaitingPayment = true;
            statusLabel.text = qsTr("Pay %1").arg(amount.toFixed(2));
        }
        function onCancelPaymentRequest() {
            if (root.awaitingPayment) {
                root.awaitingPayment = false;
                statusLabel.text = "";
            }
        }
    }
    Timer {
        id: timer
        function delay(delayTime, cb) {
            timer.interval = delayTime;
            timer.repeat = false;
            timer.triggered.connect(function timerRelease() {
                cb();
                timer.triggered.disconnect(timerRelease);
            });
            timer.start();
        }
    }

    width: 256
    height: 480
    visible: true
    title: qsTr("Example terminal")
    ColumnLayout {
        anchors.fill: parent
        Image {
            Layout.fillWidth: true
            Layout.fillHeight: true
            source: "terminal.jpg"
            fillMode: Image.PreserveAspectFit
            Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.verticalCenter
                width: parent.paintedWidth * 0.7
                height: parent.paintedHeight / 3
                color: root.isConnected? "white" : "black"
                DropArea {
                    id: dropArea
                    anchors.fill: parent
                    onEntered: function(drag) {
                        if (!root.awaitingPayment) return;
                        statusLabel.text = qsTr("Processing…");
                        timer.delay(2000, () => {
                            statusLabel.text = dropArea.containsDrag? qsTr("(√) Approved") : qsTr("(×) Try again");
                            client.send_response(dropArea.containsDrag? "success" : "failure");
                            root.awaitingPayment = false;
                            timer.delay(3000, () => {
                                statusLabel.text = "";
                            });
                        });
                    }
                }
                Text {
                    id: statusLabel
                    anchors.centerIn: parent
                    text: ""
                }
            }
            Image {
                y: parent.height - height
                width: parent.paintedWidth * 0.8
                source: "card.png"
                fillMode: Image.PreserveAspectFit
                Drag.active: cardDragArea.drag.active
                Drag.hotSpot.x: width / 2
                Drag.hotSpot.y: height / 2
                MouseArea {
                    id: cardDragArea
                    anchors.fill: parent
                    drag.target: parent
                }
            }
        }
        Button {
            Layout.fillWidth: true
            text: qsTr("Connect")
            onClicked: client.connectToPOS()
            visible: ! root.isConnected
        }
    }
}
