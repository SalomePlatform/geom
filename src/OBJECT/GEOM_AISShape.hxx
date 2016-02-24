// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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

#ifndef _GEOM_AISShape_HeaderFile
#define _GEOM_AISShape_HeaderFile

#include <SALOME_AISShape.hxx>
#include <SALOME_InteractiveObject.hxx>

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <PrsMgr_PresentationManager.hxx>
#include <TCollection_AsciiString.hxx>
#include <AIS_DisplayMode.hxx>

#include <QList>
#include <QVariant>

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Gen)

class TopoDS_Shape;

class GEOM_AISShape : public SALOME_AISShape
{
public:
  //! Enumeration of display modes
  typedef enum {
    Wireframe        = AIS_WireFrame,       //!< wireframe
    Shading          = AIS_Shaded,          //!< shadin
    ShadingWithEdges,                       //!< shading with edges
    TexturedShape,                          //!< texture
    CustomHighlight                         //!< fields
  } DispMode;

  //! Enumeration of top level display modes
  typedef enum {
    TopShowAdditionalWActor = 0,
    TopKeepCurrent, //!< Keep current display mode
    TopWireFrame, 
    TopShading, 
    TopShadingWithEdges,
  } TopLevelDispMode;
  
  Standard_EXPORT GEOM_AISShape(const TopoDS_Shape& shape, const Standard_CString aName);
  Standard_EXPORT ~GEOM_AISShape();

  Standard_EXPORT Standard_Boolean                 hasIO();
  Standard_EXPORT void                             setIO(const Handle(SALOME_InteractiveObject)& name);
  Standard_EXPORT Handle(SALOME_InteractiveObject) getIO();

  Standard_EXPORT void             setName(const Standard_CString aName);
  Standard_EXPORT Standard_CString getName();

  Standard_EXPORT Standard_Boolean isTopLevel();
  Standard_EXPORT void setTopLevel(Standard_Boolean);

  Standard_EXPORT void highlightSubShapes(const TColStd_IndexedMapOfInteger& aIndexMap, const Standard_Boolean aHighlight );
  
  Standard_EXPORT void SetShadingColor(const Quantity_Color &aCol);
  Standard_EXPORT void SetEdgesInShadingColor(const Quantity_Color &aCol);
  Standard_EXPORT void SetLabelColor(const Quantity_Color &aCol);
  Standard_EXPORT void SetDisplayVectors(bool isShow);
  Standard_EXPORT void SetDisplayVertices(bool isShow);
  Standard_EXPORT void SetDisplayName(bool isShow);

  Standard_EXPORT virtual void Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
				       const Handle(Prs3d_Presentation)& aPresentation,
				       const Standard_Integer aMode = 0);
  
  Standard_EXPORT virtual bool isShowVectors() { return myDisplayVectors; }
  Standard_EXPORT virtual bool isShowVertices() { return myDisplayVertices; }
  Standard_EXPORT virtual bool isShowName() { return myDisplayName; }

  Standard_EXPORT virtual Standard_Boolean switchTopLevel();
  Standard_EXPORT virtual Standard_Boolean toActivate();
        
  Standard_EXPORT static Quantity_Color topLevelColor();
  Standard_EXPORT static void           setTopLevelColor(const Quantity_Color c);

  Standard_EXPORT static TopLevelDispMode topLevelDisplayMode();
  Standard_EXPORT static void             setTopLevelDisplayMode(const TopLevelDispMode dm);

  Standard_EXPORT void setPrevDisplayMode(const Standard_Integer mode);
  Standard_EXPORT Standard_Integer prevDisplayMode() const {return myPrevDisplayMode;}

  // Field step information
  Standard_EXPORT void setFieldStepInfo( const GEOM::field_data_type theFieldDataType,
					 const int theFieldDimension,
					 const QList<QVariant>& theFieldStepData,
					 const TCollection_AsciiString& theFieldStepName,
					 const double theFieldStepRangeMin,
					 const double theFieldStepRangeMax );
  Standard_EXPORT void getFieldStepInfo( GEOM::field_data_type& theFieldDataType,
					 int& theFieldDimension,
					 QList<QVariant>& theFieldStepData,
					 TCollection_AsciiString& theFieldStepName,
					 double& theFieldStepRangeMin,
					 double& theFieldStepRangeMax ) const;

protected: 
  Standard_EXPORT void shadingMode(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
				   const Handle(Prs3d_Presentation)& aPrs,
				   const Standard_Integer aMode);
  
  // Displaying the field data
  Standard_EXPORT void drawField( const Handle(Prs3d_Presentation)& thePrs,
				  const bool theIsText = false,
				  const bool theIsHighlight = false );
  
  // Displaying the name of shape
  Standard_EXPORT void drawName( const Handle(Prs3d_Presentation)& thePrs );

  // Auxiliary method to compute a center of mass for the specified shape
  Standard_EXPORT static Standard_Boolean computeMassCenter( const TopoDS_Shape& theShape,
							     gp_Pnt& theCenter );
  
private: 
  Quantity_Color           myShadingColor;
  Quantity_Color           myEdgesInShadingColor;
  Quantity_Color           myLabelColor;

  TCollection_AsciiString  myName;
  bool                     myDisplayVectors;
  bool                     myDisplayVertices;
  bool                     myDisplayName;
  Standard_Boolean         myTopLevel;
  Standard_Integer         myPrevDisplayMode;

  GEOM::field_data_type    myFieldDataType;
  int                      myFieldDimension;
  QList<QVariant>          myFieldStepData;
  TCollection_AsciiString  myFieldStepName;
  double                   myFieldStepRangeMin;
  double                   myFieldStepRangeMax;

  static TopLevelDispMode  myTopLevelDm;
  static Quantity_Color    myTopLevelColor;

public:
  OCCT_DEFINE_STANDARD_RTTIEXT(GEOM_AISShape,SALOME_AISShape)
};

DEFINE_STANDARD_HANDLE(GEOM_AISShape, SALOME_AISShape)

#endif
