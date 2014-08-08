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

#ifndef _GEOMImpl_IInsertOperations_HXX_
#define _GEOMImpl_IInsertOperations_HXX_

#include "GEOM_IOperations.hxx"
#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"
#include "GEOM_Field.hxx"

#include <Basics_OCCTVersion.hxx>

#include "Utils_SALOME_Exception.hxx"

#include <TDocStd_Document.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_List.hxx>
#include <Resource_Manager.hxx>

#include <list>

class GEOMImpl_IShapesOperations;
class GEOMImpl_IGroupOperations;
class GEOMImpl_IFieldOperations;

class Handle_TColStd_HArray1OfByte;

namespace XAO {
  class Geometry;
  class BrepGeometry;
  class Xao;
}

typedef NCollection_DataMap<TCollection_ExtendedString, NCollection_List<TopoDS_Shape> >
        DataMapOfStringListOfShape;

class GEOMImpl_IInsertOperations : public GEOM_IOperations {
 public:
  Standard_EXPORT GEOMImpl_IInsertOperations(GEOM_Engine* theEngine, int theDocID);
  Standard_EXPORT ~GEOMImpl_IInsertOperations();


  Standard_EXPORT Handle(GEOM_Object) MakeCopy (Handle(GEOM_Object) theOriginal);
  
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) Import
                         (const TCollection_AsciiString& theFileName,
                          const TCollection_AsciiString& theFormatType);
  
  Standard_EXPORT TCollection_AsciiString ReadValue (const TCollection_AsciiString& theFileName,
                                                     const TCollection_AsciiString& theFormatType,
                                                     const TCollection_AsciiString& theParameterName);

  Standard_EXPORT void Export (const Handle(GEOM_Object)      theOriginal,
                               const TCollection_AsciiString& theFileName,
                               const TCollection_AsciiString& theFormatType);
  
  Standard_EXPORT Standard_Boolean ImportTranslators (Handle(TColStd_HSequenceOfAsciiString)& theFormats,
                                                      Handle(TColStd_HSequenceOfAsciiString)& thePatterns);
  
  Standard_EXPORT Standard_Boolean ExportTranslators (Handle(TColStd_HSequenceOfAsciiString)& theFormats,
                                                      Handle(TColStd_HSequenceOfAsciiString)& thePatterns);
  
  Standard_EXPORT Standard_Boolean IsSupported (const Standard_Boolean isImport,
                                                const TCollection_AsciiString& theFormat,
                                                Handle(TCollection_HAsciiString)& theLibName);
  
  Standard_EXPORT Handle(GEOM_Object) RestoreShape (std::istringstream& theStream);
  
  Standard_EXPORT int LoadTexture(const TCollection_AsciiString& theTextureFile);
  
  Standard_EXPORT int AddTexture(int theWidth, int theHeight,
                                 const Handle(TColStd_HArray1OfByte)& theTexture);

  Standard_EXPORT Handle(TColStd_HArray1OfByte) GetTexture(int theTextureId,
							                               int& theWidth, int& theHeight);

  Standard_EXPORT std::list<int> GetAllTextures();

  Standard_EXPORT bool ExportXAO(Handle(GEOM_Object) shape,
                                 std::list<Handle(GEOM_Object)> groupList,
                                 std::list<Handle(GEOM_Field)> fieldList,
                                 const char* author,
                                 const char* fileName);

  Standard_EXPORT bool ImportXAO(const char* fileName,
                                 Handle(GEOM_Object)& shape,
                                 Handle(TColStd_HSequenceOfTransient)& subShapes,
                                 Handle(TColStd_HSequenceOfTransient)& groups,
                                 Handle(TColStd_HSequenceOfTransient)& fields);

 private:
  Standard_Boolean InitResMgr ();

  void importSubShapes(XAO::Geometry* xaoGeometry, Handle(GEOM_Function) function,
                       int shapeType, int dim,
                       Handle(TColStd_HSequenceOfTransient)& subshapeList);
  void exportSubshapes(const Handle(GEOM_Object)& shape, XAO::BrepGeometry* geometry);
  void exportFields(std::list<Handle(GEOM_Field)> fieldList, XAO::Xao* xaoObject,
                    XAO::BrepGeometry* geometry);
  void exportGroups(std::list<Handle(GEOM_Object)> groupList, XAO::Xao* xaoObject,
                    XAO::BrepGeometry* geometry);

  void MakeMaterialGroups(const Handle(GEOM_Object)                  &theObject,
                          const Handle(TColStd_HSequenceOfTransient) &theSeq);

  Handle(GEOM_Object) MakeGroup
                         (const Handle(GEOM_Object)            &theObject,
                          const TCollection_ExtendedString     &theName,
                          const NCollection_List<TopoDS_Shape> &theShapes);

 private:
  std::vector<Handle(Resource_Manager)> myResMgrList;
  GEOMImpl_IShapesOperations* myShapesOperations;
  GEOMImpl_IGroupOperations* myGroupOperations;
  GEOMImpl_IFieldOperations* myFieldOperations;
};

#endif
