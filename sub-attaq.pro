QT += widgets sensors opengl
QTPLUGIN += qtsensors_ios
contains(QT_CONFIG, release): CONFIG += release
TARGET = SubAttack
QMAKE_INFO_PLIST = Info.plist # with orientation locked to landscape

icons.files = Icon-114.png Icon@2x.png Default-568h@2x.png
QMAKE_BUNDLE_DATA += icons

qtHaveModule(opengl): QT += opengl
HEADERS += boat.h \
    bomb.h \
    mainwindow.h \
    submarine.h \
    torpedo.h \
    pixmapitem.h \
    graphicsscene.h \
    animationmanager.h \
    states.h \
    boat_p.h \
    submarine_p.h \
    qanimationstate.h \
    progressitem.h \
    textinformationitem.h \
    manualtransition.h
SOURCES += boat.cpp \
    bomb.cpp \
    main.cpp \
    mainwindow.cpp \
    submarine.cpp \
    torpedo.cpp \
    pixmapitem.cpp \
    graphicsscene.cpp \
    animationmanager.cpp \
    states.cpp \
    qanimationstate.cpp \
    progressitem.cpp \
    textinformationitem.cpp
RESOURCES += subattaq.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/animation/sub-attaq
INSTALLS += target
