#  GEOM GEOM_SWIG : binding of C++ omplementaion with Python
#
#  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
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
#  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
#
#
#
#  File   : geompy.py
#  Author : Paul RASCLE, EDF
#  Module : GEOM
#  $Header$

import salome
import SALOMEDS
#import SALOMEDS_Attributes_idl

#NRI : BugID 1682 : from libSALOME_Swig import *

geom = salome.lcc.FindOrLoadComponent("FactoryServer", "GEOM")
geom.GetCurrentStudy(salome.myStudyId)
myBuilder = salome.myStudy.NewBuilder()

gg = salome.ImportComponentGUI("GEOM")

father = salome.myStudy.FindComponent("GEOM")
if father is None:
        father = myBuilder.NewComponent("GEOM")
        A1 = myBuilder.FindOrCreateAttribute(father, "AttributeName");
        FName = A1._narrow(SALOMEDS.AttributeName)
        FName.SetValue( salome.sg.getComponentUserName("GEOM") )
      	A2 = myBuilder.FindOrCreateAttribute(father, "AttributePixMap");
      	aPixmap = A2._narrow(SALOMEDS.AttributePixMap);
	aPixmap.SetPixMap( "ICON_OBJBROWSER_Geometry" );
	myBuilder.DefineComponentInstance(father,geom)


# -----------------------------------------------------------------------------
# add To Study  
# -----------------------------------------------------------------------------

def SubShapeName(aSubId, aMainId):
    index = gg.getIndexTopology(aSubId, aMainId)
    name = gg.getShapeTypeString(aSubId) + "_%d"%(index)
    return name

def addArguments(aShape):
    ListIOR = []
    ListIOR = geom.GetReferencedObjects(aShape)

    if aShape._get_StudyShapeId()!="":
    	father = salome.IDToSObject(aShape._get_StudyShapeId())

	myBuilder.NewCommand()
    	if len(ListIOR) > 0:
	  Arg = myBuilder.NewObject(father)
          A1 = myBuilder.FindOrCreateAttribute(Arg, "AttributeName");
          ArgName = A1._narrow(SALOMEDS.AttributeName)
          ArgName.SetValue("Arguments")
    	  A2 = myBuilder.FindOrCreateAttribute(Arg, "AttributeSelectable");
	  SelAttr = A2._narrow(SALOMEDS.AttributeSelectable);
    	  SelAttr.SetSelectable(0);
	
	  OneObject = 0
	  for anIOR in ListIOR:
	     Shape = salome.orb.string_to_object(anIOR)
	     if Shape is not None:
		if Shape._get_StudyShapeId()!="":
		   Obj = salome.IDToSObject(Shape._get_StudyShapeId())
		   if Obj is not None:
			   Obj1 = myBuilder.NewObject(Arg)
			   myBuilder.Addreference(Obj1,Obj)
			   OneObject = 1

	  if OneObject == 0:
	     myBuilder.RemoveObject(Arg)

    myBuilder.CommitCommand()
    return 1	

def addToStudy(aShape, aName):
    myBuilder.NewCommand()
    newObj = myBuilder.NewObject(father)
    ior = salome.orb.object_to_string(aShape)
    A1 = myBuilder.FindOrCreateAttribute(newObj, "AttributeIOR");
    ObjIOR = A1._narrow(SALOMEDS.AttributeIOR)
    ObjIOR.SetValue(ior)
    A2 = myBuilder.FindOrCreateAttribute(newObj, "AttributeName");
    ObjName = A2._narrow(SALOMEDS.AttributeName)
    ObjName.SetValue(aName)
    A3 = myBuilder.FindOrCreateAttribute(newObj, "AttributePixMap");
    ObjPixmap = A3._narrow(SALOMEDS.AttributePixMap)
    anIcon = gg.getShapeTypeIcon(ior);
    ObjPixmap.SetPixMap(anIcon)
    id = newObj.GetID()
    aShape._set_StudyShapeId(id)
    myBuilder.CommitCommand()

    addArguments( aShape )

#NRI : BugID 1682 :     sg = SALOMEGUI_Swig()
#NRI : BugID 1682 :     sg.updateObjBrowser(0)
#    salome.sg.updateObjBrowser(0)
    return id

def addToStudyInFather(aFather, aShape, aName):

#NRI : BugID 1682 :     sg = SALOMEGUI_Swig()
    myBuilder.NewCommand()
    newObj = myBuilder.NewObject( salome.IDToSObject(aFather._get_StudyShapeId()) )
    ior = salome.orb.object_to_string(aShape)
    A1 = myBuilder.FindOrCreateAttribute(newObj, "AttributeIOR");
    ObjIOR = A1._narrow(SALOMEDS.AttributeIOR)
    ObjIOR.SetValue(ior)
    A2 = myBuilder.FindOrCreateAttribute(newObj, "AttributeName");
    ObjName = A2._narrow(SALOMEDS.AttributeName)
    ObjName.SetValue(aName)
    A3 = myBuilder.FindOrCreateAttribute(newObj, "AttributePixMap");
    ObjPixmap = A3._narrow(SALOMEDS.AttributePixMap)
    anIcon = gg.getShapeTypeIcon(ior);
    ObjPixmap.SetPixMap(anIcon)
    id = newObj.GetID()
    aShape._set_StudyShapeId(id)
    myBuilder.CommitCommand()

    addArguments( aShape )

#NRI : BugID 1682 :     sg.updateObjBrowser(0)
#    salome.sg.updateObjBrowser(0)
    return id

# -----------------------------------------------------------------------------
# Create Geometry 2D
# -----------------------------------------------------------------------------

def MakeVertex(x,y,z):
    anObj = geom.MakeVertex(x,y,z)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def MakeVector(p1,p2):
    anObj = geom.MakeVector(p1,p2)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def MakeLine(p1,d1):
    anObj = geom.MakeLine(p1,d1)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def MakeArc(p1,p2,p3):
    anObj = geom.MakeArc(p1,p2,p3)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def MakeCircle(p1,d1,radius):  
    anObj = geom.MakeCircle(p1,d1,radius)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def MakePlane(p1,d1,trimsize): 
    anObj = geom.MakePlane(p1,d1,trimsize)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

# -----------------------------------------------------------------------------
# Create Geometry 3D
# -----------------------------------------------------------------------------

def MakeBox(x1,y1,z1,x2,y2,z2):
    anObj = geom.MakeBox(x1,y1,z1,x2,y2,z2)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def MakeCylinder(p1,d1,radius,height):
    anObj = geom.MakeCylinder(p1,d1,radius,height)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def MakeSphere(x,y,z,radius):
    anObj = geom.MakeSphere(x,y,z,radius)	
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def MakeCone(p1,d1,radius1,radius2,height):
    anObj = geom.MakeCone(p1,d1,radius1,radius2,height)	
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def MakeTorus(p1,d1,major_radius,minor_radius):
    anObj = geom.MakeTorus(p1,d1,major_radius,minor_radius)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

# -----------------------------------------------------------------------------
# Create base objects
# -----------------------------------------------------------------------------

def MakeEdge(p1,p2):
    anObj = geom.MakeEdge(p1,p2)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def MakeWire(ListShape):
    anObj = geom.MakeWire(ListShape)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def MakeFace(aShapeWire,WantPlanarFace):
    anObj = geom.MakeFace(aShapeWire,WantPlanarFace)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def MakeCompound(ListShape):
    anObj = geom.MakeCompound(ListShape)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

# -----------------------------------------------------------------------------
# Create advanced objects
# -----------------------------------------------------------------------------

def MakeCopy(aShape):
    anObj = geom.MakeCopy(aShape)	
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def MakePrism(baseShape,p1,p2):
    anObj = geom.MakePrism(baseShape,p1,p2)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def MakeRevolution(aShape,axis,angle):    
    anObj = geom.MakeRevolution(aShape,axis,angle)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def MakeFilling(aShape,mindeg,maxdeg,tol3d,tol2d,nbiter):
    anObj = geom.MakeFilling(aShape,mindeg,maxdeg,tol3d,tol2d,nbiter)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def MakePipe(pathShape,baseShape):
    anObj = geom.MakePipe(pathShape,baseShape)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def MakeSewing(ListShape,precision):
    anObj = geom.MakeSewing(ListShape,precision)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

# -----------------------------------------------------------------------------
# Boolean (Common, Cut, Fuse, Section)
# -----------------------------------------------------------------------------

def MakeBoolean(shape1,shape2,operation):
    anObj = geom.MakeBoolean(shape1,shape2,operation)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

# -----------------------------------------------------------------------------
# Transform objects
# -----------------------------------------------------------------------------

def MakeTranslation(aShape,x,y,z):
    anObj = geom.MakeTranslation(aShape,x,y,z)	
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def MakeRotation(aShape,axis,angle):
    anObj = geom.MakeRotation(aShape,axis,angle)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def MakeScaleTransform(aShape,theCenterofScale,factor): 
    anObj = geom.MakeScaleTransform(aShape,theCenterofScale,factor)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def MakeMirrorByPlane(aShape,aPlane):
    anObj = geom.MakeMirrorByPlane(aShape,aPlane)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def OrientationChange(aShape):
    anObj = geom.OrientationChange(aShape)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def MakeFillet(aShape,radius,ShapeType,ListShape):
    anObj = geom.MakeFillet(aShape,radius,ShapeType,ListShape)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def MakeChamfer(aShape,d1,d2,ShapeType,ListShape):
    anObj = geom.MakeChamfer(aShape,d1,d2,ShapeType,ListShape)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

# -----------------------------------------------------------------------------
# Decompose objects
# -----------------------------------------------------------------------------

def SubShape(aShape,type,ListOfId):
    anObj = geom.SubShape(aShape,type, ListOfId)	
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def SubShapeAll(aShape,type):
    ListObj = geom.SubShapeAll(aShape,type)
    for anObj in ListObj :
	    ior = salome.orb.object_to_string(anObj)
	    anObj._set_Name(ior)
    return ListObj

def SubShapeSorted(aShape,type,ListOfId):
    anObj = geom.SubShapeSorted(aShape,type, ListOfId)	
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def SubShapeAllSorted(aShape,type):
    ListObj = geom.SubShapeAllSorted(aShape,type)
    for anObj in ListObj :
	    ior = salome.orb.object_to_string(anObj)
	    anObj._set_Name(ior)
    return ListObj

# -- enumeration ShapeType as a dictionary --
ShapeType = {"COMPOUND":0, "COMPSOLID":1, "SOLID":2, "SHELL":3, "FACE":4, "WIRE":5, "EDGE":6, "VERTEX":7, "SHAPE":8}

def Partition(ListShapes, ListTools=[], ListKeepInside=[], ListRemoveInside=[], Limit=ShapeType["SHAPE"]):
    anObj = geom.Partition(ListShapes, ListTools, ListKeepInside, ListRemoveInside, Limit);
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def SuppressFaces(aShape,ListOfId):
    anObj = geom.SuppressFaces(aShape,ListOfId)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def SuppressHole(aShape,ListOfFace,ListOfWire,ListOfEndFace):
    anObj = geom.SuppressHole(aShape,ListOfFace,ListOfWire,ListOfEndFace)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

# -----------------------------------------------------------------------------
# Patterns
# -----------------------------------------------------------------------------

def MakeMultiTranslation1D(aShape,aDir,aStep,aNbTimes):
    anObj = geom.MakeMultiTranslation1D(aShape,aDir,aStep,aNbTimes)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def MakeMultiTranslation2D(aShape,d1,step1,nbtimes1,d2,step2,nbtimes2):
    anObj = geom.MakeMultiTranslation2D(aShape,d1,step1,nbtimes1,d2,step2,nbtimes2)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def MakeMultiRotation1D(aShape,aDir,aPoint,aNbTimes):
    anObj = geom.MakeMultiRotation1D(aShape,aDir,aPoint,aNbTimes)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def MakeMultiRotation2D(aShape,aDir,aPoint,anAngle,nbtimes1,aStep,nbtimes2):
    anObj = geom.MakeMultiRotation2D(aShape,aDir,aPoint,anAngle,nbtimes1,aStep,nbtimes2)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

# -----------------------------------------------------------------------------
# Import objects
# -----------------------------------------------------------------------------

def ImportBREP(filename):
    anObj = geom.ImportBREP(filename)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def ImportIGES(filename):
    anObj = geom.ImportIGES(filename)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def ImportSTEP(filename):
    anObj = geom.ImportSTEP(filename)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

# -----------------------------------------------------------------------------
# Export objects
# -----------------------------------------------------------------------------
def ExportBREP(filename,aShape):
    geom.ExportBREP(filename,aShape)

def ExportIGES(filename,aShape):
    geom.ExportIGES(filename,aShape)

def ExportSTEP(filename,aShape):
    geom.ExportSTEP(filename,aShape)

# -----------------------------------------------------------------------------
# Information objects
# -----------------------------------------------------------------------------

def MakeCDG(aShape):	
    anObj = geom.MakeCDG(aShape)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def Archimede(aShape,weight,WaterDensity,MeshingDeflection):	
    anObj = geom.Archimede(aShape,weight,WaterDensity,MeshingDeflection)
    ior = salome.orb.object_to_string(anObj)
    anObj._set_Name(ior)
    return anObj

def CheckShape(aShape):	
    Status = geom.CheckShape(aShape)
    return Status
