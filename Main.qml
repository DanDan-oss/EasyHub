import QtQuick
import QtQuick.Controls


ApplicationWindow{
    id: root
    width: 420
    height: 650
    visible: true
    title: qsTr("EasyHub")

    StackView{
        id: stackView
        anchors.fill: parent

        initialItem: LoginPage{
            onLoginSuccess:{
                stackView.replace(mrListPage)
            }
        }
    }

    Component{
        id: mrListPage
        MRListPage{
            onLogout:{
                root.width = 420
                stackView.replace(loginPage)
            }
            onDetailOpened: {
                root.x = root.x - 850
                root.width = 1270
            }
            onDetailClosed: {
                root.x = root.x + 850
                root.width = 420
            }
        }
    }

    Component{
        id: loginPage
        LoginPage{
            onLoginSuccess:{
                stackView.replace(mrListPage)
            }
        }
    }
}

