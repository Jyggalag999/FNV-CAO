# overlay of upstream vcpkg qtdeclarative@6.8.1 (git-tree 3f33c58f58416f46a068efa973cc83ca58897c0b):
# disables the FluentWinUI3 quickcontrols style, whose build hits a reproducible AUTOMOC ordering bug
# in this Qt source (missing .moc for qtquickcontrols2fluentwinui3styleimplplugin_QtQuickControls2FluentWinUI3StyleImplPlugin.cpp,
# fails identically on repeated clean builds) and which CAO's UI doesn't use anyway.
set(SCRIPT_PATH "${CURRENT_INSTALLED_DIR}/share/qtbase")
include("${SCRIPT_PATH}/qt_install_submodule.cmake")

vcpkg_buildpath_length_warning(44)

set(${PORT}_PATCHES "")

 set(TOOL_NAMES
        qml
        qmlaotstats
        qmlcachegen
        qmleasing
        qmlformat
        qmlimportscanner
        qmllint
        qmlplugindump
        qmlpreview
        qmlprofiler
        qmlscene
        qmltestrunner
        qmltime
        qmltyperegistrar
        qmldom
        qmltc
        qmlls
        qmljsrootgen
        svgtoqml
    )

qt_install_submodule(PATCHES    ${${PORT}_PATCHES}
                     TOOL_NAMES ${TOOL_NAMES}
                     CONFIGURE_OPTIONS
                      -DCMAKE_DISABLE_FIND_PACKAGE_LTTngUST:BOOL=ON
                      -DFEATURE_quickcontrols2_fluentwinui3=OFF
                     CONFIGURE_OPTIONS_RELEASE
                     CONFIGURE_OPTIONS_DEBUG
                    )
