TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

CONFIG += c++11

INCLUDEPATH += bib
INCLUDEPATH += gui_glut
INCLUDEPATH += user

DEFINES += USE_SDL
LIBS += -lglut -l3ds -lGLU -lGL -lm -lSDL -lSDL_image #-lgsl
#LIBS += -lGL -lGLU -lglut

HEADERS += \
    bib/Camera.h \
    bib/CameraDistante.h \
    bib/CameraJogo.h \
    bib/Desenha.h \
    bib/Vetor3D.h \
    bib/model3ds.h \
    bib/stanfordbunny.h \
    gui_glut/extra.h \
    gui_glut/gui.h \
    pessoal/personagem.h \
    pessoal/notebook.h \
    pessoal/cadeira.h \
    pessoal/mesa.h \
    pessoal/objeto.h \
    pessoal/board.h \
    pessoal/book.h \
    pessoal/revolver.h \
    pessoal/robot.h

SOURCES += \
    bib/Camera.cpp \
    bib/CameraDistante.cpp \
    bib/CameraJogo.cpp \
    bib/Desenha.cpp \
    bib/Vetor3D.cpp \
    main.cpp \
    bib/model3ds.cpp \
    gui_glut/extra.cpp \
    gui_glut/gui.cpp \
    pessoal/personagem.cpp \
    pessoal/notebook.cpp \
    pessoal/cadeira.cpp \
    pessoal/mesa.cpp \
    pessoal/objeto.cpp \
    pessoal/board.cpp \
    pessoal/book.cpp \
    pessoal/revolver.cpp \
    pessoal/robot.cpp

