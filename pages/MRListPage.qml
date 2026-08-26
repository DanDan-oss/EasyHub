import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: mrListPage

    signal logout()
    signal detailOpened()
    signal detailClosed()

    property bool detailVisible: false

    ListModel {
        id: mrModel

        ListElement {
            iid: 873
            title: "Fix log overwite bug"
            project: "MSIPTool"
            targetBranch: "ReleaseBranch_main"
            mrState: "Opened"
            pipelineStatus: "Success"
            addedLines: 10
            deletedLines: 15
            reviewProgress: "1/2"
            approveProgress: "0/1"
        }

        ListElement {
            iid: 912
            title: "Fix login exception"
            project: "WeSpace"
            targetBranch: "master"
            mrState: "Opened"
            pipelineStatus: "Running"
            addedLines: 8
            deletedLines: 3
            reviewProgress: "0/2"
            approveProgress: "0/1"
        }
        ListElement {
            iid: 1024
            title: "Update configuration loader"
            project: "eAPP610"
            targetBranch: "ICP-D"
            mrState: "Opened"
            pipelineStatus: "failed"
            addedLines: 23
            deletedLines: 11
            reviewProgress: "2/2"
            approveProgress: "1/1"
        }
    }


    RowLayout {
        anchors.fill: parent
        spacing: 0

        // ====== 左边MR详情栏 ============
        MRDetailPanel {
            id: detailPanel
            visible: mrListPage.detailVisible
            Layout.preferredWidth: mrListPage.detailVisible ? 850 : 0
            Layout.fillHeight: true

            onCloseRequested: {
                mrListPage.detailVisible = false
                mrListPage.detailClosed()
            }
        }

        // ====== 右边MR列表栏 ============
        Rectangle {
            Layout.preferredWidth: 420
            Layout.fillHeight: true
            color: AppTheme.background

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 10

                RowLayout {
                    Layout.fillWidth: true

                    Label {
                        text: qsTr("Merge Requests")
                        font.pixelSize: 22
                        font.bold: true
                        color: AppTheme.textPrimary
                    }
                    Item {
                        Layout.fillWidth: true
                    }

                    Button {
                        text: qsTr("Logout")
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
                            if(mrListPage.detailVisible){
                                mrListPage.detailVisible=false
                                mrListPage.detailClosed()
                            }
                            mrListPage.logout()
                        }
                    }
                }
                TabBar {
                    id: tabBar
                    Layout.fillWidth: true
                    TabButton {
                        text: qsTr("To Merge")
                        background: Rectangle {
                            radius: AppTheme.radiusMedium
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
                        text: qsTr("To Review")
                        background: Rectangle {
                            radius: AppTheme.radiusMedium
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
                        text: qsTr("Created")
                        background: Rectangle {
                            radius: AppTheme.radiusMedium
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
                        text: qsTr("To Approve")
                        background: Rectangle {
                            radius: AppTheme.radiusMedium
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

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 6
                    ComboBox {
                        Layout.fillWidth: true
                        model: [
                            qsTr("Opened"),
                            qsTr("Closed"),
                            qsTr("Merged")
                        ]
                    }
                    ComboBox {
                        Layout.fillWidth: true
                        model: [
                            qsTr("All Projects"),
                            "MSIPTool",
                            "WeSpace",
                            "eAPP610"
                        ]
                    }
                    ComboBox {
                        Layout.fillWidth: true
                        model: [
                            qsTr("All Branches"),
                            "master",
                            "ICP-D"
                        ]
                    }
                }
                ListView {
                    id: mrList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 8
                    clip: true
                    model: mrModel

                    delegate: Rectangle {
                        id: mrCard
                        width: mrList.width
                        height: 145
                        property bool hovered: false
                        radius: AppTheme.radiusMedium
                        border.width: 1
                        border.color: hovered ? "#C9D8F2" : AppTheme.border
                        color:  hovered ?  AppTheme.surfaceHover : AppTheme.surface

                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            acceptedButtons: Qt.NoButton
                            onEntered: mrCard.hovered = true
                            onExited: mrCard.hovered = false
                        }
                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 10
                            spacing: 5

                            // 第一行: MR编号 + 标题 + 状态
                            RowLayout {
                                Layout.fillWidth: true
                                Label {
                                    text: "#" +iid
                                    font.bold: true
                                }
                                Label {
                                    text: title
                                    font.bold: true
                                    color:  AppTheme.textPrimary
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                    MouseArea {
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: {
                                            console.log("MR clicked", iid, title)
                                            detailPanel.iid=iid
                                            detailPanel.mrTitle=title
                                            detailPanel.projectName=project
                                            detailPanel.targetBranch=targetBranch
                                            detailPanel.mrState=mrState
                                            detailPanel.pipelineStatus=pipelineStatus
                                            if(!mrListPage.detailVisible) {
                                                mrListPage.detailVisible=true
                                                mrListPage.detailOpened()
                                            }
                                        }
                                    }
                                }
                                Label {
                                    text: mrState
                                }
                            }
                            // 第二行: 项目 ->目标分支
                            Label {
                                Layout.fillWidth: true
                                text: project + " -> " + targetBranch
                                elide: Text.ElideRight
                                color: AppTheme.textSecondary
                            }
                            // 第三行: 代码增删 + 流水线状态
                            RowLayout {
                                Layout.fillWidth: true
                                Label {
                                    text: "+" + addedLines
                                    color: AppTheme.success
                                }
                                Label {
                                    text: "-" + deletedLines
                                    color: AppTheme.danger
                                }
                                Item {
                                    Layout.fillWidth: true
                                }
                                Rectangle {
                                    radius: AppTheme.radiusSmall
                                    implicitHeight: 24
                                    implicitWidth: pipelineLabel.implicitWidth +16

                                    color: {
                                        if(pipelineStatus === "Success" || pipelineStatus === "success")
                                            return AppTheme.successSoft
                                        if(pipelineStatus === "Failed" || pipelineStatus === "failed")
                                            return AppTheme.dangerSoft
                                        return AppTheme.warningSoft
                                    }
                                    Label {
                                        id: pipelineLabel
                                        anchors.centerIn: parent
                                        text: pipelineStatus
                                        color: {
                                            if(pipelineStatus === "Success" || pipelineStatus === "success")
                                                return AppTheme.success
                                            if(pipelineStatus === "Failed" || pipelineStatus === "failed")
                                                return AppTheme.danger
                                            return AppTheme.warning
                                        }
                                    }
                                }
                            }

                            // 第四行:检视 + 审核进度
                            RowLayout {
                                Layout.fillWidth: true
                                Label {
                                    text: qsTr("Review") + ": " + reviewProgress
                                }
                                Label {
                                    text: qsTr("Approve") + ": " + approveProgress
                                }
                                Item {
                                    Layout.fillWidth: true
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
