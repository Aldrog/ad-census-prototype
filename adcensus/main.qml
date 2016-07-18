import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2

Window {
    visible: true
    width: 820
    height: 480
    title: qsTr("AD-Census")

    ColumnLayout {
        anchors.fill: parent
        spacing: 8

        Row {
            //padding: 8
            spacing: 8

            Button {
                id: leftBtn
                text: qsTr("Choose left")

                onClicked: {
                    leftImgPicker.open()
                }
            }

            Button {
                id: rightBtn
                text: qsTr("Choose right")

                onClicked: {
                    rightImgPicker.open()
                }
            }

            Button {
                id: computeBtn
                text: qsTr("Start")

                onClicked: {
                    sharedADCensus.constructDisparityMap(leftImg.source, rightImg.source)
                }
            }
        }

        RowLayout {
            spacing: 8
            width: parent.width

            Layout.fillWidth: true
            Layout.fillHeight: true

            Image {
                id: leftImg
                fillMode: Image.PreserveAspectFit
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Image {
                id: rightImg
                fillMode: Image.PreserveAspectFit
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            /* Save result to file rather then show it atm */

//            Image {
//                id: disparityImg
//                fillMode: Image.PreserveAspectFit
//                Layout.fillWidth: true
//                Layout.fillHeight: true
//            }
        }
    }

    FileDialog {
        id: leftImgPicker

        selectedNameFilter: "*.jpg *.png"
        folder: shortcuts.home

        onAccepted: {
            leftImg.source = fileUrl
        }
    }

    FileDialog {
        id: rightImgPicker

        selectedNameFilter: "*.jpg *.png"
        folder: shortcuts.home

        onAccepted: {
            rightImg.source = fileUrl
        }
    }
}
