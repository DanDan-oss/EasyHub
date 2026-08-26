pragma Singleton

import QtQuick

QtObject {
    readonly property color background: "#F3F6FA"
    readonly property color surface: "#FFFFFF"
    readonly property color surfaceHover: "#F7FAFF"
    readonly property color border: "#E4E8EF"

    readonly property color primary: "#3478F6"
    readonly property color primarySoft: "#EAF2FF"

    readonly property color textPrimary: "#1F2937"
    readonly property color textSecondary: "#6B7280"

    readonly property color success: "#2E9B5F"
    readonly property color successSoft: "#E8F7EE"

    readonly property color warning: "#C98200"
    readonly property color warningSoft: "#FFF4E5"

    readonly property color danger: "#D64545"
    readonly property color dangerSoft: "#FDECEC"

    readonly property int radiusSmall: 6
    readonly property int radiusMedium: 10
}
