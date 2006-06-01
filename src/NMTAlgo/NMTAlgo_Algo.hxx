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

#ifndef _NMTAlgo_Algo_HeaderFile
#define _NMTAlgo_Algo_HeaderFile

#ifndef _NMTTools_PDSFiller_HeaderFile
#include <NMTTools_PDSFiller.hxx>
#endif
#ifndef _TopoDS_Shape_HeaderFile
#include <TopoDS_Shape.hxx>
#endif
#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif
class NMTTools_DSFiller;
class TopoDS_Shape;


#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

class NMTAlgo_Algo  {

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
Standard_EXPORT   void SetFiller(const NMTTools_DSFiller& aDSF) ;
Standard_EXPORT  const NMTTools_DSFiller& Filler() const;
Standard_EXPORT virtual  void ComputeWithFiller(const NMTTools_DSFiller& aDSF) ;
Standard_EXPORT virtual  void Clear() ;
Standard_EXPORT  const TopoDS_Shape& Shape() const;
Standard_EXPORT   Standard_Boolean IsDone() const;
Standard_EXPORT   Standard_Integer ErrorStatus() const;





protected:

 // Methods PROTECTED
 // 
Standard_EXPORT NMTAlgo_Algo();
Standard_EXPORT virtual ~NMTAlgo_Algo();


 // Fields PROTECTED
 //
NMTTools_PDSFiller myDSFiller;
TopoDS_Shape myShape;
Standard_Boolean myIsDone;
Standard_Boolean myIsComputed;
Standard_Integer myErrorStatus;
Standard_Integer myDraw;


private: 

 // Methods PRIVATE
 // 


 // Fields PRIVATE
 //


};





// other Inline functions and methods (like "C++: function call" methods)
//


#endif
