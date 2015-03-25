QT += opengl core gui widgets
CONFIG += console c++11
CONFIG -= windows
TARGET = Shadows_Spider
TEMPLATE = app

DEFINES += NOMINMAX _CRT_SECURE_NO_WARNINGS

win32
{
    INCLUDEPATH += C:/glew-1.11.0/include/
    INCLUDEPATH += C:/glm_961/
}

win32:CONFIG(release, debug|release): LIBS += -L"C:/glew-1.11.0/lib/Release/x64" -lglew32
win32: LIBS += -L"C:/Program Files/Microsoft SDKs/Windows/v7.1/Lib/x64" -lglu32 -lOpenGL32

unix
{
    INCLUDEPATH += /home/aleks/Desktop/glm/
    INCLUDEPATH += /home/aleks/Desktop/glew-1.11.0/include/
}

unix: LIBS += -L"/home/aleks/Desktop/glew-1.11.0/lib/" -lGLEW -lGLU -lGL
unix: QMAKE_CXX = ccache g++
unix: QMAKE_CXXFLAGS += -Wno-missing-field-initializers
unix: QMAKE_CXXFLAGS += -Wno-maybe-uninitialized

SOURCES += \
    src/Anim.cpp \
    src/Attrs.cpp \
    src/CamCombo.cpp \
    src/CPop.cpp \
    src/CPopWin.cpp \
    src/Gizmo.cpp \
    src/GLWidget.cpp \
    src/GLWidgetSh.cpp \
    src/main.cpp \
    src/MainWin.cpp \
    src/Object.cpp \
    src/Outliner.cpp \
    src/PP.cpp \
    src/Prefs.cpp \
    src/PrefWin.cpp \
    src/RadPop.cpp

HEADERS  += \
    include/stb_image.h \
    src/Anim.h \
    src/Attrs.h \
    src/CamCombo.h \
    src/CPop.h \
    src/CPopWin.h \
    src/Gizmo.h \
    src/GLWidget.h \
    src/GLWidgetSh.h \
    src/MainWin.h \
    src/Object.h \
    src/Outliner.h \
    src/PP.h \
    src/Prefs.h \
    src/PrefWin.h \
    src/RadPop.h

OTHER_FILES += \

DISTFILES += \
    shaders/aoBloomCF.glsl \
    shaders/atmosF.glsl \
    shaders/base1F.glsl \
    shaders/base1G.glsl \
    shaders/base1V.glsl \
    shaders/base2F.glsl \
    shaders/base2G.glsl \
    shaders/base2V.glsl \
    shaders/base3F.glsl \
    shaders/base3G.glsl \
    shaders/base3V.glsl \
    shaders/bbF.glsl \
    shaders/bbV.glsl \
    shaders/bloomF.glsl \
    shaders/downF.glsl \
    shaders/downV.glsl \
    shaders/fboV.glsl \
    shaders/finalF.glsl \
    shaders/fxaaF.glsl \
    shaders/gaussF.glsl \
    shaders/gBufferF.glsl \
    shaders/gBufferV.glsl \
    shaders/giz_circV.glsl \
    shaders/giz_side_overF.glsl \
    shaders/giz_sideF.glsl \
    shaders/gizF.glsl \
    shaders/gizV.glsl \
    shaders/gridF.glsl \
    shaders/lumaAdapt_vizF.glsl \
    shaders/lumaAdaptF.glsl \
    shaders/lumaInitF.glsl \
    shaders/nVizF.glsl \
    shaders/nVizG.glsl \
    shaders/nVizV.glsl \
    shaders/rttF.glsl \
    shaders/rttV.glsl \
    shaders/selRectF.glsl \
    shaders/selRectV.glsl \
    shaders/shadowF.glsl \
    shaders/shadowV.glsl \
    shaders/skyF.glsl \
    shaders/skyV.glsl \
    shaders/ssaoF.glsl \
    shaders/stencilHiF.glsl \
    shaders/stencilHiV.glsl \
    shaders/tonemapF.glsl \
    shaders/txtF.glsl \
    shaders/txtG.glsl \
    shaders/txtV.glsl \
    shaders/volumeLightF.glsl \
    shaders/volumeLightV.glsl


