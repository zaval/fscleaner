pragma Singleton
import QtQuick

QtObject {
    // Colors
    readonly property color surface: "#faf9fe"
    readonly property color surfaceDim: "#dad9df"
    readonly property color surfaceBright: "#faf9fe"
    readonly property color surfaceContainerLowest: "#ffffff"
    readonly property color surfaceContainerLow: "#f4f3f8"
    readonly property color surfaceContainer: "#eeedf3"
    readonly property color surfaceContainerHigh: "#e9e7ed"
    readonly property color surfaceContainerHighest: "#e3e2e7"
    readonly property color colorOnSurface: "#1a1b1f"
    readonly property color colorOnSurfaceVariant: "#414755"
    readonly property color colorInverseSurface: "#2f3034"
    readonly property color colorInverseOnSurface: "#f1f0f5"
    readonly property color colorOutline: "#717786"
    readonly property color colorOutlineVariant: "#c1c6d7"
    readonly property color surfaceTint: "#005bc1"
    readonly property color primary: "#0058bc"
    readonly property color colorOnPrimary: "#ffffff"
    readonly property color primaryContainer: "#0070eb"
    readonly property color colorOnPrimaryContainer: "#fefcff"
    readonly property color inversePrimary: "#adc6ff"
    readonly property color secondary: "#4c4aca"
    readonly property color colorOnSecondary: "#ffffff"
    readonly property color secondaryContainer: "#6664e4"
    readonly property color colorOnSecondaryContainer: "#fffbff"
    readonly property color tertiary: "#894d00"
    readonly property color colorOnTertiary: "#ffffff"
    readonly property color tertiaryContainer: "#ac6300"
    readonly property color colorOnTertiaryContainer: "#fffbff"
    readonly property color error: "#ba1a1a"
    readonly property color colorOnError: "#ffffff"
    readonly property color errorContainer: "#ffdad6"
    readonly property color colorOnErrorContainer: "#93000a"
    readonly property color primaryFixed: "#d8e2ff"
    readonly property color primaryFixedDim: "#adc6ff"
    readonly property color colorOnPrimaryFixed: "#001a41"
    readonly property color colorOnPrimaryFixedVariant: "#004493"
    readonly property color secondaryFixed: "#e2dfff"
    readonly property color secondaryFixedDim: "#c2c1ff"
    readonly property color colorOnSecondaryFixed: "#0c006a"
    readonly property color colorOnSecondaryFixedVariant: "#3631b4"
    readonly property color tertiaryFixed: "#ffdcbf"
    readonly property color tertiaryFixedDim: "#ffb874"
    readonly property color colorOnTertiaryFixed: "#2d1600"
    readonly property color colorOnTertiaryFixedVariant: "#6a3b00"
    readonly property color background: "#faf9fe"
    readonly property color colorOnBackground: "#1a1b1f"
    readonly property color surfaceVariant: "#e3e2e7"

    // Typography
    readonly property string displayLgFontFamily: "Inter"
    readonly property int displayLgFontSize: 28
    readonly property int displayLgFontWeight: 600
    readonly property int displayLgLineHeight: 34
    readonly property real displayLgLetterSpacing: -0.02
    
    readonly property string headlineMdFontFamily: "Inter"
    readonly property int headlineMdFontSize: 20
    readonly property int headlineMdFontWeight: 600
    readonly property int headlineMdLineHeight: 26
    readonly property real headlineMdLetterSpacing: -0.01
    
    readonly property string titleSmFontFamily: "Inter"
    readonly property int titleSmFontSize: 15
    readonly property int titleSmFontWeight: 600
    readonly property int titleSmLineHeight: 20
    
    readonly property string bodyMdFontFamily: "Inter"
    readonly property int bodyMdFontSize: 14
    readonly property int bodyMdFontWeight: 400
    readonly property int bodyMdLineHeight: 20
    
    readonly property string bodySmFontFamily: "Inter"
    readonly property int bodySmFontSize: 12
    readonly property int bodySmFontWeight: 400
    readonly property int bodySmLineHeight: 16
    
    readonly property string labelCapsFontFamily: "Inter"
    readonly property int labelCapsFontSize: 11
    readonly property int labelCapsFontWeight: 700
    readonly property int labelCapsLineHeight: 16
    readonly property real labelCapsLetterSpacing: 0.05
    
    readonly property string monoDataFontFamily: "JetBrains Mono"
    readonly property int monoDataFontSize: 12
    readonly property int monoDataFontWeight: 400
    readonly property int monoDataLineHeight: 16

    // Rounded
    readonly property real roundedSm: 4
    readonly property real roundedDefault: 8
    readonly property real roundedMd: 12
    readonly property real roundedLg: 16
    readonly property real roundedXl: 24
    readonly property int roundedFull: 9999

    // Spacing
    readonly property int spacingUnit: 4
    readonly property int spacingContainerPadding: 24
    readonly property int spacingElementGap: 12
    readonly property point spacingListItemPadding: Qt.point(10, 16)
    readonly property int spacingGridGutter: 16

    function getOnColor(name: string): color {
        switch (name) {
            case "primary":
                return colorOnPrimary
            case "secondary":
                return colorOnSecondary
            case "tertiary":
                return colorOnTertiary
            case "outlined":
                return colorOnSurface
            case "error":
                return colorOnError
            default:
                return colorOnSurface
        }
    }

    function getColor(name: string): color {
        switch (name) {
            case "primary":
                return primary
            case "secondary":
                return secondary
            case "tertiary":
                return tertiary
            case "outlined":
                return surface
            case "error":
                return error
            default:
                return surface

        }
    }

    function getFontFamily(name: string): string {
        switch (name) {
            case "display-lg":
                return displayLgFontFamily
            case "headline-md":
                return headlineMdFontFamily
            case "title-sm":
                return titleSmFontFamily
            case "body-md":
                return bodyMdFontFamily
            case "body-sm":
                return bodySmFontFamily
            case "label-caps":
                return labelCapsFontFamily
            case "mono-data":
                return monoDataFontFamily
            default:
                return bodyMdFontFamily
        }
    }

    function getFontSize(name: string): int {
        switch (name) {
            case "display-lg":
                return displayLgFontSize
            case "headline-md":
                return headlineMdFontSize
            case "title-sm":
                return titleSmFontSize
            case "body-md":
                return bodyMdFontSize
            case "body-sm":
                return bodySmFontSize
            case "label-caps":
                return labelCapsFontSize
            case "mono-data":
                return monoDataFontSize
            default:
                return bodyMdFontSize
        }
    }

    function getFontWeight(name: string): int {
        switch (name) {
            case "display-lg":
                return displayLgFontWeight
            case "headline-md":
                return headlineMdFontWeight
            case "title-sm":
                return titleSmFontWeight
            case "body-md":
                return bodyMdFontWeight
            case "body-sm":
                return bodySmFontWeight
            case "label-caps":
                return labelCapsFontWeight
            case "mono-data":
                return monoDataFontWeight
            default:
                return bodyMdFontWeight
        }
    }
}
