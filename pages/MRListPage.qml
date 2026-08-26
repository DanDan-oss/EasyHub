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
            state: "Opened"
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
            state: "Opened"
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
            state: "Opened"
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
            color: "#f5f5f5"

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
                    }
                    Item {
                        Layout.fillWidth: true
                    }

                    Button {
                        text: qsTr("Logout")
                        onClicked: {
                            mrListPage.logout()
                        }
                    }
                }
                TabBar {
                    id: tabBar
                    Layout.fillWidth: true
                    TabButton {
                        text: qsTr("To Merge")
                    }
                    TabButton {
                        text: qsTr("To Review")
                    }
                    TabButton {
                        text: qsTr("Created")
                    }
                    TabButton {
                        text: qsTr("To Approve")
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
                        width: mrList.width
                        height: 145
                        radius: 6
                        border.width: 1
                        border.color: "#d0d0d0"
                        color: "#ffffff"

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
                                            detailPanel.state=state
                                            detailPanel.pipelineStatus=pipelineStatus
                                            if(!mrListPage.detailVisible) {
                                                mrListPage.detailVisible=true
                                                mrListPage.detailOpened()
                                            }
                                        }
                                    }
                                }
                                Label {
                                    text: state
                                }
                            }
                            // 第二行: 项目 ->目标分支
                            Label {
                                Layout.fillWidth: true
                                text: project + " -> " + targetBranch
                                elide: Text.ElideRight
                            }
                            // 第三行: 代码增删 + 流水线状态
                            RowLayout {
                                Layout.fillWidth: true
                                Label {
                                    text: "+" + addedLines
                                }
                                Label {
                                    text: "-" + deletedLines
                                }
                                Item {
                                    Layout.fillWidth: true
                                }
                                Label {
                                    text: pipelineStatus
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
