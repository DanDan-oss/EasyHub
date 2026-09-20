import QtQuick
import QtQuick.Controls

TabButton {
    id: control
    property int count: 0
    background: Rectangle {
        radius: AppTheme.radiusMedium
        color: control.checked ? AppTheme.primarySoft : "transparent"
    }
    contentItem: Row {
        spacing: 4
        anchors.centerIn: parent
        Label {
            anchors.verticalCenter: parent.verticalCenter
            text: control.text
            color: control.checked ? AppTheme.primary : AppTheme.textSecondary
            font.bold: control.checked
        }
        Rectangle {
            anchors.verticalCenter: parent.verticalCenter
            visible: control.count > 0
            width: Math.max(18, countLabel.implicitWidth + 8)
            height: 18
            radius: height / 2
            color: AppTheme.danger
            Label {
                id: countLabel
                anchors.centerIn: parent
                text: control.count > 99 ? "99+" : control.count
                color: "white"
                font.pixelSize: 11
                font.bold: true
            }
        }
    }
}

