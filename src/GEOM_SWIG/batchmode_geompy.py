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
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
#
#
#  File   : geompy.py
#  Author : Paul RASCLE, EDF
#  Module : GEOM
#  $Header$

from batchmode_salome import *
import GEOM

g=None
step = 0
while step < 50 and g == None:
    g = lcc.FindOrLoadComponent("FactoryServer", "GEOM")
    step = step + 1
    time.sleep(4)
geom = g._narrow( GEOM.GEOM_Gen )

myBuilder = None
myStudyId = 0
father    = None

BasicOp  = None
CurvesOp = None
PrimOp   = None
ShapesOp = None
HealOp   = None
InsertOp = None
BoolOp   = None
TrsfOp   = None
LocalOp  = None
MeasuOp  = None
BlocksOp = None
GroupOp  = None

def init_geom(theStudy):

    global myStudy, myBuilder, myStudyId, BasicOp, CurvesOp, PrimOp, ShapesOp, HealOp
    global InsertOp, BoolOp, TrsfOp, LocalOp, MeasuOp, BlocksOp, GroupOp, father

    myStudy = theStudy
    myStudyId = myStudy._get_StudyId()
    myBuilder = myStudy.NewBuilder()
    father = myStudy.FindComponent("GEOM")
    if father is None:
        father = myBuilder.NewComponent("GEOM")
        A1 = myBuilder.FindOrCreateAttribute(father, "AttributeName")
        FName = A1._narrow(SALOMEDS.AttributeName)
        FName.SetValue("Geometry")
      	A2 = myBuilder.FindOrCreateAttribute(father, "AttributePixMap")
      	aPixmap = A2._narrow(SALOMEDS.AttributePixMap)
	aPixmap.SetPixMap("ICON_OBJBROWSER_Geometry")
	myBuilder.DefineComponentInstance(father,geom)
        pass

    # -----------------------------------------------------------------------------
    # Assign Operations Interfaces
    # -----------------------------------------------------------------------------

    BasicOp  = geom.GetIBasicOperations    (myStudyId)
    CurvesOp = geom.GetICurvesOperations   (myStudyId)
    PrimOp   = geom.GetI3DPrimOperations   (myStudyId)
    ShapesOp = geom.GetIShapesOperations   (myStudyId)
    HealOp   = geom.GetIHealingOperations  (myStudyId)
    InsertOp = geom.GetIInsertOperations   (myStudyId)
    BoolOp   = geom.GetIBooleanOperations  (myStudyId)
    TrsfOp   = geom.GetITransformOperations(myStudyId)
    LocalOp  = geom.GetILocalOperations    (myStudyId)
    MeasuOp  = geom.GetIMeasureOperations  (myStudyId)
    BlocksOp = geom.GetIBlocksOperations   (myStudyId)
    GroupOp  = geom.GetIGroupOperations   (myStudyId)
    pass

init_geom(myStudy)

#     *  Get name for sub-shape aSubObj of shape aMainObj
#
def SubShapeName(aSubObj, aMainObj):
    name = "SubShape"
    print name
    return name

#     *  Publish in study aShape with name aName
#
def addToStudy(aShape, aName):
    try:
        aSObject = geom.AddInStudy(myStudy, aShape, aName, None)
    except:
        print "addToStudy() failed"
        return ""
    return aShape.GetStudyEntry()

#     *  Publish in study aShape with name aName as sub-object of previously published aFather
#
def addToStudyInFather(aFather, aShape, aName):
    try:
        aSObject = geom.AddInStudy(myStudy, aShape, aName, aFather)
    except:
        print "addToStudyInFather() failed"
        return ""
    return aShape.GetStudyEntry()

# -----------------------------------------------------------------------------
# enumeration ShapeType as a dictionary
# -----------------------------------------------------------------------------

ShapeType = {"COMPOUND":0, "COMPSOLID":1, "SOLID":2, "SHELL":3, "FACE":4, "WIRE":5, "EDGE":6, "VERTEX":7, "SHAPE":8}

# -----------------------------------------------------------------------------
# Basic primitives
# -----------------------------------------------------------------------------

def MakeVertex(theX, theY, theZ):
    anObj = BasicOp.MakePointXYZ(theX, theY, theZ)
    if BasicOp.IsDone() == 0:
      print "MakePointXYZ : ", BasicOp.GetErrorCode()
    return anObj

def MakeVertexWithRef(vertex,x,y,z):
    anObj = BasicOp.MakePointWithReference(vertex,x,y,z)
    if BasicOp.IsDone() == 0:
      print "MakePointWithReference : ", BasicOp.GetErrorCode()
    return anObj

def MakeVertexOnCurve(curve,par):
    anObj = BasicOp.MakePointOnCurve(curve,par)
    if BasicOp.IsDone() == 0:
      print "MakePointOnCurve : ", BasicOp.GetErrorCode()
    return anObj

def MakeVectorDXDYDZ(dx,dy,dz):
    anObj = BasicOp.MakeVectorDXDYDZ(dx,dy,dz)
    if BasicOp.IsDone() == 0:
      print "MakeVectorDXDYDZ : ", BasicOp.GetErrorCode()
    return anObj

def MakeVector(p1,p2):
    anObj = BasicOp.MakeVectorTwoPnt(p1, p2)
    if BasicOp.IsDone() == 0:
      print "MakeVectorTwoPnt : ", BasicOp.GetErrorCode()
    return anObj

def MakeLine(p1, d1):
    anObj = BasicOp.MakeLine(p1,d1)
    if BasicOp.IsDone() == 0:
      print "MakeLine : ", BasicOp.GetErrorCode()
    return anObj

def MakeLineTwoPnt(p1, p2):
    anObj = BasicOp.MakeLineTwoPnt(p1,p2)
    if BasicOp.IsDone() == 0:
      print "MakeLineTwoPnt : ", BasicOp.GetErrorCode()
    return anObj

def MakePlane(p1,v1,trimsize):
    anObj = BasicOp.MakePlanePntVec(p1,v1,trimsize)
    if BasicOp.IsDone() == 0:
      print "MakePlanePntVec : ", BasicOp.GetErrorCode()
    return anObj

def MakePlaneThreePnt(p1,p2,p3,trimsize):
    anObj = BasicOp.MakePlaneThreePnt(p1,p2,p3,trimsize)
    if BasicOp.IsDone() == 0:
      print "MakePlaneThreePnt : ", BasicOp.GetErrorCode()
    return anObj

def MakePlaneFace(face,trimsize):
    anObj = BasicOp.MakePlaneFace(face,trimsize)
    if BasicOp.IsDone() == 0:
      print "MakePlaneFace : ", BasicOp.GetErrorCode()
    return anObj

def MakeMarker(OX,OY,OZ, XDX,XDY,XDZ, YDX,YDY,YDZ):
    anObj = BasicOp.MakeMarker(OX,OY,OZ, XDX,XDY,XDZ, YDX,YDY,YDZ)
    if BasicOp.IsDone() == 0:
      print "MakeMarker : ", BasicOp.GetErrorCode()
    return anObj

def MakeMarkerPntTwoVec(theOrigin, theXVec, theYVec):
    """
     *  Create a local coordinate system.
     *  \param theOrigin Point of coordinate system origin.
     *  \param theXVec Vector of X direction
     *  \param theYVec Vector of Y direction
     *  \return New GEOM_Object, containing the created coordinate system.
    """
    O = PointCoordinates( theOrigin )
    OXOY = []
    for vec in [ theXVec, theYVec ]:
        v1, v2 = SubShapeAll( vec, ShapeType["VERTEX"] )
        p1 = PointCoordinates( v1 )
        p2 = PointCoordinates( v2 )
        for i in range( 0, 3 ):
             OXOY.append( p2[i] - p1[i] )
    #
    anObj = BasicOp.MakeMarker( O[0], O[1], O[2],
                                OXOY[0], OXOY[1], OXOY[2],
                                OXOY[3], OXOY[4], OXOY[5], )
    if BasicOp.IsDone() == 0:
      print "MakeMarker : ", BasicOp.GetErrorCode()
    return anObj

# -----------------------------------------------------------------------------
# Curves
# -----------------------------------------------------------------------------

def MakeArc(p1,p2,p3):
    anObj = CurvesOp.MakeArc(p1,p2,p3)
    if CurvesOp.IsDone() == 0:
      print "MakeArc : ", CurvesOp.GetErrorCode()
    return anObj

def MakeCircle(p1,v1,radius):
    anObj = CurvesOp.MakeCirclePntVecR(p1,v1,radius)
    if CurvesOp.IsDone() == 0:
      print "MakeCirclePntVecR : ", CurvesOp.GetErrorCode()
    return anObj

def MakeCircleThreePnt(p1,p2,p3):
    anObj = CurvesOp.MakeCircleThreePnt(p1,p2,p3)
    if CurvesOp.IsDone() == 0:
      print "MakeCircleThreePnt : ", CurvesOp.GetErrorCode()
    return anObj

def MakeEllipse(p1,v1,radiusMaj,radiusMin):
    anObj = CurvesOp.MakeEllipse(p1,v1,radiusMaj, radiusMin)
    if CurvesOp.IsDone() == 0:
      print "MakeEllipse : ", CurvesOp.GetErrorCode()
    return anObj

def MakePolyline(ListShape):
    anObj = CurvesOp.MakePolyline(ListShape)
    if CurvesOp.IsDone() == 0:
      print "MakePolyline : ", CurvesOp.GetErrorCode()
    return anObj

def MakeBezier(ListShape):
    anObj = CurvesOp.MakeSplineBezier(ListShape)
    if CurvesOp.IsDone() == 0:
      print "MakeSplineBezier : ", CurvesOp.GetErrorCode()
    return anObj

def MakeInterpol(ListShape):
    anObj = CurvesOp.MakeSplineInterpolation(ListShape)
    if CurvesOp.IsDone() == 0:
      print "MakeSplineInterpolation : ", CurvesOp.GetErrorCode()
    return anObj

# <WPL>: Nine double values, defining origin,
# OZ and OX directions of the working plane.
def MakeSketcher(Cmd, WPL = [0,0,0, 0,0,1, 1,0,0]):
    anObj = CurvesOp.MakeSketcher(Cmd, WPL)
    if CurvesOp.IsDone() == 0:
      print "MakeSketcher : ", CurvesOp.GetErrorCode()
    return anObj

def MakeSketcherOnPlane(theCommand, theWorkingPlane):
    """
     *  Create a sketcher (wire or face), following the textual description,
     *  passed through \a theCommand argument. \n
     *  For format of the description string see the previous method.\n
     *  \param theCommand String, defining the sketcher in local
     *                    coordinates of the working plane.
     *  \param theWorkingPlane Planar Face of the working plane.
     *  \return New GEOM_Object, containing the created wire.
    """
    anObj = CurvesOp.MakeSketcherOnPlane(theCommand, theWorkingPlane)
    if CurvesOp.IsDone() == 0:
      print "MakeSketcher : ", CurvesOp.GetErrorCode()
    return anObj

# -----------------------------------------------------------------------------
# Create 3D Primitives
# -----------------------------------------------------------------------------

def MakeBox(x1,y1,z1,x2,y2,z2):
    pnt1 = MakeVertex(x1,y1,z1)
    pnt2 = MakeVertex(x2,y2,z2)
    return MakeBoxTwoPnt(pnt1,pnt2)

def MakeBoxDXDYDZ(dx,dy,dz):
    anObj = PrimOp.MakeBoxDXDYDZ(dx,dy,dz)
    if PrimOp.IsDone() == 0:
      print "MakeBoxDXDYDZ : ", PrimOp.GetErrorCode()
    return anObj

def MakeBoxTwoPnt(point1, point2):
    anObj = PrimOp.MakeBoxTwoPnt(point1, point2)
    if PrimOp.IsDone() == 0:
      print "MakeBoxTwoPnt : ", PrimOp.GetErrorCode()
    return anObj

def MakeCylinder(p1,v1,radius,height):
    anObj = PrimOp.MakeCylinderPntVecRH(p1,v1,radius,height)
    if PrimOp.IsDone() == 0:
      print "MakeCylinderPntVecRH : ", PrimOp.GetErrorCode()
    return anObj

def MakeCylinderRH(radius,height):
    anObj = PrimOp.MakeCylinderRH(radius,height)
    if PrimOp.IsDone() == 0:
      print "MakeCylinderRH : ", PrimOp.GetErrorCode()
    return anObj

def MakeSpherePntR(point,radius):
    anObj = PrimOp.MakeSpherePntR(point,radius)
    if PrimOp.IsDone() == 0:
      print "MakeSpherePntR : ", PrimOp.GetErrorCode()
    return anObj

def MakeSphere(x,y,z,radius):
    point = MakeVertex(x,y,z)
    anObj = MakeSpherePntR(point,radius)
    return anObj

def MakeSphereR(radius):
    anObj = PrimOp.MakeSphereR(radius)
    if PrimOp.IsDone() == 0:
      print "MakeSphereR : ", PrimOp.GetErrorCode()
    return anObj

def MakeCone(p1,v1,radius1,radius2,height):
    anObj = PrimOp.MakeConePntVecR1R2H(p1,v1,radius1,radius2,height)
    if PrimOp.IsDone() == 0:
      print "MakeConePntVecR1R2H : ", PrimOp.GetErrorCode()
    return anObj

def MakeConeR1R2H(radius1,radius2,height):
    anObj = PrimOp.MakeConeR1R2H(radius1,radius2,height)
    if PrimOp.IsDone() == 0:
      print "MakeConeR1R2H : ", PrimOp.GetErrorCode()
    return anObj

def MakeTorus(p1,v1,major_radius,minor_radius):
    anObj = PrimOp.MakeTorusPntVecRR(p1,v1,major_radius,minor_radius)
    if PrimOp.IsDone() == 0:
      print "MakeTorusPntVecRR : ", PrimOp.GetErrorCode()
    return anObj

def MakeTorusRR(major_radius,minor_radius):
    anObj = PrimOp.MakeTorusRR(major_radius,minor_radius)
    if PrimOp.IsDone() == 0:
      print "MakeTorusRR : ", PrimOp.GetErrorCode()
    return anObj

def MakePrism(baseShape,point1,point2):
    anObj = PrimOp.MakePrismTwoPnt(baseShape,point1,point2)
    if PrimOp.IsDone() == 0:
      print "MakePrismTwoPnt : ", PrimOp.GetErrorCode()
    return anObj

def MakePrismVecH(baseShape,vector,height):
    anObj = PrimOp.MakePrismVecH(baseShape,vector,height)
    if PrimOp.IsDone() == 0:
      print "MakePrismVecH : ", PrimOp.GetErrorCode()
    return anObj

def MakePipe(baseShape,pathShape):
    anObj = PrimOp.MakePipe(baseShape,pathShape)
    if PrimOp.IsDone() == 0:
      print "MakePipe : ", PrimOp.GetErrorCode()
    return anObj

def MakeRevolution(aShape,axis,angle):
    anObj = PrimOp.MakeRevolutionAxisAngle(aShape,axis,angle)
    if PrimOp.IsDone() == 0:
      print "MakeRevolutionAxisAngle : ", PrimOp.GetErrorCode()
    return anObj

# -----------------------------------------------------------------------------
# Create base shapes
# -----------------------------------------------------------------------------

def MakeEdge(p1,p2):
    anObj = ShapesOp.MakeEdge(p1,p2)
    if ShapesOp.IsDone() == 0:
      print "MakeEdge : ", ShapesOp.GetErrorCode()
    return anObj

def MakeWire(ListShape):
    anObj = ShapesOp.MakeWire(ListShape)
    if ShapesOp.IsDone() == 0:
      print "MakeWire : ", ShapesOp.GetErrorCode()
    return anObj

def MakeFace(aShapeWire,WantPlanarFace):
    anObj = ShapesOp.MakeFace(aShapeWire,WantPlanarFace)
    if ShapesOp.IsDone() == 0:
      print "MakeFace : ", ShapesOp.GetErrorCode()
    return anObj

def MakeFaceWires(ListWires,WantPlanarFace):
    anObj = ShapesOp.MakeFaceWires(ListWires,WantPlanarFace)
    if ShapesOp.IsDone() == 0:
      print "MakeFaceWires : ", ShapesOp.GetErrorCode()
    return anObj

def MakeFaces(ListWires,WantPlanarFace):
    anObj = MakeFaceWires(ListWires,WantPlanarFace)
    return anObj

def MakeShell(ListOfShapes):
    anObj = ShapesOp.MakeShell(ListOfShapes)
    if ShapesOp.IsDone() == 0:
	print "MakeShell : ", ShapesOp.GetErrorCode()
    return anObj

def MakeSolid(ListOfShells):
    anObj = ShapesOp.MakeSolidShells(ListOfShells)
    if ShapesOp.IsDone() == 0:
	print "MakeSolid : ", ShapesOp.GetErrorCode()
    return anObj

def MakeCompound(ListShape):
    anObj = ShapesOp.MakeCompound(ListShape)
    if ShapesOp.IsDone() == 0:
      print "MakeCompound : ", ShapesOp.GetErrorCode()
    return anObj

def NumberOfFaces(theShape):
    nb_faces = ShapesOp.NumberOfFaces(theShape)
    if ShapesOp.IsDone() == 0:
      print "NumberOfFaces : ", ShapesOp.GetErrorCode()
    return nb_faces

def NumberOfEdges(theShape):
    nb_edges = ShapesOp.NumberOfEdges(theShape)
    if ShapesOp.IsDone() == 0:
      print "NumberOfEdges : ", ShapesOp.GetErrorCode()
    return nb_edges

def ChangeOrientation(Shape):
    anObj = ShapesOp.ChangeOrientation(Shape)
    if ShapesOp.IsDone() == 0:
      print "ChangeOrientation : ", ShapesOp.GetErrorCode()
    return anObj

def OrientationChange(Shape):
    anObj = ChangeOrientation(Shape)
    return anObj

def GetFreeFacesIDs(theShape):
    anIDs = ShapesOp.GetFreeFacesIDs(theShape)
    if ShapesOp.IsDone() == 0:
      print "GetFreeFacesIDs : ", ShapesOp.GetErrorCode()
    return anIDs

def GetSharedShapes(theShape1, theShape2, theShapeType):
    aList = ShapesOp.GetSharedShapes(theShape1, theShape2, theShapeType)
    if ShapesOp.IsDone() == 0:
      print "GetSharedShapes : ", ShapesOp.GetErrorCode()
    return aList

def GetShapesOnPlane(theShape, theShapeType, theAx1, theState):
    aList = ShapesOp.GetShapesOnPlane(theShape, theShapeType, theAx1, theState)
    if ShapesOp.IsDone() == 0:
      print "GetShapesOnPlane : ", ShapesOp.GetErrorCode()
    return aList

def GetShapesOnPlaneIDs(theShape, theShapeType, theAx1, theState):
    aList = ShapesOp.GetShapesOnPlaneIDs(theShape, theShapeType, theAx1, theState)
    if ShapesOp.IsDone() == 0:
        print "GetShapesOnPlaneIDs : ", ShapesOp.GetErrorCode()
    return aList

def GetShapesOnCylinder(theShape, theShapeType, theAxis, theRadius, theState):
    aList = ShapesOp.GetShapesOnCylinder(theShape, theShapeType, theAxis, theRadius, theState)
    if ShapesOp.IsDone() == 0:
      print "GetShapesOnCylinder : ", ShapesOp.GetErrorCode()
    return aList

def GetShapesOnCylinderIDs(theShape, theShapeType, theAxis, theRadius, theState):
    aList = ShapesOp.GetShapesOnCylinderIDs(theShape, theShapeType, theAxis, theRadius, theState)
    if ShapesOp.IsDone() == 0:
        print "GetShapesOnCylinderIDs : ", ShapesOp.GetErrorCode()
    return aList

def GetShapesOnSphere(theShape, theShapeType, theCenter, theRadius, theState):
    aList = ShapesOp.GetShapesOnSphere(theShape, theShapeType, theCenter, theRadius, theState)
    if ShapesOp.IsDone() == 0:
      print "GetShapesOnSphere : ", ShapesOp.GetErrorCode()
    return aList

def GetShapesOnSphereIDs(theShape, theShapeType, theCenter, theRadius, theState):
    aList = ShapesOp.GetShapesOnSphereIDs(theShape, theShapeType, theCenter, theRadius, theState)
    if ShapesOp.IsDone() == 0:
        print "GetShapesOnSphereIDs : ", ShapesOp.GetErrorCode()
    return aList

def GetShapesOnQuadrangle(theShape, theShapeType, theTopLeftPoint, theTopRigthPoint, theBottomLeftPoint, theBottomRigthPoint, theState):
    aList = ShapesOp.GetShapesOnQuadrangle(theShape, theShapeType, theTopLeftPoint, theTopRigthPoint, theBottomLeftPoint, theBottomRigthPoint, theState)
    if ShapesOp.IsDone() == 0:
      print "GetShapesOnQuadrangle : ", ShapesOp.GetErrorCode()
    return aList

def GetShapesOnQuadrangleIDs(theShape, theShapeType, theTopLeftPoint, theTopRigthPoint, theBottomLeftPoint, theBottomRigthPoint, theState):
    aList = ShapesOp.GetShapesOnQuadrangleIDs(theShape, theShapeType, theTopLeftPoint, theTopRigthPoint, theBottomLeftPoint, theBottomRigthPoint, theState)
    if ShapesOp.IsDone() == 0:
        print "GetShapesOnQuadrangleIDs : ", ShapesOp.GetErrorCode()
    return aList

def GetInPlace(theShapeWhere, theShapeWhat):
    anObj = ShapesOp.GetInPlace(theShapeWhere, theShapeWhat)
    if ShapesOp.IsDone() == 0:
      print "GetInPlace : ", ShapesOp.GetErrorCode()
    return anObj

# -----------------------------------------------------------------------------
# Access to sub-shapes by their unique IDs inside the main shape.
# -----------------------------------------------------------------------------

# Obtain a composite sub-shape of <aShape>, composed from sub-shapes
# of <aShape>, selected by their unique IDs inside <aShape>
def GetSubShape(aShape, ListOfID):
    anObj = geom.AddSubShape(aShape,ListOfID)
    return anObj

# Obtain unique ID of sub-shape <aSubShape> inside <aShape>
def GetSubShapeID(aShape, aSubShape):
    anID = LocalOp.GetSubShapeIndex(aShape, aSubShape)
    if LocalOp.IsDone() == 0:
      print "GetSubShapeIndex : ", LocalOp.GetErrorCode()
    return anID

# -----------------------------------------------------------------------------
# Decompose objects
# -----------------------------------------------------------------------------

def SubShapeAll(aShape, aType):
    ListObj = ShapesOp.MakeExplode(aShape,aType,0)
    if ShapesOp.IsDone() == 0:
      print "MakeExplode : ", ShapesOp.GetErrorCode()
    return ListObj

def SubShapeAllIDs(aShape, aType):
    ListObj = ShapesOp.SubShapeAllIDs(aShape,aType,0)
    if ShapesOp.IsDone() == 0:
      print "SubShapeAllIDs : ", ShapesOp.GetErrorCode()
    return ListObj

def SubShapeAllSorted(aShape, aType):
    ListObj = ShapesOp.MakeExplode(aShape,aType,1)
    if ShapesOp.IsDone() == 0:
      print "MakeExplode : ", ShapesOp.GetErrorCode()
    return ListObj

def SubShapeAllSortedIDs(aShape, aType):
    ListIDs = ShapesOp.SubShapeAllIDs(aShape,aType,1)
    if ShapesOp.IsDone() == 0:
      print "SubShapeAllSortedIDs : ", ShapesOp.GetErrorCode()
    return ListObj

# Obtain a compound of sub-shapes of <aShape>,
# selected by they indices in list of all sub-shapes of type <aType>
def SubShape(aShape, aType, ListOfInd):
    ListOfIDs = []
    AllShapeList = SubShapeAll(aShape, aType)
    for ind in ListOfInd:
        ListOfIDs.append(GetSubShapeID(aShape, AllShapeList[ind - 1]))
    anObj = GetSubShape(aShape, ListOfIDs)
    return anObj

# Obtain a compound of sub-shapes of <aShape>,
# selected by they indices in sorted list of all sub-shapes of type <aType>
def SubShapeSorted(aShape, aType, ListOfInd):
    ListOfIDs = []
    AllShapeList = SubShapeAllSorted(aShape, aType)
    for ind in ListOfInd:
        ListOfIDs.append(GetSubShapeID(aShape, AllShapeList[ind - 1]))
    anObj = GetSubShape(aShape, ListOfIDs)
    return anObj

# -----------------------------------------------------------------------------
# Healing operations
# -----------------------------------------------------------------------------

def ProcessShape(Shape, Operators, Parameters, Values):
    anObj = HealOp.ProcessShape(Shape, Operators, Parameters, Values)
    if HealOp.IsDone() == 0:
	print "ProcessShape : ", HealOp.GetErrorCode()
    return anObj

def SuppressFaces(aShape,ListOfId):
    anObj = HealOp.SuppressFaces(aShape,ListOfId)
    if HealOp.IsDone() == 0:
      print "SuppressFaces : ", HealOp.GetErrorCode()
    return anObj

def MakeSewing(ListShape,precision):
    comp = MakeCompound(ListShape)
    anObj = Sew(comp,precision)
    return anObj

def Sew(aShape,precision):
    anObj = HealOp.Sew(aShape,precision)
    if HealOp.IsDone() == 0:
      print "Sew : ", HealOp.GetErrorCode()
    return anObj

def SuppressInternalWires(aShape, Wires):
    anObj = HealOp.RemoveIntWires(aShape, Wires)
    if HealOp.IsDone() == 0:
      print "SuppressInternalWires : ", HealOp.GetErrorCode()
    return anObj

def SuppressHoles(aShape, ListOfId):
    anObj = HealOp.FillHoles(aShape,ListOfId)
    if HealOp.IsDone() == 0:
      print "SuppressHoles : ", HealOp.GetErrorCode()
    return anObj

def CloseContour(aShape, Wires, IsCommonVertex):
    anObj = HealOp.CloseContour(aShape, Wires, IsCommonVertex)
    if HealOp.IsDone() == 0:
      print "CloseContour : ", HealOp.GetErrorCode()
    return anObj

def DivideEdge(aShape, EdgeID, Value, IsByParameter):
    anObj = HealOp.DivideEdge(aShape, EdgeID, Value, IsByParameter)
    if HealOp.IsDone() == 0:
      print "DivideEdge : ", HealOp.GetErrorCode()
    return anObj

def GetFreeBoundary(Shape):
    anObj = HealOp.GetFreeBoundary(Shape)
    if HealOp.IsDone() == 0:
      print "GetFreeBoundaries : ", HealOp.GetErrorCode()
    return anObj

# -----------------------------------------------------------------------------
# Create advanced objects
# -----------------------------------------------------------------------------

def MakeCopy(aShape):
    anObj = InsertOp.MakeCopy(aShape)
    if InsertOp.IsDone() == 0:
      print "MakeCopy : ", InsertOp.GetErrorCode()
    return anObj

def MakeFilling(aShape,mindeg,maxdeg,tol2d,tol3d,nbiter):
    anObj = PrimOp.MakeFilling(aShape,mindeg,maxdeg,tol2d,tol3d,nbiter)
    if PrimOp.IsDone() == 0:
      print "MakeFilling : ", PrimOp.GetErrorCode()
    return anObj

def MakeGlueFaces(aShape,aTolerance):
    anObj = ShapesOp.MakeGlueFaces(aShape,aTolerance)
    if ShapesOp.IsDone() == 0:
      print "MakeGlueFaces : ", ShapesOp.GetErrorCode()
    return anObj

# -----------------------------------------------------------------------------
# Boolean (Common, Cut, Fuse, Section)
# -----------------------------------------------------------------------------

def MakeBoolean(shape1,shape2,operation):
    anObj = BoolOp.MakeBoolean(shape1,shape2,operation)
    if BoolOp.IsDone() == 0:
      print "MakeBoolean : ", BoolOp.GetErrorCode()
    return anObj

def MakeCommon(s1, s2):
    return MakeBoolean(s1, s2, 1)

def MakeCut(s1, s2):
    return MakeBoolean(s1, s2, 2)

def MakeFuse(s1, s2):
    return MakeBoolean(s1, s2, 3)

def MakeSection(s1, s2):
    return MakeBoolean(s1, s2, 4)

def MakePartition(ListShapes, ListTools=[], ListKeepInside=[], ListRemoveInside=[],
                  Limit=ShapeType["SHAPE"], RemoveWebs=0, ListMaterials=[]):
    anObj = BoolOp.MakePartition(ListShapes, ListTools,
                                 ListKeepInside, ListRemoveInside,
                                 Limit, RemoveWebs, ListMaterials);
    if BoolOp.IsDone() == 0:
      print "MakePartition : ", BoolOp.GetErrorCode()
    return anObj

def Partition(ListShapes, ListTools=[], ListKeepInside=[], ListRemoveInside=[],
              Limit=ShapeType["SHAPE"], RemoveWebs=0, ListMaterials=[]):
    anObj = MakePartition(ListShapes, ListTools,
                          ListKeepInside, ListRemoveInside,
                          Limit, RemoveWebs, ListMaterials);
    return anObj

def MakeHalfPartition(theShape, thePlane):
    anObj = BoolOp.MakeHalfPartition(theShape, thePlane)
    if BoolOp.IsDone() == 0:
      print "MakeHalfPartition : ", BoolOp.GetErrorCode()
    return anObj

# -----------------------------------------------------------------------------
# Transform objects
# -----------------------------------------------------------------------------

def MakeTranslationTwoPoints(aShape,point1,point2):
    anObj = TrsfOp.TranslateTwoPointsCopy(aShape,point1,point2)
    if TrsfOp.IsDone() == 0:
      print "TranslateTwoPointsCopy : ", TrsfOp.GetErrorCode()
    return anObj

def MakeTranslation(aShape,dx,dy,dz):
    anObj = TrsfOp.TranslateDXDYDZCopy(aShape,dx,dy,dz)
    if TrsfOp.IsDone() == 0:
      print "TranslateDXDYDZCopy : ", TrsfOp.GetErrorCode()
    return anObj

def MakeTranslationVector(theObject, theVector):
    anObj = TrsfOp.TranslateVectorCopy(theObject, theVector)
    if TrsfOp.IsDone() == 0:
      print "TranslateVectorCopy : ", TrsfOp.GetErrorCode()
    return anObj

def MakeRotation(aShape,axis,angle):
    anObj = TrsfOp.RotateCopy(aShape,axis,angle)
    if TrsfOp.IsDone() == 0:
      print "RotateCopy : ", TrsfOp.GetErrorCode()
    return anObj

def MakeScaleTransform(aShape,theCenterofScale,factor):
    anObj = TrsfOp.ScaleShapeCopy(aShape,theCenterofScale,factor)
    if TrsfOp.IsDone() == 0:
      print "ScaleShapeCopy : ", TrsfOp.GetErrorCode()
    return anObj

def MakeMirrorByPlane(aShape,aPlane):
    anObj = TrsfOp.MirrorPlaneCopy(aShape,aPlane)
    if TrsfOp.IsDone() == 0:
      print "MirrorPlaneCopy : ", TrsfOp.GetErrorCode()
    return anObj

def MakeMirrorByAxis(theObject, theAxis):
    anObj = TrsfOp.MirrorAxisCopy(theObject, theAxis)
    if TrsfOp.IsDone() == 0:
      print "MirrorAxisCopy : ", TrsfOp.GetErrorCode()
    return anObj

def MakeMirrorByPoint(theObject, thePoint):
    anObj = TrsfOp.MirrorPointCopy(theObject, thePoint)
    if TrsfOp.IsDone() == 0:
      print "MirrorPointCopy : ", TrsfOp.GetErrorCode()
    return anObj

def MakePosition(aShape,theStartLCS,theEndLCS):
    anObj = TrsfOp.PositionShapeCopy(aShape,theStartLCS,theEndLCS)
    if TrsfOp.IsDone() == 0:
      print "PositionShapeCopy : ", TrsfOp.GetErrorCode()
    return anObj

def MakeOffset(aShape, anOffset):
    anObj = TrsfOp.OffsetShapeCopy(aShape, anOffset)
    if TrsfOp.IsDone() == 0:
      print "OffsetShapeCopy : ", TrsfOp.GetErrorCode()
    return anObj

# -----------------------------------------------------------------------------
# Patterns
# -----------------------------------------------------------------------------

def MakeMultiTranslation1D(aShape,aDir,aStep,aNbTimes):
    anObj = TrsfOp.MultiTranslate1D(aShape,aDir,aStep,aNbTimes)
    if TrsfOp.IsDone() == 0:
      print "MultiTranslate1D : ", TrsfOp.GetErrorCode()
    return anObj

def MakeMultiTranslation2D(aShape,d1,step1,nbtimes1,d2,step2,nbtimes2):
    anObj = TrsfOp.MultiTranslate2D(aShape,d1,step1,nbtimes1,d2,step2,nbtimes2)
    if TrsfOp.IsDone() == 0:
      print "MultiTranslate2D : ", TrsfOp.GetErrorCode()
    return anObj

def MultiRotate1D(aShape,aVec,aNbTimes):
    anObj = TrsfOp.MultiRotate1D(aShape,aVec,aNbTimes)
    if TrsfOp.IsDone() == 0:
      print "MultiRotate1D : ", TrsfOp.GetErrorCode()
    return anObj

def MultiRotate2D(aShape,aVec,anAngle,nbtimes1,aStep,nbtimes2):
    anObj = TrsfOp.MultiRotate2D(aShape,aVec,anAngle,nbtimes1,aStep,nbtimes2)
    if TrsfOp.IsDone() == 0:
      print "MultiRotate2D : ", TrsfOp.GetErrorCode()
    return anObj

def MakeMultiRotation1D(aShape,aDir,aPoint,aNbTimes):
    aVec = MakeLine(aPoint,aDir)
    anObj = MultiRotate1D(aShape,aVec,aNbTimes)
    return anObj

def MakeMultiRotation2D(aShape,aDir,aPoint,anAngle,nbtimes1,aStep,nbtimes2):
    aVec = MakeLine(aPoint,aDir)
    anObj = MultiRotate2D(aShape,aVec,anAngle,nbtimes1,aStep,nbtimes2)
    return anObj

# -----------------------------------------------------------------------------
# Local operations
# -----------------------------------------------------------------------------

def MakeFilletAll(aShape,radius):
    anObj = LocalOp.MakeFilletAll(aShape,radius)
    if LocalOp.IsDone() == 0:
      print "MakeFilletAll : ", LocalOp.GetErrorCode()
    return anObj

def MakeFillet(aShape,radius,aShapeType,ListShape):
    anObj = None
    if aShapeType == ShapeType["EDGE"]:
        anObj = LocalOp.MakeFilletEdges(aShape,radius,ListShape)
    else:
        anObj = LocalOp.MakeFilletFaces(aShape,radius,ListShape)
    if LocalOp.IsDone() == 0:
      print "MakeFillet : ", LocalOp.GetErrorCode()
    return anObj

def MakeChamferAll(aShape,d):
    anObj = LocalOp.MakeChamferAll(aShape,d)
    if LocalOp.IsDone() == 0:
      print "MakeChamferAll : ", LocalOp.GetErrorCode()
    return anObj

def MakeChamferEdge(aShape,d1,d2,face1,face2):
    anObj = LocalOp.MakeChamferEdge(aShape,d1,d2,face1,face2)
    if LocalOp.IsDone() == 0:
      print "MakeChamferEdge : ", LocalOp.GetErrorCode()
    return anObj

def MakeChamferFaces(aShape,d1,d2,ListShape):
    anObj = LocalOp.MakeChamferFaces(aShape,d1,d2,ListShape)
    if LocalOp.IsDone() == 0:
      print "MakeChamferFaces : ", LocalOp.GetErrorCode()
    return anObj

def MakeChamfer(aShape,d1,d2,aShapeType,ListShape):
    anObj = None
    if aShapeType == ShapeType["EDGE"]:
        anObj = MakeChamferEdge(aShape,d1,d2,ListShape[0],ListShape[1])
    else:
        anObj = MakeChamferFaces(aShape,d1,d2,ListShape)
    return anObj

def Archimede(aShape,weight,WaterDensity,MeshingDeflection):
    anObj = LocalOp.MakeArchimede(aShape,weight,WaterDensity,MeshingDeflection)
    if LocalOp.IsDone() == 0:
      print "MakeArchimede : ", LocalOp.GetErrorCode()
    return anObj

# -----------------------------------------------------------------------------
# Information objects
# -----------------------------------------------------------------------------

def PointCoordinates(Point):
    aTuple = MeasuOp.PointCoordinates(Point)
    if MeasuOp.IsDone() == 0:
      print "PointCoordinates : ", MeasuOp.GetErrorCode()
    return aTuple

def BasicProperties(Shape):
    aTuple = MeasuOp.GetBasicProperties(Shape)
    if MeasuOp.IsDone() == 0:
      print "BasicProperties : ", MeasuOp.GetErrorCode()
    return aTuple

def BoundingBox(Shape):
    aTuple = MeasuOp.GetBoundingBox(Shape)
    if MeasuOp.IsDone() == 0:
      print "BoundingBox : ", MeasuOp.GetErrorCode()
    return aTuple

def Inertia(Shape):
    aTuple = MeasuOp.GetInertia(Shape)
    if MeasuOp.IsDone() == 0:
      print "Inertia : ", MeasuOp.GetErrorCode()
    return aTuple

def MinDistance(Shape1, Shape2):
    aTuple = MeasuOp.GetMinDistance(Shape1, Shape2)
    if MeasuOp.IsDone() == 0:
      print "MinDistance : ", MeasuOp.GetErrorCode()
    return aTuple[0]

def Tolerance(Shape):
    aTuple = MeasuOp.GetTolerance(Shape)
    if MeasuOp.IsDone() == 0:
      print "Tolerance : ", MeasuOp.GetErrorCode()
    return aTuple

def WhatIs(Shape):
    aDescr = MeasuOp.WhatIs(Shape)
    if MeasuOp.IsDone() == 0:
      print "WhatIs : ", MeasuOp.GetErrorCode()
    return aDescr

def MakeCDG(aShape):
    anObj = MeasuOp.GetCentreOfMass(aShape)
    if MeasuOp.IsDone() == 0:
      print "GetCentreOfMass : ", MeasuOp.GetErrorCode()
    return anObj

def CheckShape(aShape):
    (IsValid, Status) = MeasuOp.CheckShape(aShape)
    if MeasuOp.IsDone() == 0:
      print "CheckShape : ", MeasuOp.GetErrorCode()
    else:
      if IsValid == 0:
        print Status
    return IsValid

# -----------------------------------------------------------------------------
# Import/Export objects
# -----------------------------------------------------------------------------

def Import(filename, formatname):
    anObj = InsertOp.Import(filename, formatname)
    if InsertOp.IsDone() == 0:
      print "Import : ", InsertOp.GetErrorCode()
    return anObj

def ImportBREP(theFileName):
    return Import(theFileName, "BREP")

def ImportIGES(theFileName):
    return Import(theFileName, "IGES")

def ImportSTEP(theFileName):
    return Import(theFileName, "STEP")

def Export(aShape, filename, formatname):
    InsertOp.Export(aShape, filename, formatname)
    if InsertOp.IsDone() == 0:
      print "Export : ", InsertOp.GetErrorCode()

def ExportBREP(theObject, theFileName):
    return Export(theObject, theFileName, "BREP")

def ExportIGES(theObject, theFileName):
    return Export(theObject, theFileName, "IGES")

def ExportSTEP(theObject, theFileName):
    return Export(theObject, theFileName, "STEP")

# -----------------------------------------------------------------------------
# Block operations
# -----------------------------------------------------------------------------

def MakeQuad(E1, E2, E3, E4):
    anObj = BlocksOp.MakeQuad(E1, E2, E3, E4)
    if BlocksOp.IsDone() == 0:
      print "MakeQuad : ", BlocksOp.GetErrorCode()
    return anObj

def MakeQuad2Edges(E1, E2):
    anObj = BlocksOp.MakeQuad2Edges(E1, E2)
    if BlocksOp.IsDone() == 0:
      print "MakeQuad2Edges : ", BlocksOp.GetErrorCode()
    return anObj

def MakeQuad4Vertices(V1, V2, V3, V4):
    anObj = BlocksOp.MakeQuad4Vertices(V1, V2, V3, V4)
    if BlocksOp.IsDone() == 0:
      print "MakeQuad4Vertices : ", BlocksOp.GetErrorCode()
    return anObj

def MakeHexa(F1, F2, F3, F4, F5, F6):
    anObj = BlocksOp.MakeHexa(F1, F2, F3, F4, F5, F6)
    if BlocksOp.IsDone() == 0:
      print "MakeHexa : ", BlocksOp.GetErrorCode()
    return anObj

def MakeHexa2Faces(F1, F2):
    anObj = BlocksOp.MakeHexa2Faces(F1, F2)
    if BlocksOp.IsDone() == 0:
      print "MakeHexa2Faces : ", BlocksOp.GetErrorCode()
    return anObj

def GetPoint(theShape, theX, theY, theZ, theEpsilon):
    anObj = BlocksOp.GetPoint(theShape, theX, theY, theZ, theEpsilon)
    if BlocksOp.IsDone() == 0:
      print "GetPoint : ", BlocksOp.GetErrorCode()
    return anObj

def GetEdge(theShape, thePoint1, thePoint2):
    anObj = BlocksOp.GetEdge(theShape, thePoint1, thePoint2)
    if BlocksOp.IsDone() == 0:
      print "GetEdge : ", BlocksOp.GetErrorCode()
    return anObj

def GetEdgeNearPoint(theShape, thePoint):
    anObj = BlocksOp.GetEdgeNearPoint(theShape, thePoint)
    if BlocksOp.IsDone() == 0:
      print "GetEdgeNearPoint : ", BlocksOp.GetErrorCode()
    return anObj

def GetFaceByPoints(theShape, thePoint1, thePoint2, thePoint3, thePoint4):
    anObj = BlocksOp.GetFaceByPoints(theShape, thePoint1, thePoint2, thePoint3, thePoint4)
    if BlocksOp.IsDone() == 0:
      print "GetFaceByPoints : ", BlocksOp.GetErrorCode()
    return anObj

def GetFaceByEdges(theShape, theEdge1, theEdge2):
    anObj = BlocksOp.GetFaceByEdges(theShape, theEdge1, theEdge2)
    if BlocksOp.IsDone() == 0:
      print "GetFaceByEdges : ", BlocksOp.GetErrorCode()
    return anObj

def GetOppositeFace(theBlock, theFace):
    anObj = BlocksOp.GetOppositeFace(theBlock, theFace)
    if BlocksOp.IsDone() == 0:
      print "GetOppositeFace : ", BlocksOp.GetErrorCode()
    return anObj

def GetFaceNearPoint(theShape, thePoint):
    anObj = BlocksOp.GetFaceNearPoint(theShape, thePoint)
    if BlocksOp.IsDone() == 0:
      print "GetFaceNearPoint : ", BlocksOp.GetErrorCode()
    return anObj

def GetFaceByNormale(theBlock, theVector):
    anObj = BlocksOp.GetFaceByNormale(theBlock, theVector)
    if BlocksOp.IsDone() == 0:
      print "GetFaceByNormale : ", BlocksOp.GetErrorCode()
    return anObj

def CheckCompoundOfBlocks(theCompound):
    (IsValid, BCErrors) = BlocksOp.CheckCompoundOfBlocks(theCompound)
    if BlocksOp.IsDone() == 0:
      print "CheckCompoundOfBlocks : ", BlocksOp.GetErrorCode()
    else:
      if IsValid == 0:
        Descr = BlocksOp.PrintBCErrors(theCompound, BCErrors)
        print Descr
    return IsValid

def RemoveExtraEdges(theShape):
    anObj = BlocksOp.RemoveExtraEdges(theShape)
    if BlocksOp.IsDone() == 0:
      print "RemoveExtraEdges : ", BlocksOp.GetErrorCode()
    return anObj

def CheckAndImprove(theShape):
    anObj = BlocksOp.CheckAndImprove(theShape)
    if BlocksOp.IsDone() == 0:
      print "CheckAndImprove : ", BlocksOp.GetErrorCode()
    return anObj

def MakeBlockExplode(theCompound, theMinNbFaces, theMaxNbFaces):
    aList = BlocksOp.ExplodeCompoundOfBlocks(theCompound, theMinNbFaces, theMaxNbFaces)
    if BlocksOp.IsDone() == 0:
      print "MakeBlockExplode : ", BlocksOp.GetErrorCode()
    return aList

def GetBlockNearPoint(theCompound, thePoint):
    anObj = BlocksOp.GetBlockNearPoint(theCompound, thePoint)
    if BlocksOp.IsDone() == 0:
      print "GetBlockNearPoint : ", BlocksOp.GetErrorCode()
    return anObj

def GetBlockByParts(theCompound, theParts):
    anObj = BlocksOp.GetBlockByParts(theCompound, theParts)
    if BlocksOp.IsDone() == 0:
      print "GetBlockByParts : ", BlocksOp.GetErrorCode()
    return anObj

def GetBlocksByParts(theCompound, theParts):
    aList = BlocksOp.GetBlocksByParts(theCompound, theParts)
    if BlocksOp.IsDone() == 0:
      print "GetBlocksByParts : ", BlocksOp.GetErrorCode()
    return aList

def MakeMultiTransformation1D(Block, DirFaceID1, DirFaceID2, NbTimes):
    anObj = BlocksOp.MakeMultiTransformation1D(Block, DirFaceID1, DirFaceID2, NbTimes)
    if BlocksOp.IsDone() == 0:
      print "MakeMultiTransformation1D : ", BlocksOp.GetErrorCode()
    return anObj

def MakeMultiTransformation2D(Block, DirFaceID1U, DirFaceID2U, NbTimesU,
			             DirFaceID1V, DirFaceID2V, NbTimesV):
    anObj = BlocksOp.MakeMultiTransformation2D(Block, DirFaceID1U, DirFaceID2U, NbTimesU,
					              DirFaceID1V, DirFaceID2V, NbTimesV)
    if BlocksOp.IsDone() == 0:
      print "MakeMultiTransformation2D : ", BlocksOp.GetErrorCode()
    return anObj

def Propagate(theShape):
    listChains = BlocksOp.Propagate(theShape)
    if BlocksOp.IsDone() == 0:
      print "Propagate : ", BlocksOp.GetErrorCode()
    return listChains

# -----------------------------------------------------------------------------
# Group operations
# -----------------------------------------------------------------------------

def CreateGroup(MainShape, ShapeType):
    anObj = GroupOp.CreateGroup(MainShape, ShapeType)
    if GroupOp.IsDone() == 0:
       print "CreateGroup : ", GroupOp.GetErrorCode()
    return anObj

def AddObject(Group, SubShapeID):
    GroupOp.AddObject(Group, SubShapeID)
    if GroupOp.IsDone() == 0:
      print "AddObject : ", GroupOp.GetErrorCode()

def RemoveObject(Group, SubShapeID):
    GroupOp.RemoveObject(Group, SubShapeID)
    if GroupOp.IsDone() == 0:
      print "RemoveObject : ", GroupOp.GetErrorCode()

def UnionList (theGroup, theSubShapes):
    GroupOp.UnionList(theGroup, theSubShapes)
    if GroupOp.IsDone() == 0:
      print "UnionList : ", GroupOp.GetErrorCode()

def UnionIDs(theGroup, theSubShapes):
    GroupOp.UnionIDs(theGroup, theSubShapes)
    if GroupOp.IsDone() == 0:
        print "UnionIDs : ", GroupOp.GetErrorCode()

def DifferenceList (theGroup, theSubShapes):
    GroupOp.DifferenceList(theGroup, theSubShapes)
    if GroupOp.IsDone() == 0:
      print "DifferenceList : ", GroupOp.GetErrorCode()

def DifferenceIDs(theGroup, theSubShapes):
    GroupOp.DifferenceIDs(theGroup, theSubShapes)
    if GroupOp.IsDone() == 0:
        print "DifferenceIDs : ", GroupOp.GetErrorCode()

def GetObjectIDs(Group):
    ListIDs = GroupOp.GetObjects(Group)
    if GroupOp.IsDone() == 0:
      print "GetObjectIDs : ", GroupOp.GetErrorCode()
    return ListIDs

def GetType(theGroup):
    aType = GroupOp.GetType(theGroup)
    if GroupOp.IsDone() == 0:
      print "GetType : ", GroupOp.GetErrorCode()
    return aType

def GetMainShape(theGroup):
    anObj = GroupOp.GetMainShape(theGroup)
    if GroupOp.IsDone() == 0:
      print "GetMainShape : ", GroupOp.GetErrorCode()
    return anObj

def GetEdgesByLength (theShape, min_length, max_length, include_min = 1, include_max = 1):
    """
    Create group of edges of theShape, whose length is in range [min_length, max_length].
    If include_min/max == 0, edges with length == min/max_length will not be included in result.
    """

    edges = SubShapeAll(theShape, ShapeType["EDGE"])
    edges_in_range = []
    for edge in edges:
        Props = BasicProperties(edge)
	if min_length <= Props[0] and Props[0] <= max_length:
	    if (not include_min) and (min_length == Props[0]):
	        skip = 1
            else:
	        if (not include_max) and (Props[0] == max_length):
	            skip = 1
                else:
	            edges_in_range.append(edge)

    if len(edges_in_range) <= 0:
        print "No edges found by given criteria"
	return 0

    group_edges = CreateGroup(theShape, ShapeType["EDGE"])
    UnionList(group_edges, edges_in_range)

    return group_edges

# Add Path to the system path
#
def addPath(Path):
    if (sys.path.count(Path) < 1):
	sys.path.append(Path)
