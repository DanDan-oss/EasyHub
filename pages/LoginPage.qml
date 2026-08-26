import QtQuick
import QtQuick.Controls


Page{
    id: loginPage
    signal loginSuccess()

    Column{
        width: 280
        anchors.centerIn: parent
        spacing: 16

        Label{
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("EasyHub")
            font.pixelSize: 32
            font.bold: true
        }

        Label{
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Developer Workspace")
        }

        TextField{
            id: usernameField
            width: parent.width
            placeholderText: qsTr("Username")
        }

        TextField{
            id: passwordField
            width: parent.width
            placeholderText: qsTr("Password")
            echoMode: TextInput.Password
        }

        Button{
            width: parent.width
            text: qsTr("Login")
            onClicked: {
                console.log("Login button clicked")
                errorLabel.text = ""
                authService.login(usernameField.text, passwordField.text)
            }
        }

        Label{
            id: errorLabel
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
        }
    }

    Connections{
        target: authService
        function onLoginSucceeded(){
            loginPage.loginSuccess()
        }

        function onLoginFailed(message){
            errorLabel.text = message
        }
    }
}

