// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com

#ifndef _NMTAlgo_Splitter1_HeaderFile
#define _NMTAlgo_Splitter1_HeaderFile

#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
#ifndef _TopTools_DataMapOfShapeInteger_HeaderFile
#include <TopTools_DataMapOfShapeInteger.hxx>
#endif
#ifndef _TopoDS_Shape_HeaderFile
#include <TopoDS_Shape.hxx>
#endif
#ifndef _NMTAlgo_Splitter_HeaderFile
#include <NMTAlgo_Splitter.hxx>
#endif
#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif
#ifndef _TopAbs_ShapeEnum_HeaderFile
#include <TopAbs_ShapeEnum.hxx>
#endif
class TopoDS_Shape;
class TopTools_DataMapOfShapeInteger;


#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

class NMTAlgo_Splitter1  : public NMTAlgo_Splitter {

public:

    void* operator new(size_t,void* anAddress) 
      {
        return anAddress;
      }
    void* operator new(size_t size) 
      { 
        return Standard::Allocate(size); 
      }
    void  operator delete(void *anAddress) 
      { 
        if (anAddress) Standard::Free((Standard_Address&)anAddress); 
      }
 // Methods PUBLIC
 // 
Standard_EXPORT NMTAlgo_Splitter1();
Standard_EXPORT virtual ~NMTAlgo_Splitter1();
Standard_EXPORT virtual  void Clear() ;
Standard_EXPORT virtual  void AddShape(const TopoDS_Shape& aS) ;
Standard_EXPORT virtual  void AddTool(const TopoDS_Shape& aS) ;
Standard_EXPORT   void SetMaterial(const TopoDS_Shape& aS,const Standard_Integer aM = 0) ;
Standard_EXPORT   void SetRemoveWebs(const Standard_Boolean bFlag) ;
Standard_EXPORT   Standard_Boolean RemoveWebs() const;
Standard_EXPORT  const TopTools_DataMapOfShapeInteger& GetMaterialTable() const;
Standard_EXPORT virtual  void Build(const TopAbs_ShapeEnum Limit = TopAbs_SHAPE) ;





protected:

 // Methods PROTECTED
 // 
Standard_EXPORT   void TreatSolids() ;
Standard_EXPORT   void TreatWebs() ;
Standard_EXPORT   void RestParts() ;


 // Fields PROTECTED
 //
Standard_Boolean myRemoveWebs;
TopTools_DataMapOfShapeInteger myMapSWM;
TopTools_DataMapOfShapeInteger myMapSWMOut;
TopoDS_Shape myRestParts;


private: 

 // Methods PRIVATE
 // 


 // Fields PRIVATE
 //


};





// other Inline functions and methods (like "C++: function call" methods)
//


#endif
