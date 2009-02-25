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
AC_DEFUN([CHECK_OPENGL],[
AC_REQUIRE([AC_PROG_CC])dnl
AC_REQUIRE([AC_PROG_CPP])dnl
AC_REQUIRE([AC_LINKER_OPTIONS])dnl

AC_LANG_SAVE
AC_LANG_CPLUSPLUS

AC_ARG_WITH(opengl,
  [AC_HELP_STRING([--with-opengl=DIR],[root directory path of OpenGL installation])],
  [opengl_dir="$withval"], 
  [dirs="/usr/lib64 /usr/lib /usr/local/lib64 /usr/local/lib /opt/graphics/OpenGL/lib64 /opt/graphics/OpenGL/lib /usr/openwin/lib64 /usr/openwin/lib /usr/X11R6/lib64 /usr/X11R6/lib"])dnl

AC_CHECKING(for OpenGL)
AC_CHECKING(for OpenGL headers)

OGL_INCLUDES=""
OGL_LIBS=""

GL_LIB_PATH=""
GLU_LIB_PATH=""

OpenGL_ok=no
OpenGL_libs_ok=no
OpenGL_headers_ok=no

dnl openGL headers
# by default
if test "x${opengl_dir}" != "x" ; then
  AC_MSG_RESULT(for opengl_dir: $opengl_dir)
  AC_CHECK_HEADER([${opengl_dir}/include/GL/gl.h],
                  [OpenGL_headers_ok=yes; OGL_INCLUDES="-I${opengl_dir}/include"],
                  [OpenGL_headers_ok=no])
  if test "x${OpenGL_headers_ok}" = "xyes" ; then
    AC_CHECKING(for default OpenGL library)
    if test "x${opengl_dir}" = "x/usr" ; then
      OGL_LIBS=""
    else
      OGL_LIBS="-L${opengl_dir}/lib"
    fi
    LDFLAGS_old="$LDFLAGS"
    LDFLAGS="$LDFLAGS $OGL_LIBS"
    AC_CHECK_LIB([GL],
                 [glBegin],
                 [OpenGL_libs_ok=yes],
                 [OpenGL_libs_ok=no])
    if test "x${OpenGL_libs_ok}" = "xyes" ; then
      AC_TRY_LINK([],
                  [],
                  [OpenGL_libs_ok=yes ; OpenGL_ok=yes; OGL_LIBS="$OGL_LIBS -lGL"],
                  [OpenGL_libs_ok=no])
    fi
    LDFLAGS="$LDFLAGS_old"
  fi
fi

if test "x${OpenGL_headers_ok}" = "xno" ; then
  AC_CHECK_HEADER(GL/gl.h,
                  [OpenGL_headers_ok=yes],
                  [OpenGL_headers_ok=no])
fi

# under SunOS ?
if test "x${OpenGL_headers_ok}" = "xno" ; then
  AC_CHECK_HEADERS(/usr/openwin/share/include/GL/glxmd.h,
                  [OpenGL_headers_ok=yes; OGL_INCLUDES="-I/usr/openwin/share/include/"],
                  [OpenGL_headers_ok=no])
fi

# under IRIX ?
if test "x${OpenGL_headers_ok}" = "xno" ; then
  AC_CHECK_HEADERS(/opt/graphics/OpenGL/include/GL/glxmd.h,
                  [OpenGL_headers_ok=yes; OGL_INCLUDES="-I/opt/graphics/OpenGL/include"],
                  [OpenGL_headers_ok=no])
fi

# some linux OpenGL servers hide the includes in /usr/X11R6/include/GL
if test "x${OpenGL_headers_ok}" = "xno" ; then
  AC_CHECK_HEADERS(/usr/X11R6/include/GL/gl.h,
                  [OpenGL_headers_ok=yes; OGL_INCLUDES="-I/usr/X11R6/include"],
                  [OpenGL_headers_ok=no])
fi

if test "x${OpenGL_headers_ok}" = "xyes" ; then
  AC_CHECKING(for OpenGL library)
  for idir in $dirs; do
    if test -r "${idir}/libGL.so"; then
      AC_MSG_RESULT(in ${idir})
      if test "x${idir}" = "x/usr/lib64" -o "x${idir}" = "x/usr/lib" ; then
        GL_LIB_PATH=""
      else
        GL_LIB_PATH="-L${idir}"
      fi
      break
    fi
    # under IRIX ?
    if test -r "${idir}/libGL.sl"; then
      AC_MSG_RESULT(in ${idir})
      if test "x${idir}" = "x/usr/lib64" -o "x${idir}" = "x/usr/lib" ; then
        GL_LIB_PATH=""
      else
        GL_LIB_PATH="-L${idir}"
      fi
      break
    fi
  done
  LDFLAGS_old="${LDFLAGS}"
  LDFLAGS="${LDFLAGS} ${GL_LIB_PATH}"
  AC_CHECK_LIB([GL],
               [glBegin],
               [OpenGL_libs_ok=yes],
               [OpenGL_libs_ok=no])
  if test "x${OpenGL_libs_ok}" = "xyes" ; then
    AC_TRY_LINK([],
                [],
                [OpenGL_libs_ok=yes ; OGL_LIBS="${OGL_LIBS} ${GL_LIB_PATH} -lGL"],
                [OpenGL_libs_ok=no])
  fi
  LDFLAGS="$LDFLAGS_old"
fi

if test "x${OpenGL_libs_ok}" = "xyes" ; then
  for idir in $dirs; do
    if test -r "${idir}/libGLU.so"; then
      AC_MSG_RESULT(in ${idir})
      if test "x${idir}" = "x/usr/lib64" -o "x${idir}" = "x/usr/lib" ; then
        GLU_LIB_PATH=""
      else
        GLU_LIB_PATH="-L${idir}"
      fi
      break
    fi
    # under IRIX ?
    if test -r "${idir}/libGLU.sl"; then
      AC_MSG_RESULT(in ${idir})
      if test "x${idir}" = "x/usr/lib64" -o "x${idir}" = "x/usr/lib" ; then
        GLU_LIB_PATH=""
      else
        GLU_LIB_PATH="-L${idir}"
      fi
      break
    fi
  done

  # workaround a problem with libGL library location
  for idir in $dirs; do
    if test -r "${idir}/libGLU.la"; then
      GLU_LA_PATH="${idir}/libGLU.la"
      GL_LA_PATH_TO_CHECK=`cat ${GLU_LA_PATH} | awk '{ for(i=1;i<NF;i++){ if(gsub("libGL.la","&",$i)>0) print $i } }'`
      if test -z ${GL_LA_PATH_TO_CHECK} || test -r ${GL_LA_PATH_TO_CHECK}; then
        # nothing to do
        break
      fi
      for jdir in $dirs; do
        if test -r "${jdir}/libGL.la"; then
          GL_LA_PATH="${jdir}/libGL.la"
          # copy the libGLU.la file and set correct libGL.la path in it
          NEW_GLU_LA_PATH=${ROOT_BUILDDIR}
          NEW_GLU_LA_FILE="${NEW_GLU_LA_PATH}/libGLU.la"
          sed -e "s%${GL_LA_PATH_TO_CHECK}%${GL_LA_PATH}%" ${GLU_LA_PATH} > "${NEW_GLU_LA_FILE}"
          chmod -f --reference=${GLU_LA_PATH} "${NEW_GLU_LA_FILE}"
          # set a new libGLU.la path
          GLU_LIB_PATH="-L${NEW_GLU_LA_PATH}"
          # create a simbolic link to libGLU.so
          for kdir in $dirs; do
            if test -r "${kdir}/libGLU.so"; then
              cp -fs "${kdir}/libGLU.so" "${NEW_GLU_LA_PATH}/libGLU.so"
              break
            fi
          done
          break
        fi
      done
      break
    fi
  done

  LDFLAGS_old="${LDFLAGS}"
  LDFLAGS="${LDFLAGS} ${OGL_LIBS} ${GLU_LIB_PATH}"
  AC_CHECK_LIB([GLU],
               [gluBeginSurface],
               [OpenGL_libs_ok=yes],
               [OpenGL_libs_ok=no])
  if test "x${OpenGL_libs_ok}" = "xyes" ; then
    AC_TRY_LINK([],
                [],
                [OpenGL_libs_ok=yes ; OGL_LIBS="${OGL_LIBS} ${GLU_LIB_PATH} -lGLU"],
                [OpenGL_libs_ok=no])
  fi
  LDFLAGS="$LDFLAGS_old"
fi

if test "x${OpenGL_headers_ok}" = "xyes" ; then
  if test "x${OpenGL_libs_ok}" = "xyes" ; then
    OpenGL_ok=yes
  fi
fi

AC_MSG_RESULT(for OpenGL_headers_ok: $OpenGL_headers_ok)
AC_MSG_RESULT(for OpenGL_libs_ok: $OpenGL_libs_ok)
AC_MSG_RESULT(for OpenGL_ok: $OpenGL_ok)

AC_SUBST(OGL_INCLUDES)
AC_SUBST(OGL_LIBS)

AC_LANG_RESTORE

])dnl

