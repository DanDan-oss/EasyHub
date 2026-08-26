import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id : detailPanel
    color: "white"

    signal closeRequested()

    property int iid: 0
    property string mrTitle: ""
    property string projectName: ""
    property string targetBranch: ""
    property string state: ""
    property string pipelineStatus: ""


    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

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
                onClicked: {
                    console.log("Open MR in browser", detailPanel.iid)
                }
            }
            Button {
                text: qsTr("Close")
                onClicked: {
                    detailPanel.closeRequested()
                }
            }
        }

        Label {
            text: qsTr("Project") + ": " +detailPanel.projectName
        }
        Label {
            text: qsTr("Target Branch") + ": " +detailPanel.targetBranch
        }
        Label {
            text: qsTr("State") + ": " +detailPanel.state
        }
        Label {
            text: qsTr("Pipeline") + ": " +detailPanel.pipelineStatus
        }
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#d0d0d0"
        }

        TabBar {
            id: detailTabBar
            Layout.fillWidth: true
            TabButton {
                text: qsTr("Discussion")
            }
            TabButton {
                text: qsTr("Commits")
            }
            TabButton {
                text: qsTr("Changes")
            }
            TabButton {
                text: qsTr("Pipeline")
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
