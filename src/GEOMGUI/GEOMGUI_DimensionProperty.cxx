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

// File   : GEOMGUI_DimensionProperty.cxx
// Author : Anton POLETAEV, Open CASCADE S.A.S.
//

#include "GEOMGUI_DimensionProperty.h"

// OCCT includes
#include <Standard_ProgramError.hxx>
#include <gp_Trsf.hxx>

#include <SalomeApp_Study.h>

// Static patterns for casting value-to-string & value-from-string. The patterns are:
//  ITEM: { name[string] : visibility : type : values[composite] };
//  PLANE: a[float] : b[float] : c[float] : d[float]
//  PROPS: flyout[float] : text h pos[int] : text v pos[int] : arrow pos[int]
//  XYZ: x [float] : y[float] : z[float]
//  FLOAT: value [float]
namespace
{
  static const QString PATTERN_ITEM_GROUP = "\\{ (Name=(?::{2,}|.)*:(?!:)Visible=.*:Type=.*:.*) \\}";
  static const QString PATTERN_ITEM  = "Name=((?::{2,}|.)*):(?!:)Visible=(\\d{1}):Type=(\\d{1}):(.*)";
  static const QString PATTERN_PLANE = "Plane=\\{(.*):(.*):(.*):(.*)\\}";
  static const QString PATTERN_PROPS = "Flyout=(.*):TextH=(.*):TextV=(.*):Arrow=(.*)";
  static const QString PATTERN_XYZ   = "%1=\\{(.*):(.*):(.*)\\}";
  static const QString PATTERN_FLOAT = "%1=(.*)";

  static const QString PATTERN_LENGTH =
    PATTERN_PLANE + ":" +
    PATTERN_PROPS + ":" +
    PATTERN_XYZ.arg( "Point1" ) + ":" +
    PATTERN_XYZ.arg( "Point2" );

  static const QString PATTERN_DIAMETER =
    PATTERN_PLANE + ":" +
    PATTERN_PROPS + ":" +
    PATTERN_XYZ.arg( "Position" ) + ":" +
    PATTERN_XYZ.arg( "NDir" ) + ":" +
    PATTERN_XYZ.arg( "XDir" ) + ":" +
    PATTERN_FLOAT.arg( "Radius" );

  static const QString PATTERN_ANGLE =
    PATTERN_PROPS + ":" +
    PATTERN_XYZ.arg( "Point1" ) + ":" +
    PATTERN_XYZ.arg( "Point2" ) + ":" +
    PATTERN_XYZ.arg( "Point3" );
};

//=================================================================================
// function : Length::Init
// purpose  : 
//=================================================================================
void GEOMGUI_DimensionProperty::Length::Init( const Handle(AIS_LengthDimension)& theIO, const gp_Ax3& theLCS )
{
  gp_Trsf aFromLCS;
  aFromLCS.SetTransformation( gp_Ax3(), theLCS );

  FirstPoint  = theIO->FirstPoint().Transformed( aFromLCS );
  SecondPoint = theIO->SecondPoint().Transformed( aFromLCS );
  Plane       = theIO->GetPlane().Transformed( aFromLCS );
  Flyout      = theIO->GetFlyout();
  TextHPos    = theIO->DimensionAspect()->TextHorizontalPosition();
  TextVPos    = theIO->DimensionAspect()->TextVerticalPosition();
  ArrowPos    = theIO->DimensionAspect()->ArrowOrientation();
}

//=================================================================================
// function : Length::Update
// purpose  : 
//=================================================================================
void GEOMGUI_DimensionProperty::Length::Update( Handle(AIS_LengthDimension)& theIO, const gp_Ax3& theLCS )
{
  gp_Trsf aToLCS;
  aToLCS.SetTransformation( theLCS, gp_Ax3() );

  gp_Pnt aPoint1 = FirstPoint.Transformed( aToLCS );
  gp_Pnt aPoint2 = SecondPoint.Transformed( aToLCS );
  gp_Pln aPlane  = Plane.Transformed( aToLCS );

  theIO->SetMeasuredGeometry( aPoint1, aPoint2, aPlane );
  theIO->SetFlyout( Flyout );

  Handle(Prs3d_DimensionAspect) aStyle = new Prs3d_DimensionAspect();
  aStyle->SetTextHorizontalPosition( TextHPos );
  aStyle->SetTextVerticalPosition( TextVPos );
  aStyle->SetArrowOrientation( ArrowPos );
  theIO->SetDimensionAspect( aStyle );
}

//=================================================================================
// function : Length::ToValues
// purpose  : 
//=================================================================================
void GEOMGUI_DimensionProperty::Length::ToValues(std::vector<double>& theValues) const
{
  // custom plane [2,3,4,5]
  Standard_Real A, B, C, D;
  Plane.Coefficients( A, B, C, D );
  theValues.push_back( (double) A );
  theValues.push_back( (double) B );
  theValues.push_back( (double) C );
  theValues.push_back( (double) D );

  // flyout size [6]
  theValues.push_back( (double) Flyout );

  // text flags [7,8]
  theValues.push_back( (double) TextHPos );
  theValues.push_back( (double) TextVPos );

  // arrow flags [9]
  theValues.push_back( (double) ArrowPos );

  // point 1 [10,11,12]
  theValues.push_back( (double) FirstPoint.X() );
  theValues.push_back( (double) FirstPoint.Y() );
  theValues.push_back( (double) FirstPoint.Z() );

  // point 2 [13,14,15]
  theValues.push_back( (double) SecondPoint.X() );
  theValues.push_back( (double) SecondPoint.Y() );
  theValues.push_back( (double) SecondPoint.Z() );
}

//=================================================================================
// function : Length::FromValues
// purpose  : 
//=================================================================================
void GEOMGUI_DimensionProperty::Length::FromValues(int& theIt, const std::vector<double>& theValues)
{
  // custom plane [2,3,4,5]
  Standard_Real A = (Standard_Real) theValues[theIt++];
  Standard_Real B = (Standard_Real) theValues[theIt++];
  Standard_Real C = (Standard_Real) theValues[theIt++];
  Standard_Real D = (Standard_Real) theValues[theIt++];
  Plane = gp_Pln( A, B, C, D );

  // flyout size [6]
  Flyout = (Standard_Real) theValues[theIt++];

  // text flags [7,8]
  TextHPos = (Prs3d_DimensionTextHorizontalPosition)(int)theValues[theIt++];
  TextVPos = (Prs3d_DimensionTextVerticalPosition)  (int)theValues[theIt++];

  // arrow flags [9]
  ArrowPos = (Prs3d_DimensionArrowOrientation) (int)theValues[theIt++];

  // point 1 [10,11,12]
  Standard_Real aFirstX = theValues[theIt++];
  Standard_Real aFirstY = theValues[theIt++];
  Standard_Real aFirstZ = theValues[theIt++];
  FirstPoint = gp_Pnt( aFirstX, aFirstY, aFirstZ );

  // point 2 [13,14,15]
  Standard_Real aSecondX = theValues[theIt++];
  Standard_Real aSecondY = theValues[theIt++];
  Standard_Real aSecondZ = theValues[theIt++];
  SecondPoint = gp_Pnt( aSecondX, aSecondY, aSecondZ );
}

//=================================================================================
// function : Length::operator == 
// purpose  : 
//=================================================================================
bool GEOMGUI_DimensionProperty::Length::operator == (const Length& theOther) const
{
  if ( FirstPoint.X()  != theOther.FirstPoint.X()
    || FirstPoint.Y()  != theOther.FirstPoint.Y()
    || FirstPoint.Z()  != theOther.FirstPoint.Z()
    || SecondPoint.X() != theOther.SecondPoint.X()
    || SecondPoint.Y() != theOther.SecondPoint.Y()
    || SecondPoint.Z() != theOther.SecondPoint.Z() )
  {
    return false;
  }

  if ( Plane.Location().X() != theOther.Plane.Location().X()
    || Plane.Location().Y() != theOther.Plane.Location().Y()
    || Plane.Location().Z() != theOther.Plane.Location().Z()
    || Plane.Axis().Direction().X() != theOther.Plane.Axis().Direction().X()
    || Plane.Axis().Direction().Y() != theOther.Plane.Axis().Direction().Y()
    || Plane.Axis().Direction().Z() != theOther.Plane.Axis().Direction().Z() )
  {
    return false;
  }

  if ( Flyout   != theOther.Flyout 
    || TextHPos != theOther.TextHPos 
    || TextVPos != theOther.TextVPos
    || ArrowPos != theOther.ArrowPos )
  {
    return false;
  }

  return true;
}

//=================================================================================
// function : Diameter::Init
// purpose  : 
//=================================================================================
void GEOMGUI_DimensionProperty::Diameter::Init( const Handle(AIS_DiameterDimension)& theIO, const gp_Ax3& theLCS )
{
  gp_Trsf aFromLCS;
  aFromLCS.SetTransformation( gp_Ax3(), theLCS );

  Circle   = theIO->Circle().Transformed( aFromLCS );
  Plane    = theIO->GetPlane().Transformed( aFromLCS );
  Flyout   = theIO->GetFlyout();
  TextHPos = theIO->DimensionAspect()->TextHorizontalPosition();
  TextVPos = theIO->DimensionAspect()->TextVerticalPosition();
  ArrowPos = theIO->DimensionAspect()->ArrowOrientation();
}

//=================================================================================
// function : Diameter::Update
// purpose  : 
//=================================================================================
void GEOMGUI_DimensionProperty::Diameter::Update( Handle(AIS_DiameterDimension)& theIO, const gp_Ax3& theLCS )
{
  gp_Trsf aToLCS;
  aToLCS.SetTransformation( theLCS, gp_Ax3() );

  gp_Circ aCircle = Circle.Transformed( aToLCS );
  gp_Pln  aPlane  = Plane.Transformed( aToLCS );

  Standard_Boolean isParallel = 
    aCircle.Axis().Direction().IsParallel( aPlane.Axis().Direction(), Precision::Angular() );

  if ( isParallel )
  {
    theIO->UnsetCustomPlane();
    theIO->SetMeasuredGeometry( aCircle );
  }
  else
  {
    theIO->SetCustomPlane( aPlane );
    theIO->SetMeasuredGeometry( aCircle );
  }

  theIO->SetFlyout( Flyout );

  Handle(Prs3d_DimensionAspect) aStyle = new Prs3d_DimensionAspect();
  aStyle->SetTextHorizontalPosition( TextHPos );
  aStyle->SetTextVerticalPosition( TextVPos );
  aStyle->SetArrowOrientation( ArrowPos );
  theIO->SetDimensionAspect( aStyle );
}

//=================================================================================
// function : Diameter::ToValues
// purpose  : 
//=================================================================================
void GEOMGUI_DimensionProperty::Diameter::ToValues(std::vector<double>& theValues) const
{
  // custom plane [2,3,4,5]
  Standard_Real A, B, C, D;
  Plane.Coefficients( A, B, C, D );
  theValues.push_back( (double) A );
  theValues.push_back( (double) B );
  theValues.push_back( (double) C );
  theValues.push_back( (double) D );

  // flyout size [6]
  theValues.push_back( (double) Flyout );

  // text flags [7,8]
  theValues.push_back( (double) TextHPos );
  theValues.push_back( (double) TextVPos );

  // arrow flags [9]
  theValues.push_back( (double) ArrowPos );

  // circle location [10,11,12]
  theValues.push_back( (double) Circle.Location().X() );
  theValues.push_back( (double) Circle.Location().Y() );
  theValues.push_back( (double) Circle.Location().Z() );

  // circle normal [13,14,15]
  theValues.push_back( (double) Circle.Axis().Direction().X() );
  theValues.push_back( (double) Circle.Axis().Direction().Y() );
  theValues.push_back( (double) Circle.Axis().Direction().Z() );

  // x-direction [16,17,18]
  theValues.push_back( (double) Circle.XAxis().Direction().X() );
  theValues.push_back( (double) Circle.XAxis().Direction().Y() );
  theValues.push_back( (double) Circle.XAxis().Direction().Z() );

  // radius [19]
  theValues.push_back( (double) Circle.Radius() );
}

//=================================================================================
// function : Diameter::FromValues
// purpose  : 
//=================================================================================
void GEOMGUI_DimensionProperty::Diameter::FromValues(int& theIt, const std::vector<double>& theValues)
{
  // custom plane [2,3,4,5]
  Standard_Real A = (Standard_Real) theValues[theIt++];
  Standard_Real B = (Standard_Real) theValues[theIt++];
  Standard_Real C = (Standard_Real) theValues[theIt++];
  Standard_Real D = (Standard_Real) theValues[theIt++];
  Plane = gp_Pln( A, B, C, D );

  // flyout size [6]
  Flyout = (Standard_Real) theValues[theIt++];

  // text flags [7,8]
  TextHPos = (Prs3d_DimensionTextHorizontalPosition)(int)theValues[theIt++];
  TextVPos = (Prs3d_DimensionTextVerticalPosition)  (int)theValues[theIt++];

  // arrow flags [9]
  ArrowPos = (Prs3d_DimensionArrowOrientation) (int)theValues[theIt++];

  // circle location [10,11,12]
  Standard_Real aLocX = (Standard_Real) theValues[theIt++];
  Standard_Real aLocY = (Standard_Real) theValues[theIt++];
  Standard_Real aLocZ = (Standard_Real) theValues[theIt++];

  // circle normal [13,14,15]
  Standard_Real aNormX = (Standard_Real) theValues[theIt++];
  Standard_Real aNormY = (Standard_Real) theValues[theIt++];
  Standard_Real aNormZ = (Standard_Real) theValues[theIt++];

  // x-direction [16,17,18]
  Standard_Real aXDirX = (Standard_Real) theValues[theIt++];
  Standard_Real aXDirY = (Standard_Real) theValues[theIt++];
  Standard_Real aXDirZ = (Standard_Real) theValues[theIt++];

  // radius [19]
  Standard_Real aRadius = (Standard_Real) theValues[theIt++];

  gp_Ax2 anAx( gp_Pnt( aLocX, aLocY, aLocZ ),
               gp_Dir( aNormX, aNormY, aNormZ ),
               gp_Dir( aXDirX, aXDirY, aXDirZ ) );

  Circle = gp_Circ( anAx, aRadius );
}

//=================================================================================
// function : Diameter::operator == 
// purpose  : 
//=================================================================================
bool GEOMGUI_DimensionProperty::Diameter::operator == (const Diameter& theOther) const
{
  if ( Circle.Location().X()  != theOther.Circle.Location().X()
    || Circle.Location().Y()  != theOther.Circle.Location().Y()
    || Circle.Location().Z()  != theOther.Circle.Location().Z()
    || Circle.Axis().Direction().X() != theOther.Circle.Axis().Direction().X()
    || Circle.Axis().Direction().Y() != theOther.Circle.Axis().Direction().Y()
    || Circle.Axis().Direction().Z() != theOther.Circle.Axis().Direction().Z()
    || Circle.XAxis().Direction().X() != theOther.Circle.XAxis().Direction().X()
    || Circle.XAxis().Direction().Y() != theOther.Circle.XAxis().Direction().Y()
    || Circle.XAxis().Direction().Z() != theOther.Circle.XAxis().Direction().Z() 
    || Circle.Radius() != theOther.Circle.Radius() )
  {
    return false;
  }

  if ( Plane.Location().X() != theOther.Plane.Location().X()
    || Plane.Location().Y() != theOther.Plane.Location().Y()
    || Plane.Location().Z() != theOther.Plane.Location().Z()
    || Plane.Axis().Direction().X() != theOther.Plane.Axis().Direction().X()
    || Plane.Axis().Direction().Y() != theOther.Plane.Axis().Direction().Y()
    || Plane.Axis().Direction().Z() != theOther.Plane.Axis().Direction().Z() )
  {
    return false;
  }

  if ( Flyout   != theOther.Flyout 
    || TextHPos != theOther.TextHPos 
    || TextVPos != theOther.TextVPos 
    || ArrowPos != theOther.ArrowPos )
  {
    return false;
  }

  return true;
}

//=================================================================================
// function : Angle::Init
// purpose  : 
//=================================================================================
void GEOMGUI_DimensionProperty::Angle::Init( const Handle(AIS_AngleDimension)& theIO, const gp_Ax3& theLCS )
{
  gp_Trsf aFromLCS;
  aFromLCS.SetTransformation( gp_Ax3(), theLCS );

  FirstPoint  = theIO->FirstPoint().Transformed( aFromLCS );
  SecondPoint = theIO->SecondPoint().Transformed( aFromLCS );
  CenterPoint = theIO->CenterPoint().Transformed( aFromLCS );
  Flyout      = theIO->GetFlyout();
  TextHPos    = theIO->DimensionAspect()->TextHorizontalPosition();
  TextVPos    = theIO->DimensionAspect()->TextVerticalPosition();
  ArrowPos    = theIO->DimensionAspect()->ArrowOrientation();
}

//=================================================================================
// function : Angle::Update
// purpose  : 
//=================================================================================
void GEOMGUI_DimensionProperty::Angle::Update( Handle(AIS_AngleDimension)& theIO, const gp_Ax3& theLCS )
{
  gp_Trsf aToLCS;
  aToLCS.SetTransformation( theLCS, gp_Ax3() );

  gp_Pnt aPoint1 = FirstPoint.Transformed( aToLCS );
  gp_Pnt aPoint2 = CenterPoint.Transformed( aToLCS );
  gp_Pnt aPoint3 = SecondPoint.Transformed( aToLCS );

  theIO->SetMeasuredGeometry( aPoint1, aPoint2, aPoint3 );
  theIO->SetFlyout( Flyout );

  Handle(Prs3d_DimensionAspect) aStyle = new Prs3d_DimensionAspect();
  aStyle->SetTextHorizontalPosition( TextHPos );
  aStyle->SetTextVerticalPosition( TextVPos );
  aStyle->SetArrowOrientation( ArrowPos );
  theIO->SetDimensionAspect( aStyle );
}

//=================================================================================
// function : Angle::ToValues
// purpose  : 
//=================================================================================
void GEOMGUI_DimensionProperty::Angle::ToValues(std::vector<double>& theValues) const
{
  // flyout [2]
  theValues.push_back( (double) Flyout );

  // text flags [3,4]
  theValues.push_back( (double) TextHPos );
  theValues.push_back( (double) TextVPos );

  // arrow flags [5]
  theValues.push_back( (double) ArrowPos );

  // point 1 [6,7,8]
  theValues.push_back( (double) FirstPoint.X() );
  theValues.push_back( (double) FirstPoint.Y() );
  theValues.push_back( (double) FirstPoint.Z() );

  // point 2 [9,10,11]
  theValues.push_back( (double) SecondPoint.X() );
  theValues.push_back( (double) SecondPoint.Y() );
  theValues.push_back( (double) SecondPoint.Z() );

  // center [12,13,14]
  theValues.push_back( (double) CenterPoint.X() );
  theValues.push_back( (double) CenterPoint.Y() );
  theValues.push_back( (double) CenterPoint.Z() );
}

//=================================================================================
// function : Angle::FromValues
// purpose  : 
//=================================================================================
void GEOMGUI_DimensionProperty::Angle::FromValues(int& theIt, const std::vector<double>& theValues)
{
  // flyout [2]
  Flyout = (Standard_Real) theValues[theIt++];

  // text flags [3,4]
  TextHPos = (Prs3d_DimensionTextHorizontalPosition)(int)theValues[theIt++];
  TextVPos = (Prs3d_DimensionTextVerticalPosition)  (int)theValues[theIt++];

  // arrow flags [5]
  ArrowPos = (Prs3d_DimensionArrowOrientation) (int)theValues[theIt++];

  // point 1 [6,7,8]
  Standard_Real aFirstX = (Standard_Real) theValues[theIt++];
  Standard_Real aFirstY = (Standard_Real) theValues[theIt++];
  Standard_Real aFirstZ = (Standard_Real) theValues[theIt++];

  // point 2 [9,10,11]
  Standard_Real aSecondX = (Standard_Real) theValues[theIt++];
  Standard_Real aSecondY = (Standard_Real) theValues[theIt++];
  Standard_Real aSecondZ = (Standard_Real) theValues[theIt++];

  // center [12,13,14]
  Standard_Real aCenterX = (Standard_Real) theValues[theIt++];
  Standard_Real aCenterY = (Standard_Real) theValues[theIt++];
  Standard_Real aCenterZ = (Standard_Real) theValues[theIt++];

  FirstPoint  = gp_Pnt( aFirstX, aFirstY, aFirstZ );
  SecondPoint = gp_Pnt( aSecondX, aSecondY, aSecondZ );
  CenterPoint = gp_Pnt( aCenterX, aCenterY, aCenterZ );
}

//=================================================================================
// function : Angle::operator == 
// purpose  : 
//=================================================================================
bool GEOMGUI_DimensionProperty::Angle::operator == (const Angle& theOther) const
{
  if ( FirstPoint.X()  != theOther.FirstPoint.X()
    || FirstPoint.Y()  != theOther.FirstPoint.Y()
    || FirstPoint.Z()  != theOther.FirstPoint.Z()
    || SecondPoint.X() != theOther.SecondPoint.X()
    || SecondPoint.Y() != theOther.SecondPoint.Y()
    || SecondPoint.Z() != theOther.SecondPoint.Z()
    || CenterPoint.X() != theOther.CenterPoint.X()
    || CenterPoint.Y() != theOther.CenterPoint.Y()
    || CenterPoint.Z() != theOther.CenterPoint.Z() )
  {
    return false;
  }

  if ( Flyout   != theOther.Flyout 
    || TextHPos != theOther.TextHPos 
    || TextVPos != theOther.TextVPos 
    || ArrowPos != theOther.ArrowPos )
  {
    return false;
  }

  return true;
}

//=================================================================================
// function : Constructor
// purpose  : 
//=================================================================================
GEOMGUI_DimensionProperty::GEOMGUI_DimensionProperty()
{
}

//=================================================================================
// function : Copy constructor
// purpose  : 
//=================================================================================
GEOMGUI_DimensionProperty::GEOMGUI_DimensionProperty( const GEOMGUI_DimensionProperty& theOther )
{
  const VectorOfVisibility& aOtherVis   = theOther.myVisibility;
  const VectorOfNames& aOtherNames      = theOther.myNames;
  const VectorOfRecords& aOtherRecords  = theOther.myRecords;

  VectorOfVisibility::const_iterator aVisIt = aOtherVis.constBegin();
  VectorOfNames::const_iterator   aNamesIt  = aOtherNames.constBegin();
  VectorOfRecords::const_iterator aRecordIt = aOtherRecords.constBegin();
  for ( ; aRecordIt != aOtherRecords.constEnd(); ++aVisIt, ++aNamesIt, ++aRecordIt )
  {
    RecordPtr aNewRecord;
    const RecordPtr& aRecord = *aRecordIt;
    switch( aRecord->Type() )
    {
      case DimensionType_Length :
        aNewRecord = RecordPtr( new Length( *aRecord->AsLength() ) );
        break;

      case DimensionType_Diameter :
        aNewRecord = RecordPtr( new Diameter( *aRecord->AsDiameter() ) );
        break;

      case DimensionType_Angle :
        aNewRecord = RecordPtr( new Angle( *aRecord->AsAngle() ) );
        break;
    }

    myVisibility.append( *aVisIt );
    myNames.append( *aNamesIt );
    myRecords.append( aNewRecord );
  }
}

//=================================================================================
// function : Init constructor
// purpose  : 
//=================================================================================
GEOMGUI_DimensionProperty::GEOMGUI_DimensionProperty( SalomeApp_Study* theStudy, const std::string& theEntry )
{
  LoadFromAttribute( theStudy, theEntry );
}

//=================================================================================
// function : Init constructor
// purpose  : 
//=================================================================================
GEOMGUI_DimensionProperty::GEOMGUI_DimensionProperty( const QString& theProperty )
{
  QRegExp aRegExpItemGroups( PATTERN_ITEM_GROUP );
  QRegExp aRegExpItem( "^" + PATTERN_ITEM + "$" );
  aRegExpItemGroups.setMinimal( true );
  aRegExpItem.setMinimal( true );

  int aPos = 0;
  while ( ( aPos = aRegExpItemGroups.indexIn( theProperty, aPos ) ) != -1 )
  {
    aPos += aRegExpItemGroups.matchedLength();

    QString aStrItem = aRegExpItemGroups.cap(1);

    if ( aRegExpItem.indexIn( aStrItem ) < 0 )
    {
      continue;
    }

    // extract name
    QString aStrName    = aRegExpItem.cap( 1 );
    QString aStrVisible = aRegExpItem.cap( 2 );
    QString aStrType    = aRegExpItem.cap( 3 );
    QString aStrValues  = aRegExpItem.cap( 4 );

    // extract values
    aStrName.replace( "::", ":" );
    bool isVisible = aStrVisible.toInt() != 0;
    int aType = aStrType.toInt();
    
    RecordPtr aRecord;
    switch ( aType )
    {
      case DimensionType_Length   : aRecord = RecordPtr( new Length ); break;
      case DimensionType_Diameter : aRecord = RecordPtr( new Diameter ); break;
      case DimensionType_Angle    : aRecord = RecordPtr( new Angle ); break;
      default:
        continue;
    }
    
    QRegExp aRegExpValues;
    switch ( aType )
    {
      case DimensionType_Length   : aRegExpValues = QRegExp( "^" + PATTERN_LENGTH + "$" ); break;
      case DimensionType_Diameter : aRegExpValues = QRegExp( "^" + PATTERN_DIAMETER + "$" ); break;
      case DimensionType_Angle    : aRegExpValues = QRegExp( "^" + PATTERN_ANGLE + "$" ); break;
    }

    aRegExpValues.setMinimal(true);

    if ( aRegExpValues.indexIn( aStrValues ) < 0 )
    {
      continue;
    }

    std::vector<double> aValues;

    QStringList aStrListOfValues = aRegExpValues.capturedTexts();
    QStringList::Iterator aStrListOfValuesIt = aStrListOfValues.begin();
    ++aStrListOfValuesIt; // skip first capture
    for ( ; aStrListOfValuesIt != aStrListOfValues.end(); ++aStrListOfValuesIt )
    {
      aValues.push_back( (*aStrListOfValuesIt).toDouble() );
    }

    int aValueIt = 0;

    aRecord->FromValues( aValueIt, aValues );

    myVisibility.append( isVisible );
    myNames.append( aStrName );
    myRecords.append( aRecord );
  }
}

//=================================================================================
// function : Destructor
// purpose  : 
//=================================================================================
GEOMGUI_DimensionProperty::~GEOMGUI_DimensionProperty()
{
}

//=================================================================================
// function : operator QVariant()
// purpose  : 
//=================================================================================
GEOMGUI_DimensionProperty::operator QVariant() const
{
  QVariant aQVariant;
  aQVariant.setValue( *this );
  return aQVariant;
}

//=================================================================================
// function : operator QString()
// purpose  : 
//=================================================================================
GEOMGUI_DimensionProperty::operator QString() const
{
  QStringList anItems;

  VectorOfVisibility::ConstIterator aVisibilityIt = myVisibility.constBegin();
  VectorOfRecords::ConstIterator aRecordIt        = myRecords.constBegin();
  VectorOfNames::ConstIterator aNameIt            = myNames.constBegin();
  for ( ; aRecordIt != myRecords.constEnd(); ++aRecordIt, ++aNameIt, ++aVisibilityIt )
  {
    QString aName            = *aNameIt;
    const bool& isVisible    = *aVisibilityIt;
    const RecordPtr& aRecord = *aRecordIt;

    // pack values
    std::vector<double> aPacked;
    aRecord->ToValues( aPacked );

    // put values into pattern
    QString aStringValues;
    switch ( aRecord->Type() )
    {
      case DimensionType_Length   : aStringValues = PATTERN_LENGTH; break;
      case DimensionType_Diameter : aStringValues = PATTERN_DIAMETER; break;
      case DimensionType_Angle    : aStringValues = PATTERN_ANGLE; break;
      default:
        continue;
    }

    aStringValues.remove("\\");

    int it = 0;
    for ( ; it < aPacked.size(); ++it )
    {
      int aNextPos = aStringValues.indexOf("(.*)");
      if ( aNextPos < 0 )
      {
        break; // invalid pattern
      }

      aStringValues.replace( aNextPos, 4, QString::number( aPacked.at(it) ) );
    }

    if ( it < aPacked.size() )
    {
      continue; // invalid pattern
    }

    // replace all ':' to '::' for pattern matching
    aName.replace(":", "::");

    anItems.append( 
      QString("{ Name=") + aName +
      QString(":") + QString("Visible=") + QString::number( isVisible ? 1 : 0 ) +
      QString(":") + QString("Type=") + QString::number( (int) aRecord->Type() ) +
      QString(":") + aStringValues + QString(" }") );
  }

  return anItems.join( ":" );
}

//=================================================================================
// function : operator ==
// purpose  : 
//=================================================================================
bool GEOMGUI_DimensionProperty::operator == (const GEOMGUI_DimensionProperty& theOther) const
{
  if ( myVisibility.size() != theOther.myVisibility.size()
    || myNames.size() != theOther.myNames.size()
    || myRecords.size() != theOther.myRecords.size() )
  {
    return false;
  }

  for ( int it = 0; it < myRecords.size(); ++it )
  {
    if ( myVisibility[it] != theOther.myVisibility[it] )
    {
      return false;
    }

    if ( myNames[it] != theOther.myNames[it] )
    {
      return false;
    }

    const RecordPtr& aRecord = myRecords[it];
    const RecordPtr& aOtherRecord = theOther.myRecords[it];
    if ( aRecord->Type() != aOtherRecord->Type() )
    {
      return false;
    }

    switch ( aRecord->Type() )
    {
      case DimensionType_Length:
        if ( (*aRecord->AsLength()) != (*aOtherRecord->AsLength()) )
        {
          return false;
        }
        break;

      case DimensionType_Diameter:
        if ( (*aRecord->AsDiameter()) != (*aOtherRecord->AsDiameter()) )
        {
          return false;
        }
        break;

      case DimensionType_Angle:
        if ( (*aRecord->AsAngle()) != (*aOtherRecord->AsAngle()) )
        {
          return false;
        }
        break;
    }
  }

  return true;
}

//=================================================================================
// function : GetNumber
// purpose  : 
//=================================================================================
int GEOMGUI_DimensionProperty::GetNumber() const
{
  return myRecords.size();
}

//=================================================================================
// function : AddRecord
// purpose  : 
//=================================================================================
void GEOMGUI_DimensionProperty::AddRecord( const Handle(AIS_Dimension)& theIO, const gp_Ax3& theLCS )
{
  RecordPtr aNewRecord;

  int aType = TypeFromIO( theIO );

  switch ( aType )
  {
    case DimensionType_Length :
    {
      Handle(AIS_LengthDimension) aLength = 
        Handle(AIS_LengthDimension)::DownCast( theIO );

      aNewRecord = RecordPtr( new Length() );
      aNewRecord->AsLength()->Init( aLength, theLCS );
      break;
    }

    case DimensionType_Diameter :
    {
      Handle(AIS_DiameterDimension) aDiam =
        Handle(AIS_DiameterDimension)::DownCast( theIO );

      aNewRecord = RecordPtr( new Diameter() );
      aNewRecord->AsDiameter()->Init( aDiam, theLCS );
      break;
    }

    case DimensionType_Angle :
    {
      Handle(AIS_AngleDimension) anAngle =
        Handle(AIS_AngleDimension)::DownCast( theIO );

      aNewRecord = RecordPtr( new Angle() );
      aNewRecord->AsAngle()->Init( anAngle, theLCS );
    }
  }

  myVisibility.append( true );
  myNames.append( QString() );
  myRecords.append( aNewRecord );
}

//=================================================================================
// function : AddRecord
// purpose  : 
//=================================================================================
void GEOMGUI_DimensionProperty::AddRecord( const RecordPtr& theRecord )
{
  myVisibility.append( true );
  myNames.append( QString() );
  myRecords.append( theRecord );
}

//=================================================================================
// function : RemoveRecord
// purpose  : 
//=================================================================================
void GEOMGUI_DimensionProperty::RemoveRecord( const int theIndex )
{
  myNames.remove( theIndex );
  myVisibility.remove( theIndex );
  myRecords.remove( theIndex );
}

//=================================================================================
// function : Clear
// purpose  : 
//=================================================================================
void GEOMGUI_DimensionProperty::Clear()
{
  myNames.clear();
  myVisibility.clear();
  myRecords.clear();
}

//=================================================================================
// function : SetRecord
// purpose  : 
//=================================================================================
void GEOMGUI_DimensionProperty::SetRecord( const int theIndex,
                                           const Handle(AIS_Dimension)& theIO,
                                           const gp_Ax3& theLCS )
{
  int aType = TypeFromIO( theIO );

  RecordPtr& aChangeRecord = myRecords[theIndex];

  switch ( aType )
  {
    case DimensionType_Length :
    {
      Handle(AIS_LengthDimension) aLength = 
        Handle(AIS_LengthDimension)::DownCast( theIO );

      aChangeRecord = RecordPtr( new Length() );
      aChangeRecord->AsLength()->Init( aLength, theLCS );
      break;
    }

    case DimensionType_Diameter :
    {
      Handle(AIS_DiameterDimension) aDiam =
        Handle(AIS_DiameterDimension)::DownCast( theIO );

      aChangeRecord = RecordPtr( new Diameter() );
      aChangeRecord->AsDiameter()->Init( aDiam, theLCS );
      break;
    }

    case DimensionType_Angle :
    {
      Handle(AIS_AngleDimension) anAngle =
        Handle(AIS_AngleDimension)::DownCast( theIO );

      aChangeRecord = RecordPtr( new Angle() );
      aChangeRecord->AsAngle()->Init( anAngle, theLCS );
    }
  }
}

//=================================================================================
// function : SetRecord
// purpose  : 
//=================================================================================
void GEOMGUI_DimensionProperty::SetRecord( const int theIndex, const RecordPtr& theRecord )
{
  myRecords[theIndex] = theRecord;
}

//=================================================================================
// function : GetRecord
// purpose  : 
//=================================================================================
const GEOMGUI_DimensionProperty::RecordPtr& GEOMGUI_DimensionProperty::GetRecord( const int theIndex ) const
{
  return myRecords[theIndex];
}

//=================================================================================
// function : IsVisible
// purpose  : 
//=================================================================================
bool GEOMGUI_DimensionProperty::IsVisible( const int theIndex ) const
{
  return myVisibility[theIndex];
}

//=================================================================================
// function : SetVisible
// purpose  : 
//=================================================================================
void GEOMGUI_DimensionProperty::SetVisible( const int theIndex, const bool theIsVisible )
{
  myVisibility[theIndex] = theIsVisible;
}

//=================================================================================
// function : GetName
// purpose  : 
//=================================================================================
QString GEOMGUI_DimensionProperty::GetName( const int theIndex ) const
{
  return myNames[theIndex];
}

//=================================================================================
// function : SetName
// purpose  : 
//=================================================================================
void GEOMGUI_DimensionProperty::SetName( const int theIndex, const QString &theName )
{
  myNames[theIndex] = theName;
}

//=================================================================================
// function : GetType
// purpose  : 
//=================================================================================
int GEOMGUI_DimensionProperty::GetType( const int theIndex ) const
{
  return myRecords[theIndex]->Type();
}

//=================================================================================
// function : LoadFromAttribute
// purpose  : 
//=================================================================================
void GEOMGUI_DimensionProperty::LoadFromAttribute( SalomeApp_Study* theStudy,
                                                   const std::string& theEntry )
{
  Clear();

  _PTR(SObject) aSObj = theStudy->studyDS()->FindObjectID( theEntry );
  if ( !aSObj )
  {
    return;
  }

  _PTR(StudyBuilder) aBuilder = theStudy->studyDS()->NewBuilder();

  _PTR(GenericAttribute) aSeekAtt;
  _PTR(AttributeTableOfReal) aRecordsAtt;

  if ( !aSObj->FindAttribute( aSeekAtt, "AttributeTableOfReal" ) )
  {
    return;
  }

  aRecordsAtt = aSeekAtt;

  for ( int aRecordIt = 1; aRecordIt <= aRecordsAtt->GetNbColumns(); ++aRecordIt )
  {
    std::vector<double> aPacked = aRecordsAtt->GetColumn( aRecordIt );

    RecordPtr aRecord;

    QString aName( aRecordsAtt->GetColumnTitle( aRecordIt ).c_str() );

    // unpack records
    int it = 0;

    // visibility [0]
    bool isVisible = (bool) aPacked[it++];

    // type [1]
    int aType = (int) aPacked[it++];

    switch (aType)
    {
      case DimensionType_Length   : aRecord = RecordPtr( new Length ); break;
      case DimensionType_Diameter : aRecord = RecordPtr( new Diameter ); break;
      case DimensionType_Angle    : aRecord = RecordPtr( new Angle ); break;
    }
    aRecord->FromValues(it, aPacked);

    myVisibility.append( isVisible );
    myNames.append( aName );
    myRecords.append( aRecord );
  }
}

//=================================================================================
// function : SaveToAttribute
// purpose  : 
//=================================================================================
void GEOMGUI_DimensionProperty::SaveToAttribute( SalomeApp_Study *theStudy,
                                                 const std::string &theEntry )
{
  _PTR(SObject) aSObj = theStudy->studyDS()->FindObjectID( theEntry );
  if ( !aSObj )
  {
    return;
  }

  _PTR(StudyBuilder) aBuilder = theStudy->studyDS()->NewBuilder();

  _PTR(AttributeTableOfReal) aRecordsAtt;

  aRecordsAtt = aBuilder->FindOrCreateAttribute( aSObj, "AttributeTableOfReal" );
  aRecordsAtt->SetNbColumns( 0 );

  for ( int it = 0; it < myRecords.size(); ++it )
  {
    bool aVisibility   = myVisibility[it];
    QString& aName     = myNames[it];
    RecordPtr& aRecord = myRecords[it];

    std::vector<double> aPacked;

    // visibility [0]
    aPacked.push_back( (double) aVisibility );

    // type [1]
    aPacked.push_back( (double) aRecord->Type() );

    // values
    aRecord->ToValues( aPacked );

    aRecordsAtt->AddColumn( aPacked );
    aRecordsAtt->SetColumnTitle( it + 1, aName.toStdString() );
  }
}

//=================================================================================
// function : TypeFromIO
// purpose  : 
//=================================================================================
int GEOMGUI_DimensionProperty::TypeFromIO( const Handle(AIS_Dimension)& theIO ) const
{
  if ( theIO->IsKind( STANDARD_TYPE( AIS_LengthDimension ) ) )
  {
    return DimensionType_Length;
  }

  if ( theIO->IsKind( STANDARD_TYPE( AIS_DiameterDimension ) ) )
  {
    return DimensionType_Diameter;
  }

  if ( theIO->IsKind( STANDARD_TYPE( AIS_AngleDimension ) ) )
  {
    return DimensionType_Angle;
  }

  Standard_ProgramError::Raise( "unsupported dimension type" );

  return 0;
}
