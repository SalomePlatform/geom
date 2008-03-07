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

#ifndef _NMTTools_DSFiller_HeaderFile
#define _NMTTools_DSFiller_HeaderFile

#ifndef _TopoDS_Shape_HeaderFile
#include <TopoDS_Shape.hxx>
#endif
#ifndef _NMTDS_PShapesDataStructure_HeaderFile
#include <NMTDS_PShapesDataStructure.hxx>
#endif
#ifndef _BOPTools_PInterferencePool_HeaderFile
#include <BOPTools_PInterferencePool.hxx>
#endif
#ifndef _NMTTools_PPaveFiller_HeaderFile
#include <NMTTools_PPaveFiller.hxx>
#endif
#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
class TopoDS_Shape;
class NMTDS_ShapesDataStructure;
class BOPTools_InterferencePool;
class NMTTools_PaveFiller;


#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif


class NMTTools_DSFiller  {

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


Standard_EXPORT NMTTools_DSFiller();

//! Destructor <br>
//! <br>
//!  Selectors/Modifiers <br>
Standard_EXPORT   void Destroy() ;
~NMTTools_DSFiller()
{
  Destroy();
}



Standard_EXPORT   void SetCompositeShape(const TopoDS_Shape& aS) ;


Standard_EXPORT  const TopoDS_Shape& CompositeShape() const;


Standard_EXPORT   void SetNewFiller(const Standard_Boolean aFlag) const;


Standard_EXPORT   Standard_Boolean IsNewFiller() const;


Standard_EXPORT   void Perform() ;


Standard_EXPORT  const NMTDS_ShapesDataStructure& DS() const;


Standard_EXPORT  const BOPTools_InterferencePool& InterfPool() const;


Standard_EXPORT   Standard_Boolean IsDone() const;


Standard_EXPORT  const NMTTools_PaveFiller& PaveFiller() const;


Standard_EXPORT   NMTTools_PaveFiller& ChangePaveFiller() ;





protected:

 // Methods PROTECTED
 // 


Standard_EXPORT   void Clear() ;


 // Fields PROTECTED
 //
TopoDS_Shape myCompositeShape;
NMTDS_PShapesDataStructure myDS;
BOPTools_PInterferencePool myInterfPool;
NMTTools_PPaveFiller myPaveFiller;
Standard_Boolean myIsDone;
Standard_Boolean myIsNewFiller;


private: 

 // Methods PRIVATE
 // 


 // Fields PRIVATE
 //


};





// other Inline functions and methods (like "C++: function call" methods)
//


#endif
