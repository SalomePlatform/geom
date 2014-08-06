// Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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

//  GEOM OBJECT : interactive object for Geometry entities visualization
//  File   : GEOM_AISShape.hxx
//  Module : GEOM
//
#ifndef _GEOM_AISShape_HeaderFile
#define _GEOM_AISShape_HeaderFile

#include "GEOM_OBJECT_defs.hxx"

#include <GEOM_Gen.hh>

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Handle_GEOM_AISShape_HeaderFile
#include "Handle_GEOM_AISShape.hxx"
#endif

#ifndef _Handle_SALOME_InteractiveObject_HeaderFile
#include "Handle_SALOME_InteractiveObject.hxx"
#endif
#ifndef _Standard_CString_HeaderFile
#include <Standard_CString.hxx>
#endif
#ifndef _SALOME_AISShape_HeaderFile
#include "SALOME_AISShape.hxx"
#endif
#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
#ifndef _PrsMgr_PresentationManager_HeaderFile
#include <PrsMgr_PresentationManager.hxx>
#endif
#ifndef _Handle_Prs3d_Presentation_HeaderFile
#include <Handle_Prs3d_Presentation.hxx>
#endif

#include <TCollection_AsciiString.hxx>

#include <AIS_DisplayMode.hxx>
#include <Graphic3d_MaterialAspect.hxx>

#include <QList>
#include <QVariant>

class Prs3d_Presentation;
class SALOME_InteractiveObject;
class TopoDS_Shape;

class GEOM_OBJECT_EXPORT GEOM_AISShape : public SALOME_AISShape {

public:

    //! Enumeration of display modes
    typedef enum {
      //WireFrame,       //!< the same as AIS_WireFrame
      //Shading,         //!< the same as AIS_Shaded
      ShadingWithEdges = AIS_Shaded+1, //!< shading with edges
      TexturedShape = ShadingWithEdges+1, //!< the same as AIS_ExactHLR
      CustomHighlight = TexturedShape+1
    } DispMode;

    //! Enumeration of top level display modes
    typedef enum {
      TopShowAdditionalWActor = 0,
      TopKeepCurrent, //!< Keep current display mode
      TopWireFrame, 
      TopShading, 
      TopShadingWithEdges,
    } TopLevelDispMode;


    inline void* operator new(size_t,void* anAddress) 
      {
        return anAddress;
      }
    inline void* operator new(size_t size) 
      { 
        return Standard::Allocate(size); 
      }
    inline void  operator delete(void *anAddress) 
      { 
        if (anAddress) Standard::Free((Standard_Address&)anAddress); 
      }
//    inline void  operator delete(void *anAddress, size_t size) 
//      { 
//        if (anAddress) Standard::Free((Standard_Address&)anAddress,size); 
//      }
 // Methods PUBLIC
 // 
        GEOM_AISShape(const TopoDS_Shape& shape, const Standard_CString aName);
        Standard_Boolean hasIO() ;
        void setIO(const Handle(SALOME_InteractiveObject)& name) ;
        void setName(const Standard_CString aName) ;
        Standard_CString getName() ;
        Standard_Boolean isTopLevel();
        void setTopLevel(Standard_Boolean);
        Handle_SALOME_InteractiveObject getIO() ;
        void highlightSubShapes(const TColStd_IndexedMapOfInteger& aIndexMap, const Standard_Boolean aHighlight );
        ~GEOM_AISShape();

        void SetShadingColor(const Quantity_Color &aCol);
        void SetEdgesInShadingColor(const Quantity_Color &aCol);
        void SetDisplayVectors(bool isShow);

        virtual  void Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
                                      const Handle(Prs3d_Presentation)& aPresentation,
                                      const Standard_Integer aMode = 0) ;

        virtual  bool isShowVectors () { return myDisplayVectors; }
                virtual  Standard_Boolean switchTopLevel();
                virtual  Standard_Boolean toActivate();
        
 // Type management
 //
        friend Handle_Standard_Type& GEOM_AISShape_Type_();
        const Handle(Standard_Type)& DynamicType() const;
        Standard_Boolean             IsKind(const Handle(Standard_Type)&) const;

        static Quantity_Color topLevelColor();
  static void           setTopLevelColor(const Quantity_Color c);

  static TopLevelDispMode topLevelDisplayMode();
  static void             setTopLevelDisplayMode(const TopLevelDispMode dm);

  void setPrevDisplayMode(const Standard_Integer mode);
  Standard_Integer prevDisplayMode() const {return myPrevDisplayMode;}

  // Field step information
  void setFieldStepInfo( const GEOM::field_data_type theFieldDataType,
                         const int theFieldDimension,
                         const QList<QVariant>& theFieldStepData,
                         const TCollection_AsciiString& theFieldStepName,
                         const double theFieldStepRangeMin,
                         const double theFieldStepRangeMax );
  void getFieldStepInfo( GEOM::field_data_type& theFieldDataType,
                         int& theFieldDimension,
                         QList<QVariant>& theFieldStepData,
                         TCollection_AsciiString& theFieldStepName,
                         double& theFieldStepRangeMin,
                         double& theFieldStepRangeMax ) const;

protected: 
  void shadingMode(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
                   const Handle(Prs3d_Presentation)& aPrs,
                   const Standard_Integer aMode);

  // Displaying the field data
  void drawField( const Handle(Prs3d_Presentation)& thePrs,
                  const bool theIsText = false,
                  const bool theIsHighlight = false );

  // Auxiliary method to compute a center of mass for the specified shape
  static Standard_Boolean computeMassCenter( const TopoDS_Shape& theShape,
                                             gp_Pnt& theCenter );

  Quantity_Color myShadingColor;
  Quantity_Color myEdgesInShadingColor;

private: 
  TCollection_AsciiString  myName;
  bool                     myDisplayVectors;
  Standard_Boolean         myTopLevel;
  Standard_Integer         myPrevDisplayMode;

  GEOM::field_data_type    myFieldDataType;
  int                      myFieldDimension;
  QList<QVariant>          myFieldStepData;
  TCollection_AsciiString  myFieldStepName;
  double                   myFieldStepRangeMin;
  double                   myFieldStepRangeMax;

  static TopLevelDispMode myTopLevelDm;
  static Quantity_Color   myTopLevelColor;
};


// other inline functions and methods (like "C++: function call" methods)
//


#endif
