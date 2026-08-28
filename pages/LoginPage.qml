import QtQuick
import QtQuick.Controls


Page{
    id: loginPage
    signal loginSuccess()

    ListModel{
        id: providerModel
        ListElement{
            name: "CodeArts"
            providerId: "codearts"
            iconSource: "../resources/icons/codearts.svg"
            loggedIn: false
        }
        ListElement{
            name: "GitLab"
            providerId: "gitlab"
            iconSource: "../resources/icons/gitlab.svg"
            loggedIn: false
        }
    }

    background: Rectangle {
        color: AppTheme.background
    }

    Rectangle {
        width: 340
        height: 360
        anchors.centerIn: parent
        radius: 16
        color: AppTheme.surface

        border.width: 1
        border.color: AppTheme.border

        Column {
            width: 280
            anchors.centerIn: parent
            spacing: 14

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("EasyHub")
                font.pixelSize: 28
                font.bold: true
                color: AppTheme.textPrimary
            }
            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Developer Workspace")
                color: AppTheme.textPrimary
            }

            Column {
                width: parent.width
                spacing: 6

                Label {
                    text: qsTr("Code Platform")
                    color: AppTheme.textSecondary
                    font.pixelSize: 12
                }

                ComboBox {
                    id: providerComboBox
                    width: parent.width
                    height: 40

                    model: providerModel
                    textRole: "name"

                    // 当前选中的平台显示
                    contentItem: Row {
                        spacing: 8
                        leftPadding: 10

                        Image {
                            width: 24
                            height: 24
                            anchors.verticalCenter: parent.verticalCenter
                            source: providerComboBox.currentIndex >= 0 ? providerModel.get(providerComboBox.currentIndex).iconSource : ""
                            fillMode: Image.PreserveAspectFit
                            smooth: true
                            mipmap: true
                        }
                        Label {
                            anchors.verticalCenter: parent.verticalCenter
                            text: providerComboBox.currentIndex >= 0 ? providerModel.get(providerComboBox.currentIndex).name : ""
                            color: AppTheme.textPrimary
                        }
                    }

                    // 下拉列表中的每一项
                    delegate: ItemDelegate {
                        required property int index
                        required property string name
                        required property url iconSource
                        required property bool loggedIn

                        width: providerComboBox.width
                        height: 40

                        enabled: !loggedIn

                        contentItem: Row {
                            spacing: 10

                            Image {
                                width: 24
                                height: 24
                                anchors.verticalCenter: parent.verticalCenter
                                source: iconSource
                                fillMode: Image.PreserveAspectFit
                                smooth: true
                                mipmap: true

                                opacity: loggedIn ? 0.4 :1.0
                            }
                            Label {
                                anchors.verticalCenter: parent.verticalCenter
                                text: name
                                color:  loggedIn ? AppTheme.textSecondary : AppTheme.textPrimary
                                opacity: loggedIn ? 0.4 :1.0
                            }

                            Label {
                                anchors.verticalCenter: parent.verticalCenter
                                visible: loggedIn
                                text: qsTr("Logged In")
                                color: AppTheme.textSecondary
                                opacity: 0.5
                            }
                        }
                        onClicked: {
                            providerComboBox.currentIndex = index
                            providerComboBox.popup.close()
                        }
                    }
                }
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
                    errorLabel.text = ""
                    var provider = providerModel.get(providerComboBox.currentIndex)
                    console.log("Login", provider.providerId, usernameField.text)
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

