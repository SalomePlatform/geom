TEMPLATE = lib
TARGET = DlgRef
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

QT_INCLUDES = $$(QTDIR)/include $$(QTDIR)/include/QtCore $$(QTDIR)/include/QtGui $$(QTDIR)/include/QtOpenGL $$(QTDIR)/include/QtXml

KERNEL_CXXFLAGS = $$(KERNEL_ROOT_DIR)/include/salome

GUI_CXXFLAGS = $$(GUI_ROOT_DIR)/include/salome

GUI_LDFLAGS = -L$$(GUI_ROOT_DIR)/lib/salome

STDLIB = -lstdc++

QT_MT_LIBS = -L$$(QTDIR)/lib -lQtCore -lQtXml -lQtGui -lQtOpenGL

INCLUDEPATH += $${QT_INCLUDES} $${KERNEL_CXXFLAGS} $${GUI_CXXFLAGS}

LIBS += $${GUI_LDFLAGS} -lqtx $${STDLIB} $${QT_MT_LIBS}

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += DLGREF_EXPORTS

HEADERS  = DlgRef_Skeleton_QTD.h
HEADERS += DlgRef_1Sel_QTD.h
HEADERS += DlgRef_2Sel_QTD.h
HEADERS += DlgRef_3Sel_QTD.h
HEADERS += DlgRef_1Sel1Check_QTD.h
HEADERS += DlgRef_1Sel3Check_QTD.h
HEADERS += DlgRef_1Sel1Check1List_QTD.h
HEADERS += DlgRef_1Sel1Spin_QTD.h
HEADERS += DlgRef_1Sel2Spin_QTD.h
HEADERS += DlgRef_1Sel3Spin_QTD.h
HEADERS += DlgRef_1Sel4Spin_QTD.h
HEADERS += DlgRef_1Sel5Spin_QTD.h
HEADERS += DlgRef_2Sel1Spin_QTD.h
HEADERS += DlgRef_2Sel2Spin_QTD.h
HEADERS += DlgRef_2Sel3Spin_QTD.h
HEADERS += DlgRef_1Sel1Spin1Check_QTD.h
HEADERS += DlgRef_1Sel3Spin1Check_QTD.h
HEADERS += DlgRef_2Sel1Spin2Check_QTD.h
HEADERS += DlgRef_2Sel2Spin1Check_QTD.h
HEADERS += DlgRef_2Sel4Spin1Check_QTD.h
HEADERS += DlgRef_3Sel4Spin2Check_QTD.h
HEADERS += DlgRef_4Sel1List_QTD.h
HEADERS += DlgRef_2Sel1List_QTD.h
HEADERS += DlgRef_1List1Spin1Btn_QTD.h
HEADERS += DlgRef_4Sel1List1Check_QTD.h
HEADERS += DlgRef_1Spin_QTD.h
HEADERS += DlgRef_2Spin_QTD.h
HEADERS += DlgRef_3Spin_QTD.h
HEADERS += DlgRef_3Spin1Check_QTD.h
HEADERS += DlgRef_3Sel3Spin1Check_QTD.h
HEADERS += DlgRef_4Sel1Spin2Check_QTD.h
HEADERS += DlgRef_SpinBox.h
HEADERS += DlgRef_1Sel1Spin.h
HEADERS += DlgRef_1Sel2Spin.h
HEADERS += DlgRef_1Sel3Spin.h
HEADERS += DlgRef_1Sel4Spin.h
HEADERS += DlgRef_1Sel5Spin.h
HEADERS += DlgRef_2Sel1Spin.h
HEADERS += DlgRef_2Sel2Spin.h
HEADERS += DlgRef_2Sel3Spin.h
HEADERS += DlgRef_1Sel1Spin1Check.h
HEADERS += DlgRef_1Sel3Spin1Check.h
HEADERS += DlgRef_2Sel1Spin2Check.h
HEADERS += DlgRef_2Sel2Spin1Check.h
HEADERS += DlgRef_2Sel4Spin1Check.h
HEADERS += DlgRef_3Sel4Spin2Check.h
HEADERS += DlgRef_3Sel3Spin1Check.h
HEADERS += DlgRef_1Spin.h
HEADERS += DlgRef_2Spin.h
HEADERS += DlgRef_3Spin.h
HEADERS += DlgRef_3Spin1Check.h
HEADERS += DlgRef_3Sel1Spin.h
HEADERS += DlgRef_1Sel_Ext.h
HEADERS += DlgRef_3Check_QTD.h
HEADERS += DlgRef_6Sel_QTD.h
HEADERS += DlgRef_4Sel1Spin2Check.h
HEADERS += DlgRef_3Sel1Check_QTD.h
HEADERS += GEOM_DlgRef.hxx

SOURCES  = DlgRef_SpinBox.cxx
SOURCES += DlgRef_1Sel1Spin.cxx
SOURCES += DlgRef_1Sel2Spin.cxx
SOURCES += DlgRef_1Sel3Spin.cxx
SOURCES += DlgRef_1Sel4Spin.cxx
SOURCES += DlgRef_1Sel5Spin.cxx
SOURCES += DlgRef_2Sel1Spin.cxx
SOURCES += DlgRef_2Sel2Spin.cxx
SOURCES += DlgRef_2Sel3Spin.cxx
SOURCES += DlgRef_1Sel1Spin1Check.cxx
SOURCES += DlgRef_1Sel3Spin1Check.cxx
SOURCES += DlgRef_2Sel1Spin2Check.cxx
SOURCES += DlgRef_2Sel2Spin1Check.cxx
SOURCES += DlgRef_2Sel4Spin1Check.cxx
SOURCES += DlgRef_3Sel4Spin2Check.cxx
SOURCES += DlgRef_3Sel3Spin1Check.cxx
SOURCES += DlgRef_1Spin.cxx
SOURCES += DlgRef_2Spin.cxx
SOURCES += DlgRef_3Spin.cxx
SOURCES += DlgRef_3Spin1Check.cxx
SOURCES += DlgRef_3Sel1Spin.cxx
SOURCES += DlgRef_4Sel1Spin2Check.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes