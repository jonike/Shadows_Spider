QT += opengl core gui widgets
CONFIG += console c++11
CONFIG -= windows
TARGET = Shadows_Spider
TEMPLATE = app

DEFINES += NOMINMAX _CRT_SECURE_NO_WARNINGS

win32
{
    INCLUDEPATH += C:/glew-1.11.0/include/
    INCLUDEPATH += C:/glm_972/
}

win32:CONFIG(release, debug|release): LIBS += -L"C:/glew-1.11.0/lib/Release/x64" -lglew32
win32: LIBS += -L"C:/Program Files/Microsoft SDKs/Windows/v7.1/Lib/x64" -lglu32 -lOpenGL32

SOURCES += \
    src/Anim.cpp \
    src/Attrs.cpp \
    src/CamCombo.cpp \
    src/CPop.cpp \
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
    src/RadPop.cpp \
    src/PaintWin.cpp \
    src/LayerSel.cpp

HEADERS  += \
    include/stb_image.h \
    src/Anim.h \
    src/Attrs.h \
    src/CamCombo.h \
    src/CPop.h \
    src/Gizmo.h \
    src/GLWidget.h \
    src/GLWidgetSh.h \
    src/MainWin.h \
    src/Object.h \
    src/Outliner.h \
    src/PP.h \
    src/Prefs.h \
    src/PrefWin.h \
    src/RadPop.h \
    src/PaintWin.h \
    src/LayerSel.h

OTHER_FILES += \

DISTFILES += \
    shaders/bbF.glsl \
    shaders/bbV.glsl \
    shaders/bloomF.glsl \
    shaders/downF.glsl \
    shaders/downV.glsl \
    shaders/fboV.glsl \
    shaders/finalF.glsl \
    shaders/fxaaF.glsl \
    shaders/gaussF.glsl \
    shaders/giz_circV.glsl \
    shaders/gizF.glsl \
    shaders/gizV.glsl \
    shaders/gridF.glsl \
    shaders/lumaAdapt_vizF.glsl \
    shaders/lumaAdaptF.glsl \
    shaders/lumaInitF.glsl \
    shaders/selRectF.glsl \
    shaders/selRectV.glsl \
    shaders/shadowF.glsl \
    shaders/shadowV.glsl \
    shaders/skyF.glsl \
    shaders/skyV.glsl \
    shaders/stencilHiF.glsl \
    shaders/stencilHiV.glsl \
    shaders/tonemapF.glsl \
    shaders/txtF.glsl \
    shaders/txtG.glsl \
    shaders/txtV.glsl \
    shaders/volumeLightF.glsl \
    shaders/volumeLightV.glsl \
    shaders/defF.glsl \
    shaders/stencilGeoF.glsl \
    shaders/stencilGeoV.glsl \
    shaders/wireframeF.glsl \
    shaders/wireframeV.glsl \
    shaders/bloomCF.glsl \
    shaders/ssrF.glsl \
    shaders/depthRevF.glsl \
    shaders/depthRevV.glsl \
    shaders/PaintStrokeCanvasF.glsl \
    shaders/paintProjF.glsl \
    shaders/paintProjV.glsl \
    shaders/copyTexF.glsl \
    shaders/blendModeF.glsl \
    shaders/paintStrokeF.glsl \
    shaders/wsQuadV.glsl \
    shaders/alphaAsRGBAF.glsl \
    shaders/eraseMixF.glsl \
    shaders/edgeDetectF.glsl \
    shaders/gBufferF.glsl \
    shaders/gBufferG.glsl \
    shaders/gBufferV.glsl \
    shaders/ssao_32F.glsl \
    shaders/ssao_64F.glsl


