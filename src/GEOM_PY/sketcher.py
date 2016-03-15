# -*- coding: iso-8859-1 -*-
#
# Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

## \defgroup simplesketcher sketcher - Simplified sketcher API
#  \{
#  \details
#  This module provides simplified access to the 2D sketcher functionality of Geometry module.
#  
#  \note This module is deprecated, refer to \ref gsketcher for more details
#  \}

"""
This module provides the user with a simple python API to realize
various sketches from the GEOM text user interface.

Example::

    from salome.geom.sketcher import Sketcher

    # Create the sketch
    mysketcher = Sketcher()
    mysketcher.startAtPoint(0,0)
    mysketcher.translateToPoint(100,0)
    mysketcher.translateWithAngleAndLength(30,100)
    mysketcher.intersectYWithAngle(15,100)
    mysketcher.close()

    # Get the geom object
    wire = mysketcher.getGeomWire()

    # Put it in the study
    from salome.geom import geomBuilder
    geompy = geomBuilder.New(salome.myStudy)
    geompy.addToStudy( wire, 'mysketch' )

Additionnal examples can be found as unit tests in the source code.
"""

geompyEnable = True
try:
    from salome.kernel.deprecation import is_called_by_sphinx
    if not is_called_by_sphinx():
        import salome
        salome.salome_init()
        import GEOM
        from salome.geom import geomBuilder
        geompy = geomBuilder.New(salome.myStudy)
        pass
    pass
except:
    geompyEnable = False
    pass

##
#  \ingroup simplesketcher
class Sketcher:

    __sketch = None
    __xstart = 0
    __ystart = 0

    def __init__(self):
        self.__sketch = "Sketcher:"

    def startAtPoint(self,x=0,y=0):
        self.__sketch = "Sketcher:F "+str(x)+" "+str(y)
        # the data x and y are registered to process the ending closure.
        self.__xstart = x
        self.__ystart = y

    def translateToPoint(self,x=10,y=0):
        '''segment->point->absolute'''
        self.__sketch = self.__sketch + ":TT "+str(x)+" "+str(y) 

    def translateWithVector(self,deltax=10,deltay=0):
        '''segment->point->relative'''
        self.__sketch = self.__sketch + ":T "+str(deltax)+" "+str(deltay) 

    #def translateToSelection(self,vertex):
    #    '''segment->point->selection'''
    #    self.__sketch = self.__sketch + ":T "+str(deltax)+" "+str(deltay) 

    def translateWithAngleAndLength(self,angle=30,length=100):
        '''segment->direction->angle+length'''
        self.__sketch = self.__sketch + ":R "+str(angle)+":L "+str(length)

    def intersectXWithAngle(self,angle=30,x=100):
        '''segment->direction->angle+length'''
        self.__sketch = self.__sketch + ":R "+str(angle)+":IX "+str(x)

    def intersectYWithAngle(self,angle=30,y=100):
        '''segment->direction->angle+length'''
        self.__sketch = self.__sketch + ":R "+str(angle)+":IY "+str(y)

    def close(self):
        '''
        add a segment to reach the starting point and get a closed wire.
        '''
        self.__sketch = self.__sketch + ":WW"

    def toString(self):
        return self.__sketch

    def getGeomWire(self):
        if not geompyEnable:
            return None
        wire = geompy.MakeSketcher(self.__sketch, [0, 0, 0, 0, 0, 1, 1, 0, -0])
        return wire
#
# ===============================================================
# Use cases and unit tests
# ===============================================================
#
def TEST_toString():
    mysketcher = Sketcher()

    mysketcher.startAtPoint(1.234,4.321)
    mysketcher.translateToPoint(2.234,5.321)

    expectedResult = "Sketcher:F 1.234 4.321:TT 2.234 5.321"
    result = mysketcher.toString()
    print "sketcher=",mysketcher.toString()
    if result == expectedResult:
        print "OK"
    else:
        print "Not OK"

def TEST_usingGeom():
    mysketcher = Sketcher()
    mysketcher.startAtPoint(0,0)
    mysketcher.translateToPoint(100,0)
    mysketcher.translateWithAngleAndLength(30,100)
    mysketcher.intersectYWithAngle(15,100)
    mysketcher.close()
    wire = mysketcher.getGeomWire()
    if geompyEnable:
        geompy.addToStudy( wire, "Sketch" )
        

if __name__ == "__main__":
    #TEST_toString()
    TEST_usingGeom()
