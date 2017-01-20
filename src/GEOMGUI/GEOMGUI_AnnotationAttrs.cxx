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

// File   : GEOMGUI_AnnotationAttr.cxx
// Author : Anton POLETAEV, Open CASCADE S.A.S.
//

// SALOME includes
#include <GEOMGUI_AnnotationAttrs.h>
#include <GEOM_Annotation.hxx>
#include <SALOMEDSImpl_AttributeParameter.hxx>

// STL includes
#include <string>
#include <vector>
#include <sstream>

IMPLEMENT_STANDARD_RTTIEXT( GEOMGUI_AnnotationAttrs, Standard_Transient )

namespace
{
  static const std::string PARAMETER_COUNT = "GEOMGUI_AnnotationAttrs_Count";

  std::string PARAMETER_I( const std::string& s, const int i ) {
    std::stringstream ss;
    ss << i;
    return std::string( s ) + ss.str();
  }
  std::string PARAMETER_IS_VISIBLE( const int i ) {
    return PARAMETER_I( "GEOMGUI_AnnotationAttrs_IsVisible", i );
  }
  std::string PARAMETER_IS_2D( const int i ) {
    return PARAMETER_I( "GEOMGUI_AnnotationAttrs_Is2D", i );
  }
  std::string PARAMETER_TEXT( const int i ) {
    return PARAMETER_I( "GEOMGUI_AnnotationAttrs_Text", i );
  }
  std::string PARAMETER_POSITION( const int i ) {
    return PARAMETER_I( "GEOMGUI_AnnotationAttrs_Position", i );
  }
  std::string PARAMETER_ATTACH( const int i ) {
    return PARAMETER_I( "GEOMGUI_AnnotationAttrs_Attach", i );
  }
  std::string PARAMETER_SHAPE(  const int i ) {
    return PARAMETER_I( "GEOMGUI_AnnotationAttrs_Shape", i );
  }

  // REGEXP pattern for converting array of entries into plain text string.
  // The pattern has the following structure:
  // ENTRY: { text[string] : visibility[bool] : screen fixed[bool] : position[xyz] : attach[xyz] }
  static const QString PATTERN_ITEM_GROUP = "\\{ (Text=(?::{2,}|.)*:(?!:)Screen=.*:Position=\\{(.*):(.*):(.*)\\}:Attach=\\{(.*):(.*):(.*)\\}:ShapeIdx=.*:ShapeType=.*) \\}";
  static const QString PATTERN_ITEM = "Text=((?::{2,}|.)*):(?!:)Screen=(\\d{1}):Position=\\{(.*):(.*):(.*)\\}:Attach=\\{(.*):(.*):(.*)\\}:ShapeIdx=(\\-?\\d{1,}):ShapeType=(\\d{1})";
  static QString toPattern (const QString& theText,
                            const bool theIsFixed,
                            const gp_Pnt& thePosition,
                            const gp_Pnt& theAttach,
                            const int theShapeIndex,
                            const int theShapeType)
  {
    return QString( "{ Text=" ) + theText +
           QString( ":" ) + QString( "Screen=" ) + QString::number( theIsFixed ? 1 : 0 ) +
           QString( ":" ) + QString( "Position={" ) +
             QString::number( thePosition.X() ) + QString( ":" ) + 
             QString::number( thePosition.Y() ) + QString( ":" ) + 
             QString::number( thePosition.Z() ) + QString( "}" ) + 
           QString( ":" ) + QString( "Attach={" ) +
             QString::number( theAttach.X() ) + QString( ":" ) + 
             QString::number( theAttach.Y() ) + QString( ":" ) + 
             QString::number( theAttach.Z() ) + QString( "}" ) +
           QString( ":" ) + QString( "ShapeIdx=" ) + QString::number( theShapeIndex ) +
           QString( ":" ) + QString( "ShapeType=" ) + QString::number( theShapeType ) +
           QString( " }" );
  }
};

//=================================================================================
// function : FindAttributes
// purpose  : 
//=================================================================================
Handle(GEOMGUI_AnnotationAttrs) GEOMGUI_AnnotationAttrs::FindAttributes( const _PTR(SObject)& theObject )
{
  _PTR(GenericAttribute) aGenericAttr;
  _PTR(AttributeParameter) aParameterMap;

  if ( !theObject || !theObject->FindAttribute( aGenericAttr, "AttributeParameter" ) )
  {
    return Handle(GEOMGUI_AnnotationAttrs)();
  }

  aParameterMap = aGenericAttr;

  if ( !aParameterMap->IsSet( PARAMETER_COUNT, PT_INTEGER ) )
  {
    return Handle(GEOMGUI_AnnotationAttrs)();
  }

  return new GEOMGUI_AnnotationAttrs( theObject, aParameterMap );
}

//=================================================================================
// function : FindOrCreateAttributes
// purpose  : 
//=================================================================================
Handle(GEOMGUI_AnnotationAttrs) GEOMGUI_AnnotationAttrs::FindOrCreateAttributes(
  const _PTR(SObject)& theObject, SalomeApp_Study* theStudy )
{
  _PTR(StudyBuilder) aBuilder = theStudy->studyDS()->NewBuilder();
  _PTR(AttributeParameter) aParameterMap = aBuilder->FindOrCreateAttribute( theObject, "AttributeParameter" );
  if ( !aParameterMap->IsSet( PARAMETER_COUNT, PT_INTEGER ) )
  {
    aParameterMap->SetInt( PARAMETER_COUNT, 0 );
  }

  return new GEOMGUI_AnnotationAttrs( theObject, aParameterMap );
}

//=================================================================================
// function : Remove
// purpose  : 
//=================================================================================
void GEOMGUI_AnnotationAttrs::Remove( const _PTR(SObject)& theObject )
{
  _PTR(GenericAttribute) aGenericAttr;
  _PTR(AttributeParameter) aParameterMap;

  if ( !theObject->FindAttribute( aGenericAttr, "AttributeParameter" ) )
  {
    return;
  }

  aParameterMap = aGenericAttr;

  if ( !aParameterMap->IsSet( PARAMETER_COUNT, PT_INTEGER ) )
  {
    return;
  }

  const int aParamCount = aParameterMap->GetInt( PARAMETER_COUNT );

  for ( int anI = 0; anI < aParamCount; ++anI )
  {
    aParameterMap->RemoveID( PARAMETER_IS_VISIBLE( anI ), PT_BOOLEAN );
    aParameterMap->RemoveID( PARAMETER_IS_2D( anI ), PT_BOOLEAN );
    aParameterMap->RemoveID( PARAMETER_TEXT( anI ), PT_STRING );
    aParameterMap->RemoveID( PARAMETER_POSITION( anI ), PT_REALARRAY );
    aParameterMap->RemoveID( PARAMETER_ATTACH( anI ), PT_REALARRAY );
    aParameterMap->RemoveID( PARAMETER_SHAPE( anI ), PT_INTARRAY );
  }

  aParameterMap->RemoveID( PARAMETER_COUNT, PT_INTEGER );
}

//=================================================================================
// function : ExportAsPropertyString
// purpose  : 
//=================================================================================
QString GEOMGUI_AnnotationAttrs::ExportAsPropertyString() const
{
  QStringList anItems;

  for ( int anI = 0; anI < GetNbAnnotation(); ++anI )
  {
    Properties aEntry;

    GetProperties( anI, aEntry );

    anItems.append( toPattern( aEntry.Text,
                               aEntry.IsScreenFixed,
                               aEntry.Position,
                               aEntry.Attach,
                               aEntry.ShapeIndex,
                               aEntry.ShapeType ) );
  }

  return anItems.join( ":" );
}

//=================================================================================
// function : ImportFromPropertyString
// purpose  : 
//=================================================================================
void GEOMGUI_AnnotationAttrs::ImportFromPropertyString( const QString& theString )
{
  SetNbAnnotation( 0 );

  QRegExp aRegExpItemGroups( PATTERN_ITEM_GROUP );
  QRegExp aRegExpItem( "^" + PATTERN_ITEM + "$" );
  aRegExpItemGroups.setMinimal( true );
  aRegExpItem.setMinimal( true );

  int aPos = 0;
  while ( ( aPos = aRegExpItemGroups.indexIn( theString, aPos ) ) != -1 )
  {
    aPos += aRegExpItemGroups.matchedLength();

    QString aStrItem = aRegExpItemGroups.cap(1);

    if ( aRegExpItem.indexIn( aStrItem ) < 0 )
    {
      continue;
    }

    QString aStrText       = aRegExpItem.cap( 1 );
    QString aStrFixed      = aRegExpItem.cap( 2 );
    QString aStrPosX       = aRegExpItem.cap( 3 );
    QString aStrPosY       = aRegExpItem.cap( 4 );
    QString aStrPosZ       = aRegExpItem.cap( 5 );
    QString aStrAttX       = aRegExpItem.cap( 6 );
    QString aStrAttY       = aRegExpItem.cap( 7 );
    QString aStrAttZ       = aRegExpItem.cap( 8 );
    QString aStrShapeIdx   = aRegExpItem.cap( 9 );
    QString aStrShapeType  = aRegExpItem.cap( 10 );
    aStrText.replace( "::", ":" );

    Properties aEntry;
    aEntry.Text = aStrText;
    aEntry.IsVisible = false;
    aEntry.IsScreenFixed = aStrFixed.toInt() != 0;
    aEntry.Position.SetX( aStrPosX.toDouble() );
    aEntry.Position.SetY( aStrPosY.toDouble() );
    aEntry.Position.SetZ( aStrPosZ.toDouble() );
    aEntry.Attach.SetX( aStrAttX.toDouble() );
    aEntry.Attach.SetY( aStrAttY.toDouble() );
    aEntry.Attach.SetZ( aStrAttZ.toDouble() );
    aEntry.ShapeIndex = aStrShapeIdx.toInt();
    aEntry.ShapeType = aStrShapeType.toInt();

    Append( aEntry );
  }
}

//=================================================================================
// function : SetNbAnnotation
// purpose  : 
//=================================================================================
void GEOMGUI_AnnotationAttrs::SetNbAnnotation( const int theCount ) const
{
  const int aCount = this->GetNbAnnotation();

  if ( aCount < theCount )
  {
    // set default values
    for ( int anI = aCount; anI < theCount; ++anI )
    {
      myParameterMap->SetBool( PARAMETER_IS_VISIBLE( anI ), true );
      myParameterMap->SetBool( PARAMETER_IS_2D( anI ), false );
      myParameterMap->SetString( PARAMETER_TEXT( anI ), std::string() );
      myParameterMap->SetRealArray( PARAMETER_POSITION( anI ), std::vector<double>(3, 0.0) );
      myParameterMap->SetRealArray( PARAMETER_ATTACH( anI ), std::vector<double>(3, 0.0) );
      myParameterMap->SetIntArray( PARAMETER_SHAPE( anI ), std::vector<int>(2, 0) );
    }
  }
  else
  {
    // remove exceeding values
    for ( int anI = theCount; anI < aCount; ++anI )
    {
      myParameterMap->RemoveID( PARAMETER_IS_VISIBLE( anI ), PT_BOOLEAN );
      myParameterMap->RemoveID( PARAMETER_IS_2D( anI ), PT_BOOLEAN );
      myParameterMap->RemoveID( PARAMETER_TEXT( anI ), PT_STRING );
      myParameterMap->RemoveID( PARAMETER_POSITION( anI ), PT_REALARRAY );
      myParameterMap->RemoveID( PARAMETER_ATTACH( anI ), PT_REALARRAY );
      myParameterMap->RemoveID( PARAMETER_SHAPE( anI ), PT_INTARRAY );
    }
  }

  myParameterMap->SetInt( PARAMETER_COUNT, theCount );
}

//=================================================================================
// function : GetNbAnnotation
// purpose  : 
//=================================================================================
int GEOMGUI_AnnotationAttrs::GetNbAnnotation() const
{
  return myParameterMap->GetInt( PARAMETER_COUNT );
}

//=================================================================================
// function : SetVisible
// purpose  : 
//=================================================================================
void GEOMGUI_AnnotationAttrs::SetIsVisible( const int theIndex, const bool theIsVisible )
{
  myParameterMap->SetBool( PARAMETER_IS_VISIBLE( theIndex ), theIsVisible );
}

//=================================================================================
// function : GetIsVisible
// purpose  : 
//=================================================================================
bool GEOMGUI_AnnotationAttrs::GetIsVisible( const int theIndex ) const
{
  return myParameterMap->GetBool( PARAMETER_IS_VISIBLE( theIndex ) );
}

//=================================================================================
// function : SetText
// purpose  : 
//=================================================================================
void GEOMGUI_AnnotationAttrs::SetText( const int theIndex, const QString& theText )
{
  myParameterMap->SetString( PARAMETER_TEXT( theIndex ), theText.toStdString() );
}

//=================================================================================
// function : GetText
// purpose  : 
//=================================================================================
QString GEOMGUI_AnnotationAttrs::GetText( const int theIndex ) const
{
  return QString::fromStdString( myParameterMap->GetString( PARAMETER_TEXT( theIndex ) ) );
}

//=================================================================================
// function : SetIsScreenFixed
// purpose  : 
//=================================================================================
void GEOMGUI_AnnotationAttrs::SetIsScreenFixed( const int theIndex, const bool theIsScreenFixed )
{
  myParameterMap->SetBool( PARAMETER_IS_2D( theIndex ), theIsScreenFixed );
}

//=================================================================================
// function : GetIsScreenFixed
// purpose  : 
//=================================================================================
bool GEOMGUI_AnnotationAttrs::GetIsScreenFixed( const int theIndex ) const
{
  return myParameterMap->GetBool( PARAMETER_IS_2D( theIndex ) );
}

//=================================================================================
// function : SetPosition
// purpose  : 
//=================================================================================
void GEOMGUI_AnnotationAttrs::SetPosition( const int theIndex, const gp_Pnt& thePosition )
{
  std::vector<double> aCoords( 3 );

  aCoords[0] = thePosition.X();
  aCoords[1] = thePosition.Y();
  aCoords[2] = thePosition.Z();

  myParameterMap->SetRealArray( PARAMETER_POSITION( theIndex ), aCoords );
}

//=================================================================================
// function : GetPosition
// purpose  : 
//=================================================================================
gp_Pnt GEOMGUI_AnnotationAttrs::GetPosition( const int theIndex ) const
{
  std::vector<double> aCoords =
    myParameterMap->GetRealArray( PARAMETER_POSITION( theIndex ) );

  return gp_Pnt( aCoords[0], aCoords[1], aCoords[2] );
}

//=================================================================================
// function : SetAttach
// purpose  : 
//=================================================================================
void GEOMGUI_AnnotationAttrs::SetAttach( const int theIndex, const gp_Pnt& theAttach )
{
  std::vector<double> aCoords( 3 );

  aCoords[0] = theAttach.X();
  aCoords[1] = theAttach.Y();
  aCoords[2] = theAttach.Z();

  myParameterMap->SetRealArray( PARAMETER_ATTACH( theIndex ), aCoords );
}

//=================================================================================
// function : GetAttach
// purpose  : 
//=================================================================================
gp_Pnt GEOMGUI_AnnotationAttrs::GetAttach( const int theIndex ) const
{
  std::vector<double> aCoords =
    myParameterMap->GetRealArray( PARAMETER_ATTACH( theIndex ) );

  return gp_Pnt( aCoords[0], aCoords[1], aCoords[2] );
}

//=================================================================================
// function : SetShapeSel
// purpose  : 
//=================================================================================
void GEOMGUI_AnnotationAttrs::SetShapeSel( const int theIndex, const int theShapeType, const int theSubIdx )
{
  std::vector<int> aSelection( 2 );

  aSelection[0] = theShapeType;
  aSelection[1] = theSubIdx;

  myParameterMap->SetIntArray( PARAMETER_SHAPE( theIndex ), aSelection );
}

//=================================================================================
// function : GetShapeSel
// purpose  : 
//=================================================================================
void GEOMGUI_AnnotationAttrs::GetShapeSel( const int theIndex, int& theShapeType, int& theSubIdx ) const
{
  std::vector<int> aSelection =
    myParameterMap->GetIntArray( PARAMETER_SHAPE( theIndex ) );

  theShapeType = aSelection[0];
  theSubIdx    = aSelection[1];
}

//=================================================================================
// function : Append
// purpose  : 
//=================================================================================
void GEOMGUI_AnnotationAttrs::Append( const Properties& theProps )
{
  this->Append( theProps, gp_Ax3() );
}

//=================================================================================
// function : Append
// purpose  : 
//=================================================================================
void GEOMGUI_AnnotationAttrs::Append( const Properties& theProps, const gp_Ax3& theShapeLCS )
{
  const int aCount = this->GetNbAnnotation();
  this->SetNbAnnotation( aCount + 1 );
  this->SetProperties( aCount, theProps, theShapeLCS );
}

//=================================================================================
// function : Remove
// purpose  : 
//=================================================================================
void GEOMGUI_AnnotationAttrs::Remove( const Standard_Integer theIndex )
{
  const int aCount = this->GetNbAnnotation();
  if ( theIndex < 0 || theIndex >= aCount ) {
    return;
  }

  std::vector<Properties> aEntries( (aCount - 1) - theIndex );
  for ( int anI = theIndex + 1; anI < aCount; ++anI ) {
    GetProperties( anI, aEntries[ anI - (theIndex + 1) ] );
  }

  SetNbAnnotation( theIndex );

  for ( int anI = 0; anI < static_cast<int>( aEntries.size() ); ++anI ) {
    Append( aEntries[anI] );
  }
}

//=================================================================================
// function : SetProperties
// purpose  : 
//=================================================================================
void GEOMGUI_AnnotationAttrs::SetProperties( const int theIndex, const Properties& theProps )
{
  this->SetProperties( theIndex, theProps, gp_Ax3() );
}

//=================================================================================
// function : SetProperties
// purpose  : 
//=================================================================================
void GEOMGUI_AnnotationAttrs::SetProperties( const int theIndex, const Properties& theProps,
                                             const gp_Ax3& theShapeLCS )
{
  gp_Trsf aToShapeLCS;
  aToShapeLCS.SetTransformation( gp_Ax3(), theShapeLCS );

  this->SetText( theIndex, theProps.Text );
  this->SetIsVisible( theIndex, theProps.IsVisible );
  this->SetIsScreenFixed( theIndex, theProps.IsScreenFixed );
  this->SetShapeSel( theIndex, theProps.ShapeType, theProps.ShapeIndex );
  this->SetAttach( theIndex, theProps.Attach.Transformed( aToShapeLCS ) );
  this->SetPosition( theIndex, (theProps.IsScreenFixed) ? 
    theProps.Position : theProps.Position.Transformed( aToShapeLCS ) );
}

//=================================================================================
// function : GetProperties
// purpose  : 
//=================================================================================
void GEOMGUI_AnnotationAttrs::GetProperties( const int theIndex, Properties& theProps ) const
{
  theProps.Text = this->GetText( theIndex );
  theProps.IsVisible = this->GetIsVisible( theIndex );
  theProps.IsScreenFixed = this->GetIsScreenFixed( theIndex );
  theProps.Position = this->GetPosition( theIndex );
  theProps.Attach = this->GetAttach( theIndex );

  this->GetShapeSel( theIndex, theProps.ShapeType, theProps.ShapeIndex );
}

//=================================================================================
// function : SetupPresentation
// purpose  : 
//=================================================================================
void GEOMGUI_AnnotationAttrs::SetupPresentation( const Handle(GEOM_Annotation)& thePresentation,
                                                 const Properties& theProps,
                                                 const gp_Ax3& theShapeLCS )
{
  gp_Trsf aFromShapeLCS;
  aFromShapeLCS.SetTransformation( theShapeLCS, gp_Ax3() );

  TCollection_ExtendedString aText;
  for (int i = 0; i < (int)theProps.Text.length(); i++ )
    aText.Insert( i + 1, theProps.Text[ i ].unicode() );

  thePresentation->SetText( aText );
  thePresentation->SetIsScreenFixed( theProps.IsScreenFixed );
  thePresentation->SetAttachPoint( theProps.Attach.Transformed( aFromShapeLCS ) );
  thePresentation->SetPosition( (theProps.IsScreenFixed) ? 
    theProps.Position : theProps.Position.Transformed( aFromShapeLCS ) );
}

//=================================================================================
// function : SetupPresentation
// purpose  : 
//=================================================================================
void GEOMGUI_AnnotationAttrs::SetupPresentation( const Handle(GEOM_Annotation)& thePresentation,
                                                 const int theIndex,
                                                 const gp_Ax3& theShapeLCS )
{
  Properties aProps;
  this->GetProperties( theIndex, aProps );
  this->SetupPresentation( thePresentation, aProps, theShapeLCS );
}
