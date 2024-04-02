# Sample: KindOfShape method for faces, which are results of partitioning of a conical surface with a prism with complex base.
# Faces of the prism are not perpendicular to cone axis, therefore contour-wires of resulting cone fragments are composed of lines and 2-order curves.

import sys
import salome

salome.salome_init()
import salome_notebook
notebook = salome_notebook.NoteBook()

###
### GEOM component
###

import GEOM
from salome.geom import geomBuilder
import math
import SALOMEDS


def approximatelyEqual(a, b, epsilon = 1e-5):
   return abs(a - b) <= ((abs(b) if (abs(a) < abs(b)) else abs(a)) * epsilon)


def assertShapeKindEquals(iShapeInfo, iKind):
  assert (len(iShapeInfo) > 0), "Yielded data array is empty."
  assert (iShapeInfo[0] == iKind), f"Expected shape kind is {iKind}, but yielded kind is {iShapeInfo[0]}."


def assertConePropsEqual(iShapeName, iShapeInfo, iExpectedShapeInfo):
   assertShapeKindEquals(iShapeInfo, geompy.kind.CONE2D)
   assert (len(iShapeInfo) == len(iExpectedShapeInfo)), f"{iShapeName}: Yielded data array is of unexpected length."
   for idx in range(1, len(iShapeInfo)):
      assert (approximatelyEqual(iShapeInfo[idx], iExpectedShapeInfo[idx])), f"{iShapeName}: Yielded data array element is not equal to the expected value."


def assertConeInfoEquals(iFace, iExpectedShapeInfo, iAddRestoredConeToStudy = False):
  ShapeInfo = geompy.KindOfShape(iFace)
  print("ShapeInfo of " + iFace.GetName() + " = ", end = "")
  print(ShapeInfo, ', ')
  assertConePropsEqual(iFace.GetName(), ShapeInfo, iExpectedShapeInfo)

  if (iAddRestoredConeToStudy):
    BottomLidCenter = geompy.MakeVertex(ShapeInfo[1], ShapeInfo[2], ShapeInfo[3])
    AxisAuxPnt = geompy.MakeVertex(ShapeInfo[1] + ShapeInfo[4], ShapeInfo[2] + ShapeInfo[5], ShapeInfo[3] + ShapeInfo[6])
    Axis = geompy.MakeVector(BottomLidCenter, AxisAuxPnt)
    R1 = ShapeInfo[7] # Bottom lid radius.
    R2 = ShapeInfo[8] # Top lid radius.
    H = ShapeInfo[9]
    RestoredCone = geompy.MakeCone(BottomLidCenter, Axis, R1, R2, H)
    geompy.addToStudy(RestoredCone, iFace.GetName() + '__RestoredCone')


# iExpectedConeFragmentShapeInfos is a dictionary of [IndexOfFace, ExpectedShapeInfoOfFace]. IndexOfFace is zero-based index, not one-based one as in Shaper GUI!
def partitionConeAndAssertShapeInfosEqual(iCone, iPartitioningShape, iExpectedConeFragmentShapeInfos, iAddResultsToStudy):
  PartitionedCone = geompy.MakePartition([iCone], [iPartitioningShape], [], [], geompy.ShapeType["FACE"], 0, [], 0)
  if (iAddResultsToStudy):
    geompy.addToStudy(PartitionedCone, "Partitioned" + iCone.GetName())

  ConeFragments = geompy.ExtractShapes(PartitionedCone, geompy.ShapeType["FACE"], True)
  ConeFragmentsIdxs = iExpectedConeFragmentShapeInfos.keys()
  for ConeFragmentIdx in ConeFragmentsIdxs:
    assert (ConeFragmentIdx < len(ConeFragments)), f"Num of faces, {iCone.GetName()} is partitioned into, <= {ConeFragmentIdx} (zero-based index)."
    ConeFragment = ConeFragments[ConeFragmentIdx]
    ConeFragmentName = f"Partitioned{iCone.GetName()}_Face_{ConeFragmentIdx+1}" # Add index to a name as Shaper GUI does.

    if (iAddResultsToStudy):
      geompy.addToStudyInFather(PartitionedCone, ConeFragment, ConeFragmentName)
    else:
      ConeFragment.SetName(ConeFragmentName)

    assertConeInfoEquals(ConeFragment, iExpectedConeFragmentShapeInfos[ConeFragmentIdx], iAddResultsToStudy)


geompy = geomBuilder.New()

OriginalConeBaseCenter = geompy.MakeVertex(100, 130, -60)
OriginalConeAxisAuxPnt = geompy.MakeVertex(100, 230, 40)
OriginalConeAxis = geompy.MakeVector(OriginalConeBaseCenter, OriginalConeAxisAuxPnt)
OriginalCone = geompy.MakeCone(OriginalConeBaseCenter, OriginalConeAxis, 100, 50, 300)
PrismSubstrateCenter = geompy.MakeVertex(100, 1000, 50)
PrismDirAuxPnt = geompy.MakeVertex(100, 950, 50)
PrismDir = geompy.MakeVector(PrismSubstrateCenter, PrismDirAuxPnt)
PrismSubstrate = geompy.MakeDiskPntVecR(PrismSubstrateCenter, PrismDir, 100)
sk = geompy.Sketcher2D()
sk.addPoint(0.395986, 43.346713)
sk.addSegmentAbsolute(66.321537, 41.733575)
sk.addSegmentAbsolute(80.619408, -2.852314)
sk.addSegmentAbsolute(67.641539, -38.565150)
sk.addSegmentAbsolute(22.193602, -56.632163)
sk.addSegmentAbsolute(-19.060136, -51.084351)
sk.addSegmentAbsolute(-60.823572, 34.825751)
sk.addSegmentAbsolute(-13.047004, 55.727527)
sk.close()
PrismBase = sk.wire(PrismSubstrate)
Prism = geompy.MakePrismVecH(PrismBase, PrismDir, 1400)
geompy.addToStudy( OriginalConeBaseCenter, 'OriginalConeBaseCenter' )
geompy.addToStudy( OriginalConeAxisAuxPnt, 'OriginalConeAxisAuxPnt' )
geompy.addToStudy( OriginalConeAxis, 'OriginalConeAxis' )
geompy.addToStudy( OriginalCone, 'OriginalCone' )
geompy.addToStudy( PrismSubstrateCenter, 'PrismSubstrateCenter' )
geompy.addToStudy( PrismDirAuxPnt, 'PrismDirAuxPnt' )
geompy.addToStudy( PrismDir, 'PrismDir' )
geompy.addToStudy( PrismSubstrate, 'PrismSubstrate' )
geompy.addToStudy( PrismBase, 'PrismBase' )
geompy.addToStudy( Prism, 'Prism' )

# Test on the original cone
ExpectedOriginalConeFragmentsShapeInfos = {
  3: ["CONE2D", 100.0, 215.76160602318674, 25.761606023186744, 0.0, 0.7071067811865475, 0.7071067811865475, 79.7857956051852, 54.62305376134459, 150.9764510630437],
  5: ["CONE2D", 100.0, 129.99999999999753, -60.000000000002466, 0.0, 0.7071067811865475, 0.7071067811865475, 100.00000000000058, 69.82277418813575, 181.06335487118898],
  11: ["CONE2D", 100.0, 216.57653245407857, 26.57653245407856, 0.0, 0.7071067811865475, 0.7071067811865475, 79.59371560336794, 52.95933239773038, 159.80629923382543]
}
partitionConeAndAssertShapeInfosEqual(OriginalCone, Prism, ExpectedOriginalConeFragmentsShapeInfos, True)

# Test on isotropically scaled cone. Non-isotropical scaling does not preserve shape kind - it is desired behavior.
ScaledCone = geompy.MakeScaleTransform(OriginalCone, OriginalConeAxisAuxPnt, 2)
ScaledCone.SetName('ScaledCone')
ExpectedScaledConeFragmentsShapeInfos = {
  4: ["CONE2D", 100.0, 29.9999999999999, -160.00000000000009, 0.0, 0.7071067811865475, 0.7071067811865475, 200.00000000000003, 162.64508449690112, 224.1294930185934],
  6: ["CONE2D", 100.0, 262.09898500769475, 72.09898500769472, 0.0, 0.7071067811865475, 0.7071067811865475, 145.2937445981814, 120.13428858458612, 150.95673608157182],
  12: ["CONE2D", 100.0, 262.8999708414969, 72.8999708414969, 0.0, 0.7071067811865475, 0.7071067811865475, 145.10495042660943, 117.46838914559419, 165.8193676860916]
}
partitionConeAndAssertShapeInfosEqual(ScaledCone, Prism, ExpectedScaledConeFragmentsShapeInfos, False)

# Test on a cone, mirrored relative to a point.
PntMirroredCone = geompy.MakeMirrorByPoint(OriginalCone, OriginalConeAxisAuxPnt)
PntMirroredCone.SetName('PntMirroredCone')
ExpectedPntMirroredConeFragmentsShapeInfos = {
  2: ["CONE2D", 100.0, 229.8712015945071, 39.87120159450711, -0.0, -0.7071067811865475, -0.7071067811865475, 76.39941588513841, 51.25530645152799, 150.8646566016625],
  7: ["CONE2D", 100.0, 330.0, 140.0, -0.0, -0.7071067811865475, -0.7071067811865475, 100.0, 71.73019727352477, 169.61881635885143],
  10: ["CONE2D", 100.0, 249.15532313133338, 59.15532313133339, -0.0, -0.7071067811865475, -0.7071067811865475, 80.9447269211102, 51.428754043115056, 177.09583726797095]
}
partitionConeAndAssertShapeInfosEqual(PntMirroredCone, Prism, ExpectedPntMirroredConeFragmentsShapeInfos, False)

if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser()
