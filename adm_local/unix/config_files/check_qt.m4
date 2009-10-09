dnl  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
dnl
dnl  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
dnl  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
dnl
dnl  This library is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU Lesser General Public
dnl  License as published by the Free Software Foundation; either
dnl  version 2.1 of the License.
dnl
dnl  This library is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl  Lesser General Public License for more details.
dnl
dnl  You should have received a copy of the GNU Lesser General Public
dnl  License along with this library; if not, write to the Free Software
dnl  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
dnl
dnl  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
dnl
dnl  OPTIONS_QT
dnl  ------------------------------------------------------------------------
dnl  Adds the --with-qt=path
dnl
AC_DEFUN([OPTIONS_QT], [
  AC_ARG_WITH([qt],
              [AC_HELP_STRING([--with-qt], [Possible usage:
                                             "Qt_Install_Path" - prefix to path where Qt was installd;
                                             "" or "yes" or "auto" - means seaching installed Qt product in the system (checking $QTDIR and then - standard system paths );
                                             "no" - not use Qt product (is not supported)
                                            By default "" option is used)])],
              [with_qt=$withval], [with_qt=""])
])

AC_DEFUN([CHECK_QT],[
AC_REQUIRE([AC_PROG_CC])dnl
AC_REQUIRE([AC_PROG_CXX])dnl
AC_REQUIRE([AC_PROG_CPP])dnl
AC_REQUIRE([AC_PROG_CXXCPP])dnl
AC_REQUIRE([CHECK_OPENGL])dnl
AC_REQUIRE([AC_LINKER_OPTIONS])dnl

AC_REQUIRE([OPTIONS_QT])dnl

AC_CHECKING(for Qt)

if test "x$OpenGL_ok" != "xyes" ; then
   AC_MSG_WARN(Qt needs OpenGL correct configuration, check configure output)
fi

qt_ok=yes

AC_LANG_SAVE
AC_LANG_CPLUSPLUS

dnl QT install dir
if test -z $with_qt ; then
  with_qt=""
fi
if test "x$with_qt" = "xyes" ; then
  dnl in case user wrote --with-qt=yes
  with_qt=""
fi
if test "x$with_qt" = "xauto" ; then
  dnl in case user wrote --with-qt=auto
  with_qt=""
fi
if test "x$with_qt" = "xno" ; then
  dnl in case user wrote --with-qt=no
  with_qt=""
  AC_MSG_WARN(Value no, specified for option --with-qt, is not supported)
fi

if test "x$with_qt" != "x" ; then
  dnl Using "--with-qt" prefix path
  QTDIR="$with_qt"
elif test "$QTDIR" = "/usr/lib/qt3" ; then
  dnl Using QTDIR environment variable
  AC_MSG_RESULT(current QTDIR is $QTDIR)
  dnl 
  if test -d /usr/lib/qt4 ; then
    AC_MSG_RESULT(it is strange for a qt4 installation !)
    AC_MSG_RESULT(/usr/lib/qt4 is present)
    AC_MSG_RESULT(replacing QTDIR by /usr/lib/qt4)
    QTDIR=/usr/lib/qt4
  fi
elif test -z $QTDIR ; then
  AC_MSG_WARN(undefined QTDIR variable which specify where Qt product was installed)
  for d in /usr/local/lib/qt4 /usr/lib/qt4 /usr ; do
    if test -f ${d}/lib/libQtCore.so ; then
      AC_MSG_RESULT(trying ${d})
      QTDIR="${d}"
      break
    elif test -f ${d}/lib64/libQtCore.so ; then
      AC_MSG_RESULT(trying ${d})
      QTDIR="${d}"
      break
    elif test -f ${d}/libQtCore.so ; then
      AC_MSG_RESULT(trying ${d})
      QTDIR="${d}"
      break
    fi
  done
fi

#
# check QTDIR environment variable
#
if test "x$QTDIR" = "x"
then
   AC_MSG_RESULT(Please define correct path in "--with-qt" option or use correct $QTDIR variable)
   qt_ok=no
fi

#
# check Qt version
#
if  test "x$qt_ok" = "xyes"
then
   qt_inc_ok=no
   QTINC=""
   AC_CHECK_FILE(${QTDIR}/include/qt4/QtCore/qglobal.h,QTINC="/qt4",QTINC="")
   if test "x$QTINC" = "x"
   then
     AC_CHECK_FILE(${QTDIR}/include${QTINC}/QtCore/qglobal.h,qt_inc_ok=yes,qt_inc_ok=no)
   else
     qt_inc_ok=yes
   fi
   if test "x$qt_inc_ok" = "xyes"
   then
     AC_MSG_CHECKING(whether Qt version >= 4.0)
     QT_VERSION=`grep "QT_VERSION_STR" ${QTDIR}/include${QTINC}/QtCore/qglobal.h | sed -e 's%^#define QT_VERSION_STR[[:space:]]*\"\([[:digit:]\.]*\)\"%\1%g'`
     QT_VERSION_ID=`echo $QT_VERSION | awk -F. '{v=$[1]*10000+$[2]*100+$[3];print v}'`
     if test $QT_VERSION_ID -ge 40000
     then
       AC_MSG_RESULT(yes)
     else
       AC_MSG_RESULT(no)
       qt_ok=no
     fi
     AC_MSG_CHECKING(Qt version)
     AC_MSG_RESULT($QT_VERSION)
   else
     qt_ok=no
   fi
fi

#
# check moc presence (meta-object compiler)
#
if  test "x$qt_ok" = "xyes"
then
  if test -f ${QTDIR}/bin/moc
  then
    MOC=${QTDIR}/bin/moc
  else
    AC_PATH_PROG(MOC, moc)
  fi
  if test "x$MOC" = "x"
  then
    qt_ok=no
    AC_MSG_RESULT(moc (Qt meta-object compiler) is not in the PATH variable)
  else
    qt_ok=yes
    AC_MSG_RESULT(moc (Qt meta-object compiler) is found)
  fi

  if test "x$qt_ok" = "xyes"
  then
    dnl check moc version
    AC_MSG_CHECKING(cheching equality Qt and moc tool version)
    MOC_VERSION=`$MOC -v 2>&1 | awk 'BEGIN{FS="[[ ()]]"};{print $(NF-1)}'`
    if test "x$QT_VERSION" = "x$MOC_VERSION"
    then
      AC_MSG_RESULT(yes)
      qt_ok=yes
    else
      AC_MSG_RESULT(moc tool and Qt product are incompatible $MOC_VERSION)
      qt_ok=no
    fi
  fi
fi

#
# check uic presence (user interface compiler)
#
if  test "x$qt_ok" = "xyes"
then
  if test -f ${QTDIR}/bin/uic
  then
    UIC=${QTDIR}/bin/uic
  else
    AC_PATH_PROG(UIC, uic)
  fi
  if test "x$UIC" = "x"
  then
    qt_ok=no
    AC_MSG_RESULT(uic (Qt user interface compiler) is not in the PATH variable)
  else
    qt_ok=yes
    AC_MSG_RESULT(uic (Qt user interface compiler) is found)
  fi
fi

#
# check rcc presence (resources compiler)
#
if  test "x$qt_ok" = "xyes"
then
  if test -f ${QTDIR}/bin/rcc
  then
    QRCC=${QTDIR}/bin/rcc
  else
    AC_PATH_PROG(QRCC, rcc)
  fi
  if test "x$QRCC" = "x"
  then
    qt_ok=no
    AC_MSG_RESULT(rcc (Qt resources compiler) is not in the PATH variable)
  else
    qt_ok=yes
    AC_MSG_RESULT(rcc (Qt resources compiler) is found)
  fi

  if test "x$qt_ok" = "xyes"
  then
    dnl check rcc version
    AC_MSG_CHECKING(cheching equality Qt and rcc tool version)
    QRCC_VERSION=`$QRCC -v 2>&1 | awk '{print $NF}'`
    if test "x$QT_VERSION" = "x$QRCC_VERSION"
    then
      AC_MSG_RESULT(yes)
      qt_ok=yes
    else
      AC_MSG_RESULT(rcc tool and Qt product are incompatible)
      qt_ok=no
    fi
  fi
fi

#
# check lrelease presence (translation files compiler)
#
if  test "x$qt_ok" = "xyes"
then
  if test -f ${QTDIR}/bin/lrelease
  then
    LRELEASE=${QTDIR}/bin/lrelease
  else
    AC_PATH_PROG(LRELEASE, lrelease)
  fi
  if test "x$LRELEASE" = "x"
  then
    qt_ok=no
    AC_MSG_RESULT(lrelease (Qt translation files compiler) is not in the PATH variable)
  else
    qt_ok=yes
    AC_MSG_RESULT(lrelease (Qt translation files compiler) is found)
  fi
  
  if test "x$qt_ok" = "xyes"
  then
    dnl check lrelease version
    AC_MSG_CHECKING( equality of Qt and lrelease tool version)
    LRELEASE_VERSION=`$LRELEASE -version 2>&1 | awk '{print $NF}'`
    if test "x$QT_VERSION" = "x$LRELEASE_VERSION"
    then
      AC_MSG_RESULT(yes)
      qt_ok=yes
    else
      AC_MSG_RESULT(lrelease tool and Qt product are incompatible)
      qt_ok=no
    fi
  fi
fi

QT_ROOT=$QTDIR

#
# check Qt header files
#
if  test "x$qt_ok" = "xyes"
then
  AC_CHECKING(include Qt header files)

  CPPFLAGS_old=$CPPFLAGS
  CPPFLAGS="$CPPFLAGS -I${QTDIR}/include${QTINC} -I${QTDIR}/include${QTINC}/QtCore"
  AC_CHECK_HEADER(QObject,qt_ok=yes,qt_ok=no)
  CPPFLAGS=$CPPFLAGS_old

  if  test "x$qt_ok" = "xno"
  then
    AC_MSG_RESULT(qt header files are not found in $QTDIR/include${QTINC}/QtCore)
    AC_MSG_RESULT(QTDIR environment variable may be wrong (probably too old Qt version))
  else
    AC_MSG_RESULT(qt header files seem to be OK)
    QT_INCLUDES="-I${QTDIR}/include${QTINC}"
    # user header files
    QT_INCLUDES="${QT_INCLUDES} -I${QTDIR}/include${QTINC}/QtCore"
    QT_INCLUDES="${QT_INCLUDES} -I${QTDIR}/include${QTINC}/QtGui"
    QT_INCLUDES="${QT_INCLUDES} -I${QTDIR}/include${QTINC}/QtOpenGL"
    QT_INCLUDES="${QT_INCLUDES} -I${QTDIR}/include${QTINC}/QtXml"
    # not used currently header files (uncomment if required)
    #QT_INCLUDES="${QT_INCLUDES} -I$(QTDIR)/include${QTINC}/QtAssistant"
    #QT_INCLUDES="${QT_INCLUDES} -I$(QTDIR)/include${QTINC}/QtDBus"
    #QT_INCLUDES="${QT_INCLUDES} -I$(QTDIR)/include${QTINC}/QtDesigner"
    #QT_INCLUDES="${QT_INCLUDES} -I$(QTDIR)/include${QTINC}/QtNetwork"
    #QT_INCLUDES="${QT_INCLUDES} -I$(QTDIR)/include${QTINC}/QtSql"
    #QT_INCLUDES="${QT_INCLUDES} -I$(QTDIR)/include${QTINC}/QtSvg"
    #QT_INCLUDES="${QT_INCLUDES} -I$(QTDIR)/include${QTINC}/QtTest"
    #QT_INCLUDES="${QT_INCLUDES} -I$(QTDIR)/include${QTINC}/QtUiTools"

    QT_ASSISTANT_INCLUDES="${QT_ASSISTANT_INCLUDES} -I${QTDIR}/include${QTINC}/QtAssistant"
    QT_ASSISTANT_INCLUDES="${QT_ASSISTANT_INCLUDES} -I${QTDIR}/include${QTINC}/QtNetwork"

    # this flag is obsolete with Qt 4
    QT_MT_INCLUDES="$QT_INCLUDES"
  fi
fi

#
# test Qt libraries
#
if  test "x$qt_ok" = "xyes"
then
  AC_MSG_CHECKING(linking against Qt library)

  CXXFLAGS_old=$CXXFLAGS
  CXXFLAGS="$CXXFLAGS $QT_INCLUDES"

  LIBS_old=$LIBS
  if test "x$QTDIR" = "x/usr" ; then
    QT_LIB_DIR=""
  elif test -d ${QTDIR}/lib; then
    QT_LIB_DIR="-L$QTDIR/lib"
  elif test -d ${QTDIR}/lib64; then
    QT_LIB_DIR="-L$QTDIR/lib64"
  elif test "x$QTDIR" = "x/usr/lib/qt4" ; then
    QT_LIB_DIR=""
  else
    AC_MSG_ERROR(Can't detect of Qt library directory )
    qt_ok=no
  fi
  LIBS="$LIBS $QT_LIB_DIR -lQtCore"

  AC_CACHE_VAL(salome_cv_lib_qt,[
    AC_TRY_LINK(
#include <QCoreApplication>
,   int n;
    char **s;
    QCoreApplication a(n, s);
    a.exec();,
    eval "salome_cv_lib_qt=yes",eval "salome_cv_lib_qt=no")
  ])
  qt_ok="$salome_cv_lib_qt"

  # BEGIN: for CCRT (installation of qt have only a "lib" directory)
  if  test "x$qt_ok" = "xno"
  then
    QT_LIB_DIR="-L$QTDIR/lib"
    LIBS="$LIBS_old $QT_LIB_DIR -lQtCore"

    AC_CACHE_VAL(salome_cv_lib_qt,[
      AC_TRY_LINK(
#include <QCoreApplication>
,     int n;
      char **s;
      QCoreApplication a(n, s);
      a.exec();,
      eval "salome_cv_lib_qt=yes",eval "salome_cv_lib_qt=no")
    ])
    qt_ok="$salome_cv_lib_qt"
  fi
  # END: for CCRT

  if  test "x$qt_ok" = "xno"
  then
    AC_MSG_RESULT(no)
    AC_MSG_RESULT(unable to link with qt library)
    AC_MSG_RESULT(QTDIR environment variable may be wrong (probably too old Qt version))
  else
    AC_MSG_RESULT(yes)
    # core libs
    QT_CORE_LIBS="$QT_LIB_DIR -lQtCore -lQtXml"
    # gui libs
    QT_GUI_LIBS="$QT_LIB_DIR -lQtGui -lQtOpenGL"
    # other libs (currently not used)
    QT_OTHER_LIBS="$QT_LIB_DIR"
    # other libs (can be used if necessary)
    #QT_OTHER_LIBS="$QT_LIB_DIR -lQt3Support -lQtAssistantClient -lQtDesigner -lQtNetwork -lQtSql -lQtSvg -lQtTest -ltQtUiTools"
    QT_ASSISTANT_LIBS="$QT_LIB_DIR -lQtAssistantClient -lQtNetwork"
    # all libs
    QT_LIBS="$QT_CORE_LIBS $QT_GUI_LIBS $QT_OTHER_LIBS"
    # this flag is obsolete with Qt 4
    QT_MT_LIBS="$QT_LIBS"
  fi
  LIBS=$LIBS_old
  CXXFLAGS=$CXXFLAGS_old
fi

AC_SUBST(QTDIR)
AC_SUBST(MOC)
AC_SUBST(UIC)
AC_SUBST(QRCC)
AC_SUBST(LRELEASE)

AC_SUBST(QT_ROOT)
AC_SUBST(QT_INCLUDES)
AC_SUBST(QT_MT_INCLUDES)
AC_SUBST(QT_ASSISTANT_INCLUDES)
AC_SUBST(QT_LIB_DIR)
AC_SUBST(QT_CORE_LIBS)
AC_SUBST(QT_GUI_LIBS)
AC_SUBST(QT_OTHER_LIBS)
AC_SUBST(QT_LIBS)
AC_SUBST(QT_MT_LIBS)
AC_SUBST(QT_ASSISTANT_LIBS)
AC_SUBST(QT_VERSION)
AC_SUBST(QT_VERSION_ID)

AC_LANG_RESTORE

AC_MSG_RESULT(for Qt: $qt_ok)

# Save cache
AC_CACHE_SAVE

])dnl
dnl
