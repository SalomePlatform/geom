# Check patch face functionality

def CheckFacesOnHoles( faces ):
        """
        check that none of the faces in the list have holes
        param faces - list of faces
        return bool - *True* if all faces not have hole; *False* otherwise
        """
        for face in faces:
            newFaces = geompy.PatchFace(face)
            if len(newFaces) != 1:
                return False
        return True

def CompareAreaAfterPatchFace( originalFace, patchFaceResult, eps=1e-06 ):
        """
        Specific function for check result of PatchFace operation.
        Check that area of original face equal area first face from list (face by outer wire of original face)
        minus the areas of the rest elements from list (holes)
        param originalFace - original face
        param patchFaceResult - list of faces result PatchFace operation. 
                              First element - face from outer wire of original face,
                              other elements - faces from holes on original face
        param eps - defines tolerance for comparison
        return bool - *True* if area of original face is equal of area face[0] minus area of holes; *False* otherwise
        """
        areaOfHoles = 0.
        for index in range(1, len(patchFaceResult)):
            areaOfHoles += geompy.BasicProperties(patchFaceResult[index])[1]
        return geompy.BasicProperties(originalFace)[1] - (geompy.BasicProperties(patchFaceResult[0])[1] - areaOfHoles) <= eps

import math
import salome
salome.salome_init_without_session()
import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New()

# Create shape
vz = geompy.MakeVectorDXDYDZ(0, 0, 1)
pyz = geompy.MakeVertex(0, -150, 100)

geomObj = geompy.MakeMarker(0, 0, 0, 1, 0, 0, 0, 1, 0)
sk = geompy.Sketcher2D()
sk.addPoint(30.0000000, 50.0000000)
sk.addArcAbsolute(70.0000000, 20.0000000)
sk.addSegmentAbsolute(0.0000000, 0.0000000)
sk.close()
sketch = sk.wire(geomObj)
face_1 = geompy.MakeFaceWires([sketch], 1)
multiTranslation = geompy.MakeMultiTranslation1D(face_1, None, 105, 3)
[face_2,face_3,face_4] = geompy.ExtractShapes(multiTranslation, geompy.ShapeType["FACE"], True)
scale_1 = geompy.MakeScaleTransform(face_3, None, 0.3)
scale_2 = geompy.MakeScaleTransform(face_4, None, 0.3)

translation_1 = geompy.MakeTranslation(scale_1, -10, 25, 0)
translation_2 = geompy.MakeTranslation(scale_2, -25, 20, 0)

rotation_1 = geompy.MakeRotation(translation_1, vz, -19*math.pi/180.0)
rotation_2 = geompy.MakeRotation(translation_2, vz, 15*math.pi/180.0)

cut = geompy.MakeCutList(face_1, [rotation_2, rotation_1], True)

#Perform oepration
faces = geompy.PatchFace(cut)

# Check, that result faces haven't holes
assert(CheckFacesOnHoles(faces))

#Check area
assert(CompareAreaAfterPatchFace(cut, faces))
