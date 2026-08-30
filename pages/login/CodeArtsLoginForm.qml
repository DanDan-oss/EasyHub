import QtQuick
import QtQuick.Controls

Column {
    id: root
    signal loginRequested(string domainName, string username, string password, string region)

    width: 280
    spacing: 14

    TextField {
        id: usernameField
        width: parent.width
        placeholderText: qsTr("Username")
    }

    TextField {
        id: passwordField
        width: parent.width
        placeholderText: qsTr("Password")
        echoMode: TextInput.Password
    }

    TextField {
        id: domainField
        width: parent.width
        placeholderText: qsTr("Domain")
    }

    TextField {
        id: regionField
        width: parent.width
        placeholderText: qsTr("Region")
    }

    Button {
        width: parent.width
        text: qsTr("Login")

        background: Rectangle {
            radius: AppTheme.radiusSmall
            color: parent.down ? "#2F60DF" : AppTheme.primary
        }

        contentItem: Label {
            text: parent.text
            color: "white"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.bold: true
        }

        onClicked: {
            root.loginRequested(domainField.text, usernameField.text, passwordField.text, regionField.text)
        }
    }

    Label {
        id: errorLabel
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        color: AppTheme.danger
    }

    function showError(message) {
        errorLabel.text = message
    }

    function clearError() {
        errorLabel.text = ""
    }
}
