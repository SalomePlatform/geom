// Copyright (C) 2014-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// internal includes
#include "XAOPlugin_IECallBack.hxx"
#include "XAOPlugin_IOperations.hxx"
#include "XAOPlugin_OperationsCreator.hxx"
#include "GEOMImpl_IShapesOperations.hxx"

//=============================================================================
/*!
 *  constructor
 */
//=============================================================================
XAOPlugin_IECallBack::XAOPlugin_IECallBack()
{
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
XAOPlugin_IECallBack::~XAOPlugin_IECallBack()
{
}

//=============================================================================
/*!
 *  Export
 */
//=============================================================================
bool
XAOPlugin_IECallBack::Export( int                            theDocId,
			      const Handle(GEOM_Object)      theOriginal,
			      const TCollection_AsciiString& theFileName,
			      const TCollection_AsciiString& theFormatName )
{
  XAOPlugin_IOperations* aPluginOperations = XAOPlugin_OperationsCreator::get( GetEngine(), theDocId );
  GEOMImpl_IShapesOperations* aShapesOperations = GetEngine()->GetIShapesOperations( theDocId );

  Handle(TColStd_HSequenceOfTransient) groups = aShapesOperations->GetExistingSubObjects( theOriginal, GEOMImpl_IShapesOperations::Groups );
  Handle(TColStd_HSequenceOfTransient) fields = aShapesOperations->GetExistingSubObjects( theOriginal, GEOMImpl_IShapesOperations::Fields );
  std::list<Handle(GEOM_Object)> lgroups;
  std::list<Handle(GEOM_Field)> lfields;
  for (int i = 1; i <= groups->Length(); i++)
    lgroups.push_back( Handle(GEOM_Object)::DownCast( groups->Value(i) ) );
  for (int i = 1; i <= fields->Length(); i++)
    lfields.push_back( Handle(GEOM_Field)::DownCast( fields->Value(i) ) );
  aPluginOperations->ExportXAO( theOriginal, lgroups, lfields, "SIMAN Author", theFileName.ToCString() );
  return true;
}

//=============================================================================
/*!
 *  Import
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient)
XAOPlugin_IECallBack::Import( int                            theDocId,
			      const TCollection_AsciiString& theFormatName,
			      const TCollection_AsciiString& theFileName )
{
  XAOPlugin_IOperations* aPluginOperations = XAOPlugin_OperationsCreator::get( GetEngine(), theDocId );

  Handle(TColStd_HSequenceOfTransient) result = new TColStd_HSequenceOfTransient();
  Handle(TColStd_HSequenceOfTransient) subshapes = new TColStd_HSequenceOfTransient();
  Handle(TColStd_HSequenceOfTransient) groups = new TColStd_HSequenceOfTransient();
  Handle(TColStd_HSequenceOfTransient) fields = new TColStd_HSequenceOfTransient();
  Handle(GEOM_Object) shape;

  bool ok = aPluginOperations->ImportXAO( theFileName.ToCString(), shape, subshapes, groups, fields );
  if ( ok ) {
    result->Append( shape );
    for ( int i = 1; i <= groups->Length(); i++ )
      result->Append( groups->Value(i) );
    for ( int i = 1; i <= fields->Length(); i++ )
      result->Append( fields->Value(i) );
  }
  return result;
}
