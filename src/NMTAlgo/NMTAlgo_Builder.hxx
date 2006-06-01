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

#ifndef _NMTAlgo_Builder_HeaderFile
#define _NMTAlgo_Builder_HeaderFile

#ifndef _BRepAlgo_Image_HeaderFile
#include <BRepAlgo_Image.hxx>
#endif
#ifndef _TopTools_IndexedMapOfShape_HeaderFile
#include <TopTools_IndexedMapOfShape.hxx>
#endif
#ifndef _TopTools_IndexedDataMapOfShapeListOfShape_HeaderFile
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#endif
#ifndef _TopTools_IndexedDataMapOfShapeShape_HeaderFile
#include <TopTools_IndexedDataMapOfShapeShape.hxx>
#endif
#ifndef _NMTAlgo_Algo_HeaderFile
#include <NMTAlgo_Algo.hxx>
#endif
#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
class NMTTools_DSFiller;
class TopoDS_Edge;
class TopoDS_Face;


#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

class NMTAlgo_Builder  : public NMTAlgo_Algo {

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
Standard_EXPORT NMTAlgo_Builder();
Standard_EXPORT virtual ~NMTAlgo_Builder();
Standard_EXPORT virtual  void Clear() ;
Standard_EXPORT virtual  void ComputeWithFiller(const NMTTools_DSFiller& aDSF) ;





protected:

 // Methods PROTECTED
 // 
Standard_EXPORT   void FillImagesEdges() ;
Standard_EXPORT   void FillIn2DParts() ;
Standard_EXPORT   void FillImagesFaces() ;
Standard_EXPORT   void FillSDFaces() ;
Standard_EXPORT   void SplitVertices() ;
Standard_EXPORT   Standard_Boolean IsSectionEdge(const TopoDS_Edge& E) const;
Standard_EXPORT   Standard_Boolean HasSameDomainF(const TopoDS_Face& F) const;
Standard_EXPORT   Standard_Boolean IsSameDomainF(const TopoDS_Face& F1,const TopoDS_Face& F2) const;


 // Fields PROTECTED
 //
BRepAlgo_Image myImagesEdges;
BRepAlgo_Image myImagesFaces;
TopTools_IndexedMapOfShape myQueryShapes;
TopTools_IndexedDataMapOfShapeListOfShape myIn2DParts;
TopTools_IndexedDataMapOfShapeListOfShape mySectionParts;
TopTools_IndexedDataMapOfShapeShape mySDFaces;


private: 

 // Methods PRIVATE
 // 


 // Fields PRIVATE
 //


};





// other Inline functions and methods (like "C++: function call" methods)
//


#endif
