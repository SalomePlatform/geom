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
//
//  GEOM PARTITION : partition algorithm
//
//  File   : Partition_Loop2d.hxx
//  Module : GEOM

#ifndef _Partition_Loop2d_HeaderFile
#define _Partition_Loop2d_HeaderFile

#ifndef _TopoDS_Face_HeaderFile
#include <TopoDS_Face.hxx>
#endif
#ifndef _TopAbs_Orientation_HeaderFile
#include <TopAbs_Orientation.hxx>
#endif
#ifndef _TopTools_ListOfShape_HeaderFile
#include <TopTools_ListOfShape.hxx>
#endif
#ifndef _TopTools_MapOfShape_HeaderFile
#include <TopTools_MapOfShape.hxx>
#endif
class TopoDS_Face;
class TopoDS_Edge;
class TopTools_ListOfShape;
class BRepAlgo_Image;


#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

class Partition_Loop2d  {

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
Standard_EXPORT Partition_Loop2d();
Standard_EXPORT   void Init(const TopoDS_Face& F) ;
Standard_EXPORT   void AddConstEdge(const TopoDS_Edge& E) ;
Standard_EXPORT   void AddSectionEdge(const TopoDS_Edge& E) ;
Standard_EXPORT   void Perform() ;
Standard_EXPORT  const TopTools_ListOfShape& NewWires() const;
Standard_EXPORT   void WiresToFaces(const BRepAlgo_Image& EdgeImage) ;
Standard_EXPORT  const TopTools_ListOfShape& NewFaces() const;





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
TopoDS_Face myFace;
TopAbs_Orientation myFaceOri;
TopTools_ListOfShape myConstEdges;
TopTools_ListOfShape myNewWires;
TopTools_ListOfShape myNewFaces;
TopTools_ListOfShape myInternalWL;
TopTools_MapOfShape mySectionEdges;


};





// other Inline functions and methods (like "C++: function call" methods)
//


#endif
