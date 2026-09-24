import QtQuick
import QtQuick.Controls


ApplicationWindow{
    id: root
    width: 476
    height: 650
    visible: true
    title: qsTr("EasyHub")

    StackView{
        id: stackView
        anchors.fill: parent

        initialItem: LoginPage{
            onLoginSuccess:{
                root.width = 476
                stackView.replace(mrListPage)
            }
        }
    }

    Component{
        id: mrListPage
        MRListPage{
            onLogout:{
                root.width = 476
                stackView.replace(loginPage)
            }
            onDetailOpened: {
                root.x = root.x - 850
                root.width = 1326
            }
            onDetailClosed: {
                root.x = root.x + 850
                root.width = 476
            }
            onNotificationRequested: {
                notificationCenter.open()
            }
        }
    }

    Component{
        id: loginPage
        LoginPage{
            onLoginSuccess:{
                root.width = 476
                stackView.replace(mrListPage)
            }
        }
    }

    NotificationPopup {
        id: notificationPopup
        onMessageCenterRequested: {
            notificationCenter.open()
        }
    }
    NotificationCenter {
        id: notificationCenter
    }
    Connections {
        target: notificationService
        function onPopupBatchReady(notificationIds) {
            notificationPopup.showBatch(notificationIds)
        }
    }
}

