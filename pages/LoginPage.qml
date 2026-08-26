import QtQuick
import QtQuick.Controls


Page{
    id: loginPage
    signal loginSuccess()

    background: Rectangle {
        color: AppTheme.background
    }

    Rectangle {
        width: 340
        height: 290
        anchors.centerIn: parent
        radius: 16
        color: AppTheme.surface

        border.width: 1
        border.color: AppTheme.border

        Column{
            width: 280
            anchors.centerIn: parent
            spacing: 14

            Label{
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("EasyHub")
                font.pixelSize: 28
                font.bold: true
                color: AppTheme.textPrimary
            }

            Label{
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Developer Workspace")
                color: AppTheme.textPrimary
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
                color: AppTheme.danger
            }
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

