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

#ifndef _NMTAlgo_Splitter_HeaderFile
#define _NMTAlgo_Splitter_HeaderFile

#ifndef _TopAbs_ShapeEnum_HeaderFile
#include <TopAbs_ShapeEnum.hxx>
#endif
#ifndef _BRep_Builder_HeaderFile
#include <BRep_Builder.hxx>
#endif
#ifndef _TopTools_ListOfShape_HeaderFile
#include <TopTools_ListOfShape.hxx>
#endif
#ifndef _TopTools_MapOfShape_HeaderFile
#include <TopTools_MapOfShape.hxx>
#endif
#ifndef _TopTools_DataMapOfShapeShape_HeaderFile
#include <TopTools_DataMapOfShapeShape.hxx>
#endif
#ifndef _BRepAlgo_Image_HeaderFile
#include <BRepAlgo_Image.hxx>
#endif
#ifndef _TopTools_MapOfOrientedShape_HeaderFile
#include <TopTools_MapOfOrientedShape.hxx>
#endif
#ifndef _TopTools_DataMapOfShapeListOfShape_HeaderFile
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#endif
#ifndef _NMTAlgo_Builder_HeaderFile
#include <NMTAlgo_Builder.hxx>
#endif
#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
class TopoDS_Shape;
class NMTTools_DSFiller;
class TopTools_ListOfShape;


#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

class NMTAlgo_Splitter  : public NMTAlgo_Builder {

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
Standard_EXPORT NMTAlgo_Splitter();
Standard_EXPORT virtual ~NMTAlgo_Splitter();
Standard_EXPORT virtual  void AddShape(const TopoDS_Shape& S) ;
Standard_EXPORT virtual  void AddTool(const TopoDS_Shape& S) ;
Standard_EXPORT virtual  void Compute() ;
Standard_EXPORT virtual  void Build(const TopAbs_ShapeEnum aLimit = TopAbs_SHAPE) ;
Standard_EXPORT virtual  void ComputeWithFiller(const NMTTools_DSFiller& aDSF) ;
Standard_EXPORT   void KeepShapesInside(const TopoDS_Shape& S) ;
Standard_EXPORT   void RemoveShapesInside(const TopoDS_Shape& S) ;
Standard_EXPORT  const TopTools_ListOfShape& Modified(const TopoDS_Shape& S) ;
Standard_EXPORT  const TopTools_ListOfShape& Generated(const TopoDS_Shape& S) ;
Standard_EXPORT   Standard_Boolean IsDeleted(const TopoDS_Shape& S) ;
Standard_EXPORT virtual  void Clear() ;
Standard_EXPORT  const TopTools_ListOfShape& SourceShapes() const;





protected:

 // Methods PROTECTED
 // 
Standard_EXPORT   void FillResult() ;
Standard_EXPORT   void FillImageShape() ;
Standard_EXPORT   void SplittedFaces() ;
Standard_EXPORT   void SplittedWires() ;
Standard_EXPORT   void SplitsAndSections() ;
Standard_EXPORT   void ShellsAndSolids() ;
Standard_EXPORT   void MakeShells(const TopoDS_Shape& aS,TopTools_ListOfShape& aLNS) ;
Standard_EXPORT   void MakeSolids(const TopoDS_Shape& Solid,TopTools_ListOfShape& Shells) ;
Standard_EXPORT   TopoDS_Shape FindFacesInside(const TopoDS_Shape& S,const Standard_Boolean CheckClosed = Standard_False,const Standard_Boolean All = Standard_False) ;
Standard_EXPORT static  Standard_Boolean IsInside(const TopoDS_Shape& S1,const TopoDS_Shape& S2) ;
Standard_EXPORT   TopoDS_Shape GetOriginalShape(const TopoDS_Shape& aShape) const;
Standard_EXPORT   void FindImage(const TopoDS_Shape& aS,TopTools_ListOfShape& aLIms) ;


 // Fields PROTECTED
 //
TopAbs_ShapeEnum myDoneStep;
BRep_Builder myBuilder;
TopTools_ListOfShape myListShapes;
TopTools_MapOfShape myMapFaces;
TopTools_MapOfShape myMapTools;
TopTools_MapOfShape myEqualEdges;
TopTools_MapOfShape myNewSection;
TopTools_MapOfShape myClosedShapes;
TopTools_MapOfShape myWrappingSolid;
TopTools_DataMapOfShapeShape myFaceShapeMap;
TopTools_DataMapOfShapeShape myInternalFaces;
TopTools_DataMapOfShapeShape myIntNotClFaces;
BRepAlgo_Image myImageShape;
TopTools_MapOfOrientedShape myAddedFacesMap;
TopTools_ListOfShape mySourceShapes;
TopAbs_ShapeEnum myLimit;
TopTools_MapOfShape myToolShapes;
TopTools_MapOfShape myObjShapes;
TopTools_DataMapOfShapeShape myMapSIFC;
TopTools_ListOfShape myGenerated;
TopTools_DataMapOfShapeListOfShape myModifiedFaces;


private: 

 // Methods PRIVATE
 // 


 // Fields PRIVATE
 //


};





// other Inline functions and methods (like "C++: function call" methods)
//


#endif
