import QtQuick 6.0

QtObject {
    // Dark theme colors
    readonly property color backgroundDark: "#1a1a1a"
    readonly property color backgroundMedium: "#252525"
    readonly property color backgroundLight: "#2d2d2d"
    
    // Vibrant accent colors for edges
    readonly property color accentBlue: "#00a8ff"
    readonly property color accentPurple: "#9c27b0"
    readonly property color accentGreen: "#4caf50"
    readonly property color accentOrange: "#ff9800"
    readonly property color accentRed: "#f44336"
    readonly property color accentCyan: "#00bcd4"
    
    // Text colors
    readonly property color textPrimary: "#ffffff"
    readonly property color textSecondary: "#b0b0b0"
    readonly property color textTertiary: "#808080"
    
    // Border/edge colors
    readonly property color borderPrimary: accentBlue
    readonly property color borderSecondary: accentPurple
    readonly property color borderSuccess: accentGreen
    readonly property color borderWarning: accentOrange
    readonly property color borderError: accentRed
    
    // Button colors
    readonly property color buttonBackground: backgroundMedium
    readonly property color buttonHover: backgroundLight
    readonly property color buttonPressed: "#353535"
    
    // Spacing
    readonly property int spacingSmall: 4
    readonly property int spacingMedium: 8
    readonly property int spacingLarge: 16
    
    // Border width for thin edges
    readonly property int edgeWidth: 1
    readonly property int edgeWidthThick: 2
}

