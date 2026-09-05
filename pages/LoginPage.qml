import QtQuick
import QtQuick.Controls
import "login"


Page{
    id: loginPage
    signal loginSuccess()

    background: Rectangle {
        color: AppTheme.background
    }

    Component {
        id: codeArtsLoginForm
        CodeArtsLoginForm {
            width: loginFormLoader.width
            onLoginRequested: function (domainName, username, password, region)
            {
                clearError()
                const providerId = providerModel.providerIdAt(providerComboBox.currentIndex)
                console.log("Login CodeArts", domainName, username, region)
                // 下一步这里再接AuthService
                authService.login(
                    providerId,
                    {
                        "domainName" : domainName,
                        "userName": username,
                        "password": password,
                        "region": region
                    }
                )
            }
        }
    }

    Component {
        id: gitLabLoginForm
        GitLabLoginForm {
            width: loginFormLoader.width
        }
    }

    Rectangle {
        width: 340
        height: contentColumn.implicitHeight + 52
        anchors.centerIn: parent
        radius: 16
        color: AppTheme.surface

        border.width: 1
        border.color: AppTheme.border

        Column {
            id: contentColumn
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
                    model: providerModel
                    textRole: "providerName"

                    contentItem: Row {
                        spacing: 8
                        leftPadding: 10

                        Image {
                            width: 24
                            height: 24
                            anchors.verticalCenter: parent.verticalCenter
                            source: providerComboBox.currentIndex >= 0 ? providerModel.iconSourceAt(providerComboBox.currentIndex) : ""
                            fillMode: Image.PreserveAspectFit
                            smooth: true
                            mipmap: true
                        }
                        Label {
                            anchors.verticalCenter: parent.verticalCenter
                            text: providerComboBox.displayText
                            color: AppTheme.textPrimary
                        }
                    }

                    delegate: ItemDelegate {
                        required property int index
                        required property string providerId
                        required property string providerName
                        required property url iconSource
                        required property bool loggedIn

                        width: providerComboBox.width
                        enabled: !loggedIn

                        contentItem: Row {
                            spacing: 10
                            leftPadding: 10
                            Image {
                                width: 24
                                height: 24
                                anchors.verticalCenter: parent.verticalCenter
                                source:iconSource
                                fillMode: Image.PreserveAspectFit
                                smooth: true
                                mipmap: true
                                opacity: loggedIn ? 0.4 : 1.0
                            }
                            Label {
                                anchors.verticalCenter: parent.verticalCenter
                                text: providerName
                                color:  loggedIn ? AppTheme.textSecondary : AppTheme.textPrimary
                                opacity: loggedIn ? 0.4 : 1.0
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

            Loader {
                id: loginFormLoader
                width: parent.width
                sourceComponent: {
                    if(providerComboBox.currentIndex < 0)
                        return null
                    var providerId = providerModel.providerIdAt(providerComboBox.currentIndex)
                    switch(providerId)
                    {
                    case "codearts":
                        return codeArtsLoginForm
                    case "gitlab":
                        return gitLabLoginForm
                    default:
                        return null
                    }
                }
            }
        }
    }

    Connections{
        target: authService
        function onLoginSucceeded(){
            loginPage.loginSuccess()
        }

        function onSessionRestored(type){
            loginPage.loginSuccess()
        }

        function onLoginFailed(message){
            if(loginFormLoader.item && loginFormLoader.item.showError)
            {
                loginFormLoader.item.showError(message)
            }
        }
    }
}

