// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  File   : GeometryGUI_Operations.h
//  Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#ifndef GEOMETRYGUI_OPERATIONS_H
#define GEOMETRYGUI_OPERATIONS_H

namespace GEOMOp {
  enum {
    // ToolsGUI ------------------//--------------------------------
    OpImport            = 1000,   // MENU FILE  - IMPORT
    OpExport            = 1001,   // MENU FILE  - EXPORT
    OpDelete            = 1020,   // MENU EDIT  - DELETE
    OpCheckGeom         = 1030,   // MENU TOOLS - CHECK GEOMETRY
    OpSelectVertex      = 1100,   // POPUP MENU - SELECT ONLY - VERTEX
    OpSelectEdge        = 1101,   // POPUP MENU - SELECT ONLY - EDGE
    OpSelectWire        = 1102,   // POPUP MENU - SELECT ONLY - WIRE
    OpSelectFace        = 1103,   // POPUP MENU - SELECT ONLY - FACE
    OpSelectShell       = 1104,   // POPUP MENU - SELECT ONLY - SHELL
    OpSelectSolid       = 1105,   // POPUP MENU - SELECT ONLY - SOLID
    OpSelectCompound    = 1106,   // POPUP MENU - SELECT ONLY - COMPOUND
    OpSelectAll         = 1107,   // POPUP MENU - SELECT ONLY - SELECT ALL
    OpDeflection        = 1200,   // POPUP MENU - DEFLECTION COEFFICIENT
    OpColor             = 1201,   // POPUP MENU - COLOR
    OpTransparency      = 1202,   // POPUP MENU - TRANSPARENCY
    OpIncrTransparency  = 1203,   // SHORTCUT   - INCREASE TRANSPARENCY
    OpDecrTransparency  = 1204,   // SHORTCUT   - DECREASE TRANSPARENCY
    OpIsos              = 1205,   // POPUP MENU - ISOS
    OpIncrNbIsos        = 1206,   // SHORTCUT   - INCREASE NB ISOS
    OpDecrNbIsos        = 1207,   // SHORTCUT   - DECREASE NB ISOS
    OpAutoColor         = 1208,   // POPUP MENU - AUTO COLOR
    OpNoAutoColor       = 1209,   // POPUP MENU - DISABLE AUTO COLOR
    OpPointMarker       = 1210,   // POPUP MENU - POINT MARKER
    OpSetTexture        = 1211,   // POPUP MENU - SETTEXTURE
    OpShowChildren      = 1250,   // POPUP MENU - SHOW CHILDREN
    OpHideChildren      = 1251,   // POPUP MENU - HIDE CHILDREN
    OpUnpublishObject   = 1253,   // POPUP MENU - UNPUBLISH
    OpPublishObject     = 1254,   // GEOM ROOT OBJECT - POPUP MENU - PUBLISH

    // DisplayGUI ----------------//--------------------------------
    OpDisplayMode       = 2000,   // MENU VIEW  - DISPLAY MODE - WIREFRAME/SHADING
    OpSwitchVectors     = 2001,   // MENU VIEW  - DISPLAY MODE - SHOW/HIDE EDGE DIRECTION
    OpShowAll           = 2002,   // MENU VIEW  - SHOW ALL
    OpHideAll           = 2003,   // MENU VIEW  - HIDE ALL
    OpShow              = 2100,   // POPUP MENU - SHOW
    OpShowOnly          = 2101,   // POPUP MENU - SHOW ONLY
    OpHide              = 2102,   // POPUP MENU - HIDE
    OpWireframe         = 2200,   // POPUP MENU - DISPLAY MODE - WIREFRAME
    OpShading           = 2201,   // POPUP MENU - DISPLAY MODE - SHADING
    OpVectors           = 2202,   // POPUP MENU - DISPLAY MODE - SHOW EDGE DIRECTION
    OpTexture           = 2203,   // POPUP MENU - DISPLAY MODE - TEXTURE
    // BasicGUI ------------------//--------------------------------
    OpPoint             = 3000,   // MENU NEW ENTITY - BASIC - POINT
    OpLine              = 3001,   // MENU NEW ENTITY - BASIC - LINE
    OpCircle            = 3002,   // MENU NEW ENTITY - BASIC - CIRCLE
    OpEllipse           = 3003,   // MENU NEW ENTITY - BASIC - ELLIPSE
    OpArc               = 3004,   // MENU NEW ENTITY - BASIC - ARC
    OpVector            = 3005,   // MENU NEW ENTITY - BASIC - VECTOR
    OpPlane             = 3006,   // MENU NEW ENTITY - BASIC - PLANE
    OpCurve             = 3007,   // MENU NEW ENTITY - BASIC - CURVE
    OpLCS               = 3008,   // MENU NEW ENTITY - BASIC - LOCAL COORDINATE SYSTEM
    OpOriginAndVectors  = 3009,   // MENU NEW ENTITY - BASIC - ORIGIN AND BASE VECTORS
    // PrimitiveGUI --------------//--------------------------------
    OpBox               = 3100,   // MENU NEW ENTITY - PRIMITIVES - BOX
    OpCylinder          = 3101,   // MENU NEW ENTITY - PRIMITIVES - CYLINDER
    OpSphere            = 3102,   // MENU NEW ENTITY - PRIMITIVES - SPHERE
    OpTorus             = 3103,   // MENU NEW ENTITY - PRIMITIVES - TORUS
    OpCone              = 3104,   // MENU NEW ENTITY - PRIMITIVES - CONE
    OpRectangle         = 3105,   // MENU NEW ENTITY - PRIMITIVES - FACE
    OpDisk              = 3106,   // MENU NEW ENTITY - PRIMITIVES - DISK
    // GenerationGUI -------------//--------------------------------
    OpPrism             = 3200,   // MENU NEW ENTITY - GENERATION - EXTRUSION
    OpRevolution        = 3201,   // MENU NEW ENTITY - GENERATION - REVOLUTION
    OpFilling           = 3202,   // MENU NEW ENTITY - GENERATION - FILLING
    OpPipe              = 3203,   // MENU NEW ENTITY - GENERATION - EXTRUSION ALONG PATH
    // EntityGUI -----------------//--------------------------------
    Op2dSketcher        = 3300,   // MENU NEW ENTITY - SKETCHER
    Op3dSketcher        = 3301,   // MENU NEW ENTITY - 3D SKETCHER
    OpExplode           = 3302,   // MENU NEW ENTITY - EXPLODE
#ifdef WITH_OPENCV
    OpFeatureDetect     = 3303,   // MENU NEW ENTITY - FEATURE DETECTION
    OpPictureImport     = 3304,   // MENU NEW ENTITY - IMPORT PICTURE IN VIEWER
#endif
    // BuildGUI ------------------//--------------------------------
    OpEdge              = 3400,   // MENU NEW ENTITY - BUILD - EDGE
    OpWire              = 3401,   // MENU NEW ENTITY - BUILD - WIRE
    OpFace              = 3402,   // MENU NEW ENTITY - BUILD - FACE
    OpShell             = 3403,   // MENU NEW ENTITY - BUILD - SHELL
    OpSolid             = 3404,   // MENU NEW ENTITY - BUILD - SOLID
    OpCompound          = 3405,   // MENU NEW ENTITY - BUILD - COMPOUND
    // BooleanGUI ----------------//--------------------------------
    OpFuse              = 3500,   // MENU OPERATIONS - BOOLEAN - FUSE
    OpCommon            = 3501,   // MENU OPERATIONS - BOOLEAN - COMMON
    OpCut               = 3502,   // MENU OPERATIONS - BOOLEAN - CUT
    OpSection           = 3503,   // MENU OPERATIONS - BOOLEAN - SECTION
    // TransformationGUI ---------//--------------------------------
    OpTranslate         = 3600,   // MENU OPERATIONS - TRANSFORMATION - TRANSLATION
    OpRotate            = 3601,   // MENU OPERATIONS - TRANSFORMATION - ROTATION
    OpChangeLoc         = 3602,   // MENU OPERATIONS - TRANSFORMATION - LOCATION
    OpMirror            = 3603,   // MENU OPERATIONS - TRANSFORMATION - MIRROR
    OpScale             = 3604,   // MENU OPERATIONS - TRANSFORMATION - SCALE
    OpOffset            = 3605,   // MENU OPERATIONS - TRANSFORMATION - OFFSET
    OpMultiTranslate    = 3606,   // MENU OPERATIONS - TRANSFORMATION - MULTI-TRANSLATION
    OpMultiRotate       = 3607,   // MENU OPERATIONS - TRANSFORMATION - MULTI-ROTATION
    OpReimport          = 3608,   // POPUP MENU - RELOAD IMPORTED
    OpProjection        = 3609,   // MENU OPERATIONS - TRANSFORMATION - PROJECTION
    // OperationGUI
    OpPartition         = 3700,   // MENU OPERATION - PARTITION
    OpArchimede         = 3701,   // MENU OPERATION - ARCHIMEDE
    OpFillet3d          = 3702,   // MENU OPERATION - FILLET
    OpChamfer           = 3703,   // MENU OPERATION - CHAMFER
    OpShapesOnShape     = 3704,   // MENU OPERATION - GET SHAPES ON SHAPE
    OpFillet2d          = 3705,   // MENU OPERATION - FILLET 2D
    OpFillet1d          = 3706,   // MENU OPERATION - FILLET 1D
    OpClipping          = 3707,   // MENU OPERATION - CLIPPING RANGE
    OpSharedShapes      = 3708,   // MENU OPERATION - GET SHARED SHAPES
    // RepairGUI -----------------//--------------------------------
    OpSewing            = 4000,   // MENU REPAIR - SEWING
    OpSuppressFaces     = 4001,   // MENU REPAIR - SUPPRESS FACES
    OpSuppressHoles     = 4002,   // MENU REPAIR - SUPPRESS HOLES
    OpShapeProcess      = 4003,   // MENU REPAIR - SHAPE PROCESSING
    OpCloseContour      = 4004,   // MENU REPAIR - CLOSE CONTOUR
    OpRemoveIntWires    = 4005,   // MENU REPAIR - REMOVE INTERNAL WIRES
    OpAddPointOnEdge    = 4006,   // MENU REPAIR - ADD POINT ON EDGE
    OpFreeBoundaries    = 4007,   // MENU MEASURES - FREE BOUNDARIES
    OpFreeFaces         = 4008,   // MENU MEASURES - FREE FACES
    OpOrientation       = 4009,   // MENU REPAIR - CHANGE ORIENTATION
    OpGlueFaces         = 4010,   // MENU REPAIR - GLUE FACES
    OpRemoveExtraEdges  = 4011,   // MENU REPAIR - REMOVE EXTRA EDGES
    OpLimitTolerance    = 4012,   // MENU REPAIR - LIMIT TOLERANCE
    OpGlueEdges         = 4013,   // MENU REPAIR - GLUE EDGES
    // MeasureGUI ----------------//--------------------------------
    OpProperties        = 5000,   // MENU MEASURES - PROPERTIES
    OpCenterMass        = 5001,   // MENU MEASURES - CENTRE OF MASS
    OpInertia           = 5002,   // MENU MEASURES - INERTIA
    OpNormale           = 5003,   // MENU MEASURES - NORMALE
    OpBoundingBox       = 5004,   // MENU MEASURES - BOUNDING BOX
    OpMinDistance       = 5005,   // MENU MEASURES - MIN DISTANCE
    OpAngle             = 5006,   // MENU MEASURES - ANGLE
    OpTolerance         = 5007,   // MENU MEASURES - TOLERANCE
    OpWhatIs            = 5008,   // MENU MEASURES - WHAT IS
    OpCheckShape        = 5009,   // MENU MEASURES - CHECK
    OpCheckCompound     = 5010,   // MENU MEASURES - CHECK COMPOUND OF BLOCKS
    OpPointCoordinates  = 5011,   // MENU MEASURES - POINT COORDINATES
    // GroupGUI ------------------//--------------------------------
    OpGroupCreate       = 6000,   // MENU GROUP - CREATE
    OpGroupEdit         = 6001,   // MENU GROUP - EDIT
    OpGroupCreatePopup  = 6002,   // POPUP MENU - CREATE GROUP
    // BlocksGUI -----------------//--------------------------------
    OpHexaSolid         = 6100,   // MENU BLOCKS - HEXAHEDRAL SOLID
    OpMultiTransform    = 6101,   // MENU BLOCKS - MULTI-TRANSFORMATION
    OpQuadFace          = 6102,   // MENU BLOCKS - QUADRANGLE FACE
    OpPropagate         = 6103,   // MENU BLOCKS - PROPAGATE
    OpExplodeBlock      = 6104,   // MENU BLOCKS - EXPLODE ON BLOCKS
    // AdvancedGUI ---------------//--------------------------------
    OpAdvancedNoOp      = 10000,  // NO OPERATION (advanced operations base)
    OpPipeTShape        = 10001,  // MENU NEW ENTITY - ADVANCED - PIPE TSHAPE
//     OpPipeTShapeGroups  = 10002,  // MENU NEW ENTITY - ADVANCED - PIPE TSHAPE GROUPS
    //@@ insert new functions before this line @@ do not remove this line @@//
  };
}

#endif // GEOMETRYGUI_OPERATIONS_H
