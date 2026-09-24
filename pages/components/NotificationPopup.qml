import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

Window {
    id: popupWindow

    width: 360
    height: 120
    visible: false

    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool
    color: "transparent"

    property var notificationIds: []

    signal messageCenterRequested()

    function showBatch(ids) {
        notificationIds = ids
        if(notificationIds.length === 0) {
            notificationService.finishPopupBatch()
            return
        }

        updateContent()
        visible = true
        updatePosition()
        raise()
        closeTimer.restart()
    }

    function updateContent() {
        if(notificationIds.length >1) {
            titleLabel.text = qsTr("%1 new Merge Requests").arg(notificationIds.length)
            messageLabel.text = qsTr("View Messages")
            return
        }
        const notification = notificationService.model.get(notificationIds[0])
        if(!notification || !notification.id) {
            titleLabel.text = ""
            messageLabel.text = ""
            return
        }
        titleLabel.text = notification.title
        messageLabel.text = notification.message
    }

    function updatePosition() {
        const screen = popupWindow.screen
        if(!screen)
            return
        x = screen.virtualX + screen.desktopAvailableWidth - width - 16
        y = screen.virtualY + screen.desktopAvailableHeight - height - 16
    }
    function finish() {
        if(!visible)
            return
        closeTimer.stop()
        visible = false
        notificationService.finishPopupBatch()
    }

    function activateBatch() {
        if(notificationIds.length === 1)
        {
            const notificationId = notificationIds[0]
            notificationService.activate(notificationId)
            finish()
            return
        }
        messageCenterRequested()
        finish()
    }

    Rectangle {
        anchors.fill: parent
        radius: AppTheme.radiusMedium
        color: AppTheme.surface
        border.color: AppTheme.border

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 8

            RowLayout {
                Layout.fillWidth: true
                ColumnLayout {
                    Layout.fillWidth: true
                    Label {
                        id: titleLabel
                        Layout.fillWidth: true
                        color: AppTheme.textPrimary
                        font.bold: true
                        font.pixelSize: 14

                        elide: Text.ElideRight
                    }
                    Label {
                        id: messageLabel
                        Layout.fillWidth: true
                        color: AppTheme.textSecondary
                        font.pixelSize: 13
                        elide: Text.ElideRight
                    }
                    TapHandler {
                        enabled: popupWindow.notificationIds.length > 0
                        onTapped: {
                            popupWindow.activateBatch()
                        }
                    }
                }
            }

            ToolButton {
                Layout.alignment: Qt.AlignTop
                text: "x"
                onClicked: {
                    popupWindow.finish()
                }
            }
        }
        Item {
            Layout.fillHeight: true
        }
    }

    Timer {
        id: closeTimer
        interval: 5000
        repeat: false

        onTriggered: {
            popupWindow.finish()
        }
    }
}
