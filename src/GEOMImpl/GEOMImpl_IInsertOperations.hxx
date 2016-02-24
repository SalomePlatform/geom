// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
#include <TColStd_HArray1OfByte.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_List.hxx>
#include <Resource_Manager.hxx>

#include <list>

class GEOMImpl_IShapesOperations;
class GEOMImpl_IGroupOperations;
class GEOMImpl_IFieldOperations;

class GEOMImpl_IInsertOperations : public GEOM_IOperations {
 public:

  struct TransferDatum
  {
    TCollection_AsciiString myName;
    long                    myNumber;
    long                    myMaxNumber;
  };

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
  
  Standard_EXPORT Handle(GEOM_Object) RestoreShape (std::istringstream& theStream);
  
  Standard_EXPORT int LoadTexture(const TCollection_AsciiString& theTextureFile);
  
  Standard_EXPORT int AddTexture(int theWidth, int theHeight,
                                 const Handle(TColStd_HArray1OfByte)& theTexture);

  Standard_EXPORT Handle(TColStd_HArray1OfByte) GetTexture(int theTextureId,
							                               int& theWidth, int& theHeight);

  Standard_EXPORT std::list<int> GetAllTextures();

  Standard_EXPORT bool TransferData
                          (const Handle(GEOM_Object)      &theObjectFrom,
                           const Handle(GEOM_Object)      &theObjectTo,
                           const int                       theFindMethod,
                                 std::list<TransferDatum> &theResult);

 private:
  std::vector<Handle(Resource_Manager)> myResMgrList;
  GEOMImpl_IShapesOperations* myShapesOperations;
  GEOMImpl_IGroupOperations* myGroupOperations;
  GEOMImpl_IFieldOperations* myFieldOperations;
};

#endif
