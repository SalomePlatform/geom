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

#ifndef _NMTAlgo_Tools_HeaderFile
#define _NMTAlgo_Tools_HeaderFile

#ifndef _TopAbs_Orientation_HeaderFile
#include <TopAbs_Orientation.hxx>
#endif
#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif
#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
class TopoDS_Shell;
class TopoDS_Face;
class TopoDS_Edge;
class TopoDS_Shape;
class TopTools_ListOfShape;
class TopTools_IndexedMapOfShape;


#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

class NMTAlgo_Tools  {

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
Standard_EXPORT static  void OrientFacesOnShell(const TopoDS_Shell& aShell,TopoDS_Shell& aShellNew) ;
Standard_EXPORT static  void OrientFacesOnShell(const TopoDS_Face& aF,TopoDS_Shell& aSh) ;
Standard_EXPORT static  TopAbs_Orientation Orientation(const TopoDS_Edge& aE,const TopoDS_Face& aF) ;
Standard_EXPORT static  Standard_Integer Sense(const TopoDS_Face& aF1,const TopoDS_Face& aF2) ;
Standard_EXPORT static  Standard_Boolean IsInside(const TopoDS_Shape& aS1,const TopoDS_Shape& aS2) ;
Standard_EXPORT static  void MakeShells(const TopoDS_Shape& aFC,TopTools_ListOfShape& aLS) ;
Standard_EXPORT static  void MakeSolids(TopTools_ListOfShape& aLS) ;
Standard_EXPORT static  void MakeSolids(const TopoDS_Shape& aFC,TopTools_ListOfShape& aLS) ;
Standard_EXPORT static  void BreakWebs(const TopoDS_Shape& aS1,TopoDS_Shape& aS2) ;
Standard_EXPORT static  Standard_Boolean FindImageSolid(const TopoDS_Shape& aFC,const TopTools_IndexedMapOfShape& aMSo,TopoDS_Shape& aSo) ;





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


};





// other Inline functions and methods (like "C++: function call" methods)
//


#endif
