//  GEOM OBJECT : interactive object for Geometry entities visualization
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : GEOM_AISShape.hxx
//  Module : GEOM

#ifndef _GEOM_AISShape_HeaderFile
#define _GEOM_AISShape_HeaderFile

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
#ifndef _Handle_PrsMgr_PresentationManager3d_HeaderFile
#include <Handle_PrsMgr_PresentationManager3d.hxx>
#endif
#ifndef _Handle_Prs3d_Presentation_HeaderFile
#include <Handle_Prs3d_Presentation.hxx>
#endif

class PrsMgr_PresentationManager3d;
class Prs3d_Presentation;
class SALOME_InteractiveObject;
class TopoDS_Shape;


class GEOM_AISShape : public SALOME_AISShape {

public:

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
Standard_EXPORT   GEOM_AISShape(const TopoDS_Shape& shape, const Standard_CString aName);
Standard_EXPORT   Standard_Boolean hasIO() ;
Standard_EXPORT   void setIO(const Handle(SALOME_InteractiveObject)& name) ;
Standard_EXPORT   void setName(const Standard_CString aName) ;
Standard_EXPORT   Standard_CString getName() ;
Standard_EXPORT   Handle_SALOME_InteractiveObject getIO() ;
Standard_EXPORT   void highlightSubShapes(const TColStd_IndexedMapOfInteger& aIndexMap, const Standard_Boolean aHighlight );
Standard_EXPORT   ~GEOM_AISShape();

Standard_EXPORT   void SetTransparency(const Standard_Real aValue);
Standard_EXPORT   void SetShadingColor(const Quantity_Color &aCol);

Standard_EXPORT virtual  void Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
				      const Handle(Prs3d_Presentation)& aPresentation,
				      const Standard_Integer aMode = 0) ;

 // Type management
 //
 Standard_EXPORT friend Handle_Standard_Type& GEOM_AISShape_Type_();
 Standard_EXPORT const Handle(Standard_Type)& DynamicType() const;
 Standard_EXPORT Standard_Boolean	      IsKind(const Handle(Standard_Type)&) const;

protected:

 // Methods PROTECTED
 // 


 // Fields PROTECTED
 //


private: 

 // Methods PRIVATE
 // 


 // Fields PRIVATE
 //
  Handle_SALOME_InteractiveObject myIO;
  Standard_CString myName;
  Quantity_Color myShadingColor;
};





// other inline functions and methods (like "C++: function call" methods)
//


#endif
