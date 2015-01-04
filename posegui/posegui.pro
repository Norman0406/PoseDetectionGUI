TARGET = posegui
TEMPLATE = app
CONFIG += link_prl

SOURCES += src/main2.cpp

HEADERS += 

INCLUDEPATH += $${_PRO_FILE_PWD_}/src \
    $${_PRO_FILE_PWD_}/external/include \
    $${_PRO_FILE_PWD_}/../pose/include \

win32 {
    DEFINES += _CRT_SECURE_NO_WARNINGS

    INCLUDEPATH += $$"C:/Program Files/Microsoft SDKs/Kinect/v1.8/inc" \
        $$"C:/Program Files/Microsoft SDKs/Kinect/v2.0_1409/inc"

    LIBS += "C:/Program Files/Microsoft SDKs/Kinect/v1.8/lib/x86/Kinect10.lib" \
        "C:/Program Files/Microsoft SDKs/Kinect/v2.0_1409/lib/x86/Kinect20.lib" \
        $${_PRO_FILE_PWD_}/external/lib/Dimagerdll.lib

    OTHER_FILES += "$${_PRO_FILE_PWD_}/external/bin/Dimagerdll.dll"

    CONFIG(debug, debug|release) {
        BUILDDIR = debug
        QMAKE_CXXFLAGS_DEBUG += /openmp
        LIBS += -L$$OUT_PWD/../pose/debug/ -lpose
    }
    CONFIG(release, debug|release) {
        BUILDDIR = release
        QMAKE_CXXFLAGS_RELEASE += /openmp
        LIBS += -L$$OUT_PWD/../pose/release/ -lpose
    }

    # copy binary files to output directory
    for (file, OTHER_FILES) {
        copyFiles.commands += $(COPY) \"$$shell_path($$file)\" \"$$shell_path($$OUT_PWD/$$BUILDDIR)\" $$escape_expand(\n\t)
    }
    QMAKE_EXTRA_TARGETS += copyFiles
    POST_TARGETDEPS += copyFiles
}
