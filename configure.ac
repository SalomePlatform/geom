#  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

# Author : Marc Tajchman (CEA)
# Date : 28/06/2001
# Modified by : Patrick GOLDBRONN (CEA)
# Modified by : Marc Tajchman (CEA)
# Modified by : Alexander BORODIN (OCN) - autotools usage
# Created from configure.in.base
#
AC_INIT([Salome2 Project GEOM module], [5.1.4], [webmaster.salome@opencascade.com], [SalomeGEOM])
AC_CONFIG_AUX_DIR(adm_local/unix/config_files)
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([tar-pax -Wno-portability])


XVERSION=`echo $VERSION | awk -F. '{printf("0x%02x%02x%02x",$1,$2,$3)}'`
AC_SUBST(XVERSION)

# set up MODULE_NAME variable for dynamic construction of directories (resources, etc.)
MODULE_NAME=geom
AC_SUBST(MODULE_NAME)

echo
echo ---------------------------------------------
echo Initialize source and build root directories
echo ---------------------------------------------
echo

dnl
dnl Initialize source and build root directories
dnl

ROOT_BUILDDIR=`pwd`
ROOT_SRCDIR=`echo $0 | sed -e "s,[[^/]]*$,,;s,/$,,;s,^$,.,"`
cd $ROOT_SRCDIR
ROOT_SRCDIR=`pwd`
cd $ROOT_BUILDDIR

AC_SUBST(ROOT_SRCDIR)
AC_SUBST(ROOT_BUILDDIR)

echo
echo Source root directory : $ROOT_SRCDIR
echo Build  root directory : $ROOT_BUILDDIR
echo
echo

if test -z "$AR"; then
   AC_CHECK_PROGS(AR,ar xar,:,$PATH)
fi
AC_SUBST(AR)

dnl Export the AR macro so that it will be placed in the libtool file
dnl correctly.
export AR

echo
echo ---------------------------------------------
echo testing make
echo ---------------------------------------------
echo

AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_LOCAL_INSTALL
dnl
dnl libtool macro check for CC, LD, NM, LN_S, RANLIB, STRIP + pour les librairies dynamiques !

echo
echo ---------------------------------------------
echo Configuring production
echo ---------------------------------------------
echo
AC_ENABLE_DEBUG(yes)
AC_DISABLE_PRODUCTION

echo ---------------------------------------------
echo testing libtool
echo ---------------------------------------------

dnl first, we set static to no!
dnl if we want it, use --enable-static
AC_ENABLE_STATIC(no)

AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL

dnl Fix up the INSTALL macro if it s a relative path. We want the
dnl full-path to the binary instead.
case "$INSTALL" in
   *install-sh*)
      INSTALL='\${ROOT_BUILDDIR}'/adm_local/unix/config_files/install-sh
      ;;
esac

echo
echo ---------------------------------------------
echo testing C/C++
echo ---------------------------------------------
echo

cc_ok=no
dnl inutil car libtool
dnl AC_PROG_CC
AC_PROG_CXX
AC_CXX_WARNINGS
AC_CXX_TEMPLATE_OPTIONS
AC_DEPEND_FLAG
# AC_CC_WARNINGS([ansi])
cc_ok=yes

dnl Library libdl :
AC_CHECK_LIB(dl,dlopen)

dnl add library libm :
AC_CHECK_LIB(m,ceil)

dnl
dnl Well we use sstream which is not in gcc pre-2.95.3
dnl We must test if it exists. If not, add it in include !
dnl

AC_CXX_HAVE_SSTREAM

dnl
dnl ---------------------------------------------
dnl testing MPICH
dnl ---------------------------------------------
dnl

CHECK_MPICH

echo
echo ---------------------------------------------
echo testing LEX \& YACC
echo ---------------------------------------------
echo

lex_yacc_ok=no
AC_PROG_YACC
AC_PROG_LEX
lex_yacc_ok=yes

echo
echo ---------------------------------------------
echo testing python
echo ---------------------------------------------
echo

CHECK_PYTHON

dnl echo
dnl echo ---------------------------------------------
dnl echo testing java
dnl echo ---------------------------------------------
dnl echo

dnl CHECK_JAVA

echo
echo ---------------------------------------------
echo testing swig
echo ---------------------------------------------
echo

AM_PATH_PYTHON(2.3)
CHECK_SWIG

echo
echo ---------------------------------------------
echo testing threads
echo ---------------------------------------------
echo

ENABLE_PTHREADS

if test "x${GUI_DISABLE_CORBA}" != "xyes" ; then
echo
echo ---------------------------------------------
echo testing omniORB
echo ---------------------------------------------
echo

CHECK_OMNIORB

dnl echo
dnl echo ---------------------------------------------
dnl echo testing mico
dnl echo ---------------------------------------------
dnl echo

dnl CHECK_MICO

echo
echo ---------------------------------------------
echo default ORB : omniORB
echo ---------------------------------------------
echo

DEFAULT_ORB=omniORB

echo
echo ---------------------------------------------
echo testing Corba
echo ---------------------------------------------
echo

CHECK_CORBA

AC_SUBST_FILE(CORBA)
corba=make_$ORB
CORBA=adm_local/unix/$corba

fi

echo
echo ---------------------------------------------
echo Testing GUI
echo ---------------------------------------------
echo

CHECK_GUI_MODULE

gui_ok=no
if test "${SalomeGUI_need}" != "no" -a "${FullGUI_ok}" = "yes" ; then 
  gui_ok=yes
fi

AM_CONDITIONAL(GEOM_ENABLE_GUI, [test "${gui_ok}" = "yes"])

if test "${SalomeGUI_need}" == "yes"; then
  if test "${FullGUI_ok}" != "yes"; then
    AC_MSG_WARN(For configure GEOM module necessary full GUI!)
  fi
elif test "${SalomeGUI_need}" == "auto"; then
  if test "${FullGUI_ok}" != "yes"; then
    AC_MSG_WARN(Full GUI not found. Build will be done without GUI!)
  fi
elif test "${SalomeGUI_need}" == "no"; then
  echo Build without GUI option has been chosen
fi

if test "${gui_ok}" = "yes"; then
    echo
    echo ---------------------------------------------
    echo testing openGL
    echo ---------------------------------------------
    echo

    CHECK_OPENGL

    echo
    echo ---------------------------------------------
    echo testing QT
    echo ---------------------------------------------
    echo

    CHECK_QT
fi

echo
echo ---------------------------------------------
echo testing VTK
echo ---------------------------------------------
echo

CHECK_VTK

echo
echo ---------------------------------------------
echo testing HDF5
echo ---------------------------------------------
echo

CHECK_HDF5

echo
echo ---------------------------------------------
echo BOOST Library
echo ---------------------------------------------
echo

CHECK_BOOST

echo
echo ---------------------------------------------
echo Testing OpenCascade
echo ---------------------------------------------
echo

CHECK_CAS

echo
echo ---------------------------------------------
echo Testing html generators
echo ---------------------------------------------
echo

CHECK_HTML_GENERATORS

echo
echo ---------------------------------------------
echo testing sphinx
echo ---------------------------------------------
echo
CHECK_SPHINX

echo
echo ---------------------------------------------
echo Testing Kernel
echo ---------------------------------------------
echo

CHECK_KERNEL

echo
echo ---------------------------------------------
echo Summary
echo ---------------------------------------------
echo

echo Configure

if test "${gui_ok}" = "yes"; then
  variables="cc_ok lex_yacc_ok python_ok swig_ok threads_ok OpenGL_ok qt_ok vtk_ok hdf5_ok omniORB_ok boost_ok occ_ok doxygen_ok graphviz_ok sphinx_ok Kernel_ok gui_ok"
elif test "${SalomeGUI_need}" != "no"; then
  variables="cc_ok lex_yacc_ok python_ok swig_ok threads_ok vtk_ok hdf5_ok omniORB_ok boost_ok occ_ok doxygen_ok graphviz_ok Kernel_ok gui_ok"
else
  variables="cc_ok lex_yacc_ok python_ok swig_ok threads_ok vtk_ok hdf5_ok omniORB_ok boost_ok occ_ok doxygen_ok graphviz_ok Kernel_ok"
fi

for var in $variables
do
   printf "   %10s : " `echo \$var | sed -e "s,_ok,,"`
   eval echo \$$var
done

echo
echo "Default ORB   : $DEFAULT_ORB"
echo

dnl We don t need to say when we re entering directories if we re using
dnl GNU make because make does it for us.
if test "X$GMAKE" = "Xyes"; then
   AC_SUBST(SETX) SETX=":"
else
   AC_SUBST(SETX) SETX="set -x"
fi

echo
echo ---------------------------------------------
echo generating Makefiles and configure files
echo ---------------------------------------------
echo

#AC_OUTPUT_COMMANDS([ \
#  chmod +x ./bin/*; \
#])

# This list is initiated using autoscan and must be updated manually
# when adding a new file <filename>.in to manage. When you execute
# autoscan, the Makefile list is generated in the output file configure.scan.
# This could be helpfull to update de configuration.
AC_OUTPUT([ \
  adm_local/Makefile \
  adm_local/cmake_files/Makefile \
  adm_local/unix/Makefile \
  adm_local/unix/config_files/Makefile \
  bin/VERSION \
  bin/Makefile \
  GEOM_version.h \
  doc/Makefile \
  doc/docutils/Makefile \
  doc/salome/Makefile \
  doc/salome/gui/Makefile \
  doc/salome/gui/GEOM/Makefile \
  doc/salome/gui/GEOM/doxyfile \
  doc/salome/gui/GEOM/doxyfile_py \
  doc/salome/gui/GEOM/doxyfile_tui \
  doc/salome/gui/GEOM/static/header.html \
  doc/salome/tui/Makefile \
  doc/salome/tui/doxyfile \
  doc/salome/tui/static/header.html \
  src/Makefile \
  src/AdvancedGUI/Makefile \
  src/ARCHIMEDE/Makefile \
  src/BREPExport/Makefile \
  src/BREPImport/Makefile \
  src/BasicGUI/Makefile \
  src/BlocksGUI/Makefile \
  src/BooleanGUI/Makefile \
  src/BuildGUI/Makefile \
  src/DisplayGUI/Makefile \
  src/DlgRef/Makefile \
  src/EntityGUI/Makefile \
  src/GEOM/Makefile \
  src/GEOMAlgo/Makefile \
  src/GEOMBase/Makefile \
  src/GEOMClient/Makefile \
  src/GEOMFiltersSelection/Makefile \
  src/GEOMGUI/Makefile \
  src/GEOMImpl/Makefile \
  src/GEOMToolsGUI/Makefile \
  src/GEOM_I/Makefile \
  src/GEOM_I_Superv/Makefile \
  src/GEOM_SWIG/Makefile \
  src/GEOM_SWIG_WITHIHM/Makefile \
  src/GEOM_PY/Makefile \
  src/GEOM_PY/structelem/Makefile \
  src/GenerationGUI/Makefile \
  src/GroupGUI/Makefile \
  src/IGESExport/Makefile \
  src/IGESImport/Makefile \
  src/MeasureGUI/Makefile \
  src/NMTDS/Makefile \
  src/NMTTools/Makefile \
  src/OBJECT/Makefile \
  src/OCC2VTK/Makefile \
  src/OperationGUI/Makefile \
  src/PrimitiveGUI/Makefile \
  src/RepairGUI/Makefile \
  src/SKETCHER/Makefile \
  src/STEPExport/Makefile \
  src/STEPImport/Makefile \
  src/STLExport/Makefile \
  src/ShHealOper/Makefile \
  src/TransformationGUI/Makefile \
  src/VTKExport/Makefile \
  resources/Makefile \
  resources/GEOMCatalog.xml \
  idl/Makefile \
  Makefile \
])

AC_HACK_LIBTOOL
