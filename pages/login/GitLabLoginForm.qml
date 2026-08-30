import QtQuick
import QtQuick.Controls

Column {
    id: root
    width: 280
    spacing: 14

    Label {
        width: parent.width
        text: qsTr("GitLab login is not implemented yet.")
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        color: AppTheme.textSecondary
    }
}
