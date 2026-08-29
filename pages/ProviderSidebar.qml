import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: sidebar

    signal providerSelected(string providerId, string providerName)
    signal addProviderRequested()
    signal settingsRequested()

    implicitWidth: 56
    color: AppTheme.surface
    border.width: 1
    border.color: AppTheme.border

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 6
        spacing: 8

        // 已登录平台
        Repeater {
            model: providerModel

            delegate: Rectangle {
                id: providerItem
                required property string providerId
                required property string providerName
                required property url iconSource
                required property bool loggedIn

                visible: loggedIn

                Layout.preferredWidth: 44
                Layout.preferredHeight: loggedIn ? 44 : 0
                radius: AppTheme.radiusMedium
                color: providerModel.currentProviderId == providerId ? AppTheme.primarySoft : providerMouseArea.containsMouse ? AppTheme.surfaceHover : "transparent"
                border.width: providerModel.currentProviderId === providerId ? 1 : 0
                border.color: AppTheme.primary

                Image {
                    anchors.centerIn: parent
                    width: 28
                    height: 28
                    source: iconSource
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                    mipmap: true
                }

                MouseArea {
                    id: providerMouseArea
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true

                    onClicked: {
                        if(providerModel.setCurrentProvider(providerId))
                        {
                            sidebar.providerSelected(providerId, providerName)
                        }
                    }
                }
                ToolTip.visible: providerMouseArea.containsMouse
                ToolTip.text: providerName
            }
        }
        // 添加平台
        Rectangle {
            Layout.preferredWidth: 44
            Layout.preferredHeight: 44
            radius: AppTheme.radiusMedium
            color: addMouseArea.containsMouse ? AppTheme.surfaceHover : "transparent"
            Label {
                anchors.centerIn: parent
                text: "+"
                font.pixelSize: 26
                color: AppTheme.textSecondary
            }
            MouseArea {
                id: addMouseArea
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    sidebar.addProviderRequested()
                }
            }

            ToolTip.visible: addMouseArea.containsMouse
            ToolTip.text: qsTr("Add Code Platform")
        }

        Item {
            Layout.fillHeight: true
        }

        // 设置
        Rectangle {
            Layout.preferredWidth: 44
            Layout.preferredHeight: 44

            radius: AppTheme.radiusMedium
            color: settingsMouseArea.containsMouse ? AppTheme.surfaceHover : "transparent"

            Label {
                anchors.centerIn: parent
                text: "⚙"
                font.pixelSize: 20
                color: AppTheme.textSecondary
            }

            MouseArea {
                id: settingsMouseArea
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor

                onClicked: {
                    sidebar.settingsRequested()
                }
            }
            ToolTip.visible: settingsMouseArea.containsMouse
            ToolTip.text: qsTr("Settings")
        }
    }
}
