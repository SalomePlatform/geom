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

//NOTE: This is an intreface to a function for the Transfer Data functionality.
//


#include "GEOMImpl_ITransferData.hxx"

#include <TDataStd_Comment.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>


//=============================================================================
/*!
 *  SetName
 */
//=============================================================================
void GEOMImpl_ITransferData::SetStringData
                  (const TopoDS_Shape            &theSubShape,
                   const TCollection_AsciiString &theData,
                   const Standard_Boolean         IsName)
{
  TDF_Label           aLabel;
  TDF_ChildIDIterator anIt
    (_func->GetNamingEntry(), TDataStd_Name::GetID(), Standard_True);

  for (; anIt.More(); anIt.Next()) {
    Handle(TDataStd_Name) aNameAttr =
        Handle(TDataStd_Name)::DownCast(anIt.Value());

    if (aNameAttr.IsNull()) {
      continue;
    }

    TDF_Label aLab = aNameAttr->Label();
    Handle(TNaming_NamedShape) aShAttr;

    if (aLab.FindAttribute(TNaming_NamedShape::GetID(), aShAttr) &&
        aShAttr->Get().IsEqual(theSubShape)) {
      aLabel = aLab;
      break;
    }
  }

  if (aLabel.IsNull()) {
    TDF_TagSource aTag;

    aLabel = aTag.NewChild(_func->GetNamingEntry());

    TNaming_Builder aTnBuild (aLabel);

    aTnBuild.Generated(theSubShape);
  }

  // set a name or a material
  TCollection_ExtendedString aDataExt(theData);

  if (IsName) {
    TDataStd_Name::Set(aLabel, aDataExt);
  } else {
    TDataStd_Comment::Set(aLabel, aDataExt);
  }
}

//=============================================================================
/*!
 *  GetStringData
 */
//=============================================================================
TCollection_AsciiString GEOMImpl_ITransferData::GetStringData
                  (const TopoDS_Shape     &theSubShape,
                   const Standard_Boolean  IsName)
{
  // check all named shapes using iterator
  TCollection_AsciiString aResult;
  TDF_ChildIDIterator     anIt
    (_func->GetNamingEntry(), TNaming_NamedShape::GetID(), Standard_True);

  for (; anIt.More(); anIt.Next()) {
    Handle(TNaming_NamedShape) aShAttr =
        Handle(TNaming_NamedShape)::DownCast(anIt.Value());

    if (aShAttr.IsNull()) {
      continue;
    }

    if (aShAttr->Get().IsEqual(theSubShape)) {
      TDF_Label        aLabel  = aShAttr->Label();
      Standard_Boolean isFound = Standard_False;

      if (IsName) {
        Handle(TDataStd_Name) aString;

        if(aLabel.FindAttribute(TDataStd_Name::GetID(), aString)) {
          aResult = TCollection_AsciiString(aString->Get());
          isFound = Standard_True;
        }
      } else {
        Handle(TDataStd_Comment) aString;

        if(aLabel.FindAttribute(TDataStd_Comment::GetID(), aString)) {
          aResult = TCollection_AsciiString(aString->Get());
          isFound = Standard_True;
        }
      }

      if (isFound) {
        break;
      }
    }
  }

  return aResult;
}
