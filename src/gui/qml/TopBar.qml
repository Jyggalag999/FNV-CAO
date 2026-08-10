// Step 5 (part 2): real top bar QML, replacing MainWindow.ui's old native profile/pattern
// QComboBoxes/QPushButtons/QLabels (the "groupBox" grid - see MainWindow.ui/.cpp). Bound to a
// TopBarBridge instance exposed as the "topBar" context property. select*()/manage*() only relay
// intent via signals - MainWindow does the actual Settings mutation/dialog-opening, exactly like
// the native widgets' old connect() lambdas did (see MainWindow.cpp's constructor).
//
// No Run button here: the app's one real Run button lives in mainGroupBox (Open Directory/Run/
// One mod), untouched by this change - the original Widgets UI never had a second one in the
// profile/pattern box, so adding one here would be a new control, not a port.
//
// Each row is a plain Item with its combo box anchored left/right (label on the left, Manage
// button on the right) rather than a Row with a fixed combo width, so the combo stretches to
// fill the available width the way the original QGridLayout's column did - not a fixed size.
// Hosted with QQuickWidget::SizeRootObjectToView + an explicit minimum height set in C++ (see
// MainWindow.cpp) rather than a bare SizeRootObjectToView: this widget sits directly in
// centralwidget's top-level QVBoxLayout with nothing else to size it (unlike module tabs, which
// inherit an already-large size for free from QTabWidget), so without that explicit minimum the
// layout collapsed it to zero height - a chicken-and-egg problem, since the root's size depends
// on the view's size, which the layout couldn't determine without a hint from the root.
//
// anyPopupOpen: a QQuickWidget can't render content past its own bounds (unlike a native
// QComboBox's popup, which is its own top-level window, free to draw over everything else) - a
// combo box's dropdown list opening here would get clipped by the fixed minimum height above.
// MainWindow.cpp watches this property and temporarily grows the widget while a dropdown is
// open, then shrinks it back once both are closed, rather than permanently reserving that space.
import QtQuick

Rectangle {
    anchors.fill: parent
    color: "#170c26"

    property bool anyPopupOpen: profileCombo.popupOpen || (patternRow.visible && patternCombo.popupOpen)

    Column {
        anchors.fill: parent
        anchors.margins: 8
        // Matches anchors.margins above, so the gap above "Pattern" (this spacing, between the
        // two rows) is identical to the gap above "Profile" (the top margin) and below the last
        // row (the bottom margin) - previously 6 vs 8, a visible unevenness.
        spacing: 8

        Item {
            width: parent.width
            height: 26

            Text {
                id: profileLabel
                width: 50
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                color: "#e6d8ef"
                text: "Profile"
            }

            CaoButton {
                id: manageProfilesButton
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                visible: topBar.manageProfilesVisible
                text: "Manage"
                onClicked: topBar.manageProfiles()
            }

            CaoComboBox {
                id: profileCombo
                anchors.left: profileLabel.right
                anchors.leftMargin: 8
                anchors.right: manageProfilesButton.visible ? manageProfilesButton.left : parent.right
                anchors.rightMargin: manageProfilesButton.visible ? 8 : 0
                anchors.verticalCenter: parent.verticalCenter
                options: topBar.profileList
                currentIndex: topBar.profileList.indexOf(topBar.currentProfile)
                onActivated: (index) => topBar.selectProfile(topBar.profileList[index])
            }
        }

        Item {
            id: patternRow
            width: parent.width
            height: 26
            visible: topBar.patternsVisible

            Text {
                id: patternLabel
                width: 50
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                color: "#e6d8ef"
                text: "Pattern"
            }

            CaoButton {
                id: managePatternsButton
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                text: "Manage"
                onClicked: topBar.managePatterns()
            }

            CaoComboBox {
                id: patternCombo
                anchors.left: patternLabel.right
                anchors.leftMargin: 8
                anchors.right: managePatternsButton.left
                anchors.rightMargin: 8
                anchors.verticalCenter: parent.verticalCenter
                options: topBar.patternList
                currentIndex: topBar.patternList.indexOf(topBar.currentPattern)
                onActivated: (index) => topBar.selectPattern(topBar.patternList[index])
            }
        }
    }
}
