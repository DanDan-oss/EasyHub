import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id : detailPanel
    color: AppTheme.background

    signal closeRequested()

    property int iid: 0
    property string mrTitle: ""
    property string projectName: ""
    property string targetBranch: ""
    property string mrState: ""
    property string pipelineStatus: ""


    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        // 顶部MR信息卡
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 180

            radius: AppTheme.radiusMedium
            color: AppTheme.surface

            border.width: 1
            border.color: AppTheme.border

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 8
                RowLayout {
                    Layout.fillWidth: true
                    Label {
                        text: "#" + detailPanel.iid
                        font.pixelSize: 22
                        font.bold:  true
                    }
                    Label {
                        text: detailPanel.mrTitle
                        font.pixelSize: 22
                        font.bold:  true
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                    }
                    Button {
                        text: qsTr("Open in Browser")
                        background: Rectangle {
                            radius: AppTheme.radiusSmall
                            color: parent.hovered ? AppTheme.surfaceHover : AppTheme.surface
                            border.width: 1
                            border.color: AppTheme.border
                        }
                        contentItem: Label {
                            text: parent.text
                            color: AppTheme.textPrimary
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        onClicked: {
                            console.log("Open MR in browser", detailPanel.iid)
                        }
                    }
                    Button {
                        text: qsTr("Close")
                        background: Rectangle {
                            radius: AppTheme.radiusSmall
                            color: parent.hovered ? AppTheme.surfaceHover : AppTheme.surface
                            border.width: 1
                            border.color: AppTheme.border
                        }

                        contentItem: Label {
                            text: parent.text
                            color: AppTheme.textPrimary
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        onClicked: {
                            detailPanel.closeRequested()
                        }
                    }
                }
                Label {
                    text: qsTr("Project") + ": " +detailPanel.projectName
                    color: AppTheme.textSecondary
                }
                Label {
                    text: qsTr("Target Branch") + ": " +detailPanel.targetBranch
                    color: AppTheme.textSecondary
                }
                Label {
                    text: qsTr("State") + ": " +detailPanel.mrState
                    color: AppTheme.textSecondary
                }
                Label {
                    text: qsTr("Pipeline") + ": " +detailPanel.pipelineStatus
                    color: AppTheme.textSecondary
                }
            }
        }

        // 详情卡片
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true

            radius: AppTheme.radiusMedium
            color: AppTheme.surface

            border.width: 1
            border.color: AppTheme.border

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 8

                TabBar {
                    id: detailTabBar
                    Layout.fillWidth: true
                    TabButton {
                        text: qsTr("Discussion")
                        background: Rectangle {
                            radius: AppTheme.radiusSmall
                            color: parent.checked ? AppTheme.primarySoft : "transparent"
                        }
                        contentItem: Label {
                            text: parent.text
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            color: parent.checked ? AppTheme.primary : AppTheme.textSecondary
                            font.bold: parent.checked
                        }
                    }
                    TabButton {
                        text: qsTr("Commits")
                        background: Rectangle {
                            radius: AppTheme.radiusSmall
                            color: parent.checked ? AppTheme.primarySoft : "transparent"
                        }
                        contentItem: Label {
                            text: parent.text
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            color: parent.checked ? AppTheme.primary : AppTheme.textSecondary
                            font.bold: parent.checked
                        }
                    }
                    TabButton {
                        text: qsTr("Changes")
                        background: Rectangle {
                            radius: AppTheme.radiusSmall
                            color: parent.checked ? AppTheme.primarySoft : "transparent"
                        }
                        contentItem: Label {
                            text: parent.text
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            color: parent.checked ? AppTheme.primary : AppTheme.textSecondary
                            font.bold: parent.checked
                        }
                    }
                    TabButton {
                        text: qsTr("Pipeline")
                        background: Rectangle {
                            radius: AppTheme.radiusSmall
                            color: parent.checked ? AppTheme.primarySoft : "transparent"
                        }
                        contentItem: Label {
                            text: parent.text
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            color: parent.checked ? AppTheme.primary : AppTheme.textSecondary
                            font.bold: parent.checked
                        }
                    }
                }

                StackLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    currentIndex: detailTabBar.currentIndex
                    Label {
                        text: qsTr("Discussion will be shown here.")
                    }
                    Label {
                        text: qsTr("Commits will be shown here.")
                    }
                    Label {
                        text: qsTr("Changes will be shown here.")
                    }
                    Label {
                        text: qsTr("Pipeline will be shown here.")
                    }
                }
            }
        }

    }
}
