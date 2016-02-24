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

// File      : GEOM_BaseDriver.cxx
// Created   : Thu Jun  6 15:44:27 2013
// Author    : Edward AGAPOV (eap)

#include "GEOM_BaseDriver.hxx"

#include "GEOM_Function.hxx"
#include "GEOM_Object.hxx"
#include "GEOM_Engine.hxx"

#include <TDataStd_Name.hxx>
#include <TDocStd_Owner.hxx>

OCCT_IMPLEMENT_STANDARD_RTTIEXT(GEOM_BaseDriver,TFunction_Driver);

//================================================================================
/*!
 * Returns document id
 */
//================================================================================
int GEOM_BaseDriver::GetDocID() const
{
  int docId = 0;
  if (!Label().IsNull()) {
    Handle(TDocStd_Document) aDoc = TDocStd_Owner::GetDocument(Label().Data());
    docId = GEOM_Engine::GetEngine()->GetDocID(aDoc);
  }
  return docId;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

// bool GEOM_BaseDriver::GetCreationInformation(std::string&             theOperationName,
//                                              std::vector<GEOM_Param>& theParams) = 0;

//================================================================================
/*!
 * \brief Adds GEOM_Param to params and sets its name
 *
 * This method is safer than resizing the params vector and accessing to its items
 */
//================================================================================

GEOM_Param& GEOM_BaseDriver::AddParam(std::vector<GEOM_Param>& params,
                                      const char*              name)
{
  GEOM_Param p;
  p.Set( name );
  params.push_back( p );
  return params.back();
}

//================================================================================
/*!
 * \brief Appends a GEOM_Function value
 */
//================================================================================

GEOM_Param & GEOM_Param::operator<<( const Handle(Standard_Transient)& fun )
{
  return *this << Handle(GEOM_Function)::DownCast( fun );
}

//================================================================================
/*!
 * \brief Appends a GEOM_Function value
 */
//================================================================================

GEOM_Param & GEOM_Param::operator<<( const Handle(GEOM_Function)& fun )
{
  if ( !fun.IsNull() )
  {
    TDF_Label label = fun->GetOwnerEntry();
    Handle(GEOM_Object) obj = GEOM_Object::GetObject( label );
    if ( !obj.IsNull() )
    {
      TopoDS_Shape s = obj->GetValue();
      (*this) << ( s.IsNull() ? TopAbs_SHAPE : s.ShapeType() );

      TCollection_AsciiString entry = obj->GetAuxData();
      TCollection_ExtendedString name;
      {
        Handle(TDataStd_Name) aNameAttr;
        if( obj->GetEntry().FindAttribute(TDataStd_Name::GetID(), aNameAttr))
          name = aNameAttr->Get();
      }
      if ( name.Length() > 0 && entry.Length() > 0 )
        (*this) << "('" <<  name << "'," << entry << ")";
      else if ( name.Length() > 0 )
        (*this) << "('" << name << "')";
      else if ( entry.Length() > 0 )
        (*this) << "(" << entry << ")";
    }
  }
  return *this;
}

//================================================================================
/*!
 * \brief Appends several GEOM_Function's to the value
 */
//================================================================================

GEOM_Param & GEOM_Param::operator<<( const Handle(TColStd_HSequenceOfTransient)& funs )
{
  if ( !funs.IsNull() )
  {
    if ( funs->Length() > 1 )
      (*this) << funs->Length() << " objects: ";
    for ( int i = 1; i <= funs->Length(); ++i )
      (*this) << funs->Value( i ) << " ";
  }
  return *this;
}

template <class HSEQ> void appendSeq( GEOM_Param& param,
                                      const HSEQ& seq,
                                      int         iLow,
                                      int         iUp)
{
  int len = 1 + iUp - iLow;
  if ( len > 1 )
    param << len << " items: ";
  for ( ; iLow <= iUp; ++iLow )
    param << seq->Value( iLow ) << " ";
}

//================================================================================
/*!
 * \brief Appends several int's to the value
 */
//================================================================================

GEOM_Param & GEOM_Param::operator<<( const Handle(TColStd_HArray1OfInteger)& vals )
{
  if ( !vals.IsNull() )
    appendSeq( *this, vals, vals->Lower(), vals->Upper() );
  return *this;
}

//================================================================================
/*!
 * \brief Appends TopAbs_ShapeEnum to the value
 */
//================================================================================

GEOM_Param & GEOM_Param::operator<<( TopAbs_ShapeEnum type )
{
  const char* str[] = {
    "Compound","Compsolid","Solid","Shell","Face","Wire","Edge","Vertex","Shape"
  };
  if ( 0 <= type && type <= TopAbs_SHAPE )
    (*this) << str[type];
  else
    (*this) << "TopAbs_ShapeEnum(" << type << ")";
  return *this;
}

//================================================================================
/*!
 * \brief Appends TopAbs_State to the value
 */
//================================================================================

GEOM_Param & GEOM_Param::operator<<( TopAbs_State state )
{
  const char* str[] = {"IN","OUT","ON","UNKNOWN"};
  if ( 0 <= state && state <= TopAbs_UNKNOWN )
    (*this) << str[state];
  else
    (*this) << "TopAbs_State(" << state << ")";
  return *this;
}
