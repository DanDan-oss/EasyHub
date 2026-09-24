import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Popup {
    id: control

    width: 360
    height: 420
    padding: 0
    modal: false
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    background:  Rectangle {
        radius: AppTheme.radiusMedium
        color: AppTheme.surface
        border.color: AppTheme.border
    }

    contentItem: ColumnLayout {
        spacing: 0

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 52
            Layout.leftMargin: 16
            Layout.rightMargin: 8

            Label {
                Layout.fillWidth: true
                text: qsTr("Message")
                color: AppTheme.textPrimary
                font.pixelSize: 16
                font.bold: true
            }

            Label {
                visible: notificationService.unreadCount > 0
                text: notificationService.unreadCount
                color: AppTheme.textSecondary
                font.pixelSize: 12
            }

            ToolButton {
                text: "x"
                onClicked: {
                    control.close()
                }
            }
        }
        ListView {
            id: notificationList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: notificationService.model
            delegate: ItemDelegate {
                required property string notificationId
                required property string title
                required property string message
                required property int iid

                width: notificationList.width
                height: 72
                onClicked: {
                    notificationService.activate(notificationId)
                    control.close()
                }
                contentItem: ColumnLayout {
                    spacing: 4
                    Label {
                        Layout.fillWidth: true
                        text: title
                        color: AppTheme.textPrimary
                        font.bold: true
                        font.pixelSize: 13
                        elide: Text.ElideRight
                    }
                    Label {
                        Layout.fillWidth: true
                        text: "!" + iid + " " + message
                        color: AppTheme.textSecondary
                        font.pixelSize: 12
                        elide: Text.ElideRight
                    }
                }
            }
            Label {
                anchors.centerIn: parent
                visible: notificationList.count === 0
                text:  qsTr("No New Messages")
                color: AppTheme.textSecondary
            }
        }
    }
}
