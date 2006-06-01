# Check availability of Geom binary distribution
#
# Author : Nicolas REJNERI (OPEN CASCADE, 2003)
#

AC_DEFUN([CHECK_GEOM],[

AC_CHECKING(for Geom)

Geom_ok=no

GEOM_LDFLAGS=""
GEOM_CXXFLAGS=""

AC_ARG_WITH(geom,
	    [  --with-geom=DIR root directory path of GEOM installation ],
	    GEOM_DIR="$withval",GEOM_DIR="")

if test "x$GEOM_DIR" == "x" ; then

# no --with-geom-dir option used

   if test "x$GEOM_ROOT_DIR" != "x" ; then

    # GEOM_ROOT_DIR environment variable defined
      GEOM_DIR=$GEOM_ROOT_DIR

   else

    # search Geom binaries in PATH variable
      AC_PATH_PROG(TEMP, libGEOM_Swig.py)
      if test "x$TEMP" != "x" ; then
         GEOM_BIN_DIR=`dirname $TEMP`
         GEOM_DIR=`dirname $GEOM_BIN_DIR`
      fi
      
   fi
# 
fi

if test -f ${GEOM_DIR}/bin/salome/libGEOM_Swig.py ; then
   Geom_ok=yes
   AC_MSG_RESULT(Using Geom module distribution in ${GEOM_DIR})

   if test "x$GEOM_ROOT_DIR" == "x" ; then
      GEOM_ROOT_DIR=${GEOM_DIR}
   fi
   AC_SUBST(GEOM_ROOT_DIR)

   GEOM_LDFLAGS=-L${GEOM_DIR}/lib${LIB_LOCATION_SUFFIX}/salome
   GEOM_CXXFLAGS=-I${GEOM_DIR}/include/salome

   AC_SUBST(GEOM_LDFLAGS)
   AC_SUBST(GEOM_CXXFLAGS)

else
   AC_MSG_WARN("Cannot find compiled Geom module distribution")
fi

AC_MSG_RESULT(for Geom: $Geom_ok)
 
])dnl
 
