dnl Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
dnl
dnl This library is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU Lesser General Public
dnl License as published by the Free Software Foundation; either
dnl version 2.1 of the License, or (at your option) any later version.
dnl
dnl This library is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl Lesser General Public License for more details.
dnl
dnl You should have received a copy of the GNU Lesser General Public
dnl License along with this library; if not, write to the Free Software
dnl Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
dnl
dnl See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
dnl

dnl  File   : check_OpenCV.m4
dnl  Author : Renaud NEDELEC, Open CASCADE S.A.S (renaud.nedelec@opencascade.com)
dnl
AC_DEFUN([CHECK_OPENCV],[

AC_REQUIRE([AC_PROG_CXX])dnl
AC_REQUIRE([AC_PROG_CXXCPP])dnl

AC_CHECKING([for OPENCV product])

AC_LANG_SAVE
AC_LANG_C

OPENCV_INCLUDES=""
OPENCV_LIBS=""

AC_ARG_WITH([opencv],
        [  --with-opencv=DIR       root directory path of OPENCV installation])

opencv_ok=no

if test "$with_opencv" != "no" ; then
  if test "$with_opencv" == "yes" || test "$with_opencv" == "auto"; then
    OPENCV_HOME=""
  else
    OPENCV_HOME="$with_opencv"
  fi

  if test "$OPENCV_HOME" == "" ; then
    if test "x$OPENCV_INSTALL_DIR" != "x" ; then
      OPENCV_HOME=$OPENCV_INSTALL_DIR
    fi
  fi

  if test "x$OPENCV_HOME" != "x"; then

    LOCAL_INCLUDES="-I$OPENCV_HOME/include -I$OPENCV_HOME/include/opencv"
    LOCAL_LIBS="-L$OPENCV_HOME/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann"

    CPPFLAGS_old="$CPPFLAGS"
    CXXFLAGS_old="$CXXFLAGS"
    CPPFLAGS="$LOCAL_INCLUDES $CPPFLAGS"
    CXXFLAGS="$LOCAL_INCLUDES $CXXFLAGS"

    AC_MSG_CHECKING([for OPENCV library])

    LDFLAGS_old="$LDFLAGS"
    LDFLAGS="-L. $LOCAL_LIBS $LDFLAGS"

    LDFLAGS="$LDFLAGS_old"
    
    opencv_ok=yes

    AC_MSG_RESULT([$opencv_ok])

    CPPFLAGS="$CPPFLAGS_old"
    CXXFLAGS="$CXXFLAGS_old"

  fi
fi

if test "x$opencv_ok" == xno ; then
  AC_MSG_RESULT([for OPENCV: no])
  AC_MSG_WARN([OPENCV includes or libraries are not found or are not properly installed])
  AC_MSG_WARN([Use --with-opencv option to define OPENCV installation.])
else
  OPENCV_INCLUDES=$LOCAL_INCLUDES
  OPENCV_LIBS=$LOCAL_LIBS
  AC_MSG_RESULT([for OPENCV: yes])
fi

AM_CONDITIONAL(WITH_OPENCV, [test x"$opencv_ok" = xyes])

AC_SUBST(OPENCV_INCLUDES)
AC_SUBST(OPENCV_LIBS)

AC_LANG_RESTORE

])dnl
