// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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
  ArrowPos = theIO->DimensionAspect()->ArrowOrientation();
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
GEOMGUI_DimensionProperty::operator QVariant()
{
  QVariant aQVariant;
  aQVariant.setValue( *this );
  return aQVariant;
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
      case DimensionType_Length :
      {
        Length* aLength = new Length;

        // custom plane [2,3,4,5]
        Standard_Real A = (Standard_Real) aPacked[it++];
        Standard_Real B = (Standard_Real) aPacked[it++];
        Standard_Real C = (Standard_Real) aPacked[it++];
        Standard_Real D = (Standard_Real) aPacked[it++];
        aLength->Plane = gp_Pln( A, B, C, D );

        // flyout size [6]
        aLength->Flyout = (Standard_Real) aPacked[it++];

        // text flags [7,8]
        aLength->TextHPos = (Prs3d_DimensionTextHorizontalPosition)(int)aPacked[it++];
        aLength->TextVPos = (Prs3d_DimensionTextVerticalPosition)  (int)aPacked[it++];

        // arrow flags [9]
        aLength->ArrowPos = (Prs3d_DimensionArrowOrientation) (int)aPacked[it++];

        // point 1 [10,11,12]
        Standard_Real aFirstX = aPacked[it++];
        Standard_Real aFirstY = aPacked[it++];
        Standard_Real aFirstZ = aPacked[it++];
        aLength->FirstPoint = gp_Pnt( aFirstX, aFirstY, aFirstZ );

        // point 2 [13,14,15]
        Standard_Real aSecondX = aPacked[it++];
        Standard_Real aSecondY = aPacked[it++];
        Standard_Real aSecondZ = aPacked[it++];
        aLength->SecondPoint = gp_Pnt( aSecondX, aSecondY, aSecondZ );

        aRecord = RecordPtr( aLength );
        break;
      }

      case DimensionType_Diameter :
      {
        Diameter* aDiam = new Diameter;

        // custom plane [2,3,4,5]
        Standard_Real A = (Standard_Real) aPacked[it++];
        Standard_Real B = (Standard_Real) aPacked[it++];
        Standard_Real C = (Standard_Real) aPacked[it++];
        Standard_Real D = (Standard_Real) aPacked[it++];
        aDiam->Plane = gp_Pln( A, B, C, D );

        // flyout size [6]
        aDiam->Flyout = (Standard_Real) aPacked[it++];

        // text flags [7,8]
        aDiam->TextHPos = (Prs3d_DimensionTextHorizontalPosition)(int)aPacked[it++];
        aDiam->TextVPos = (Prs3d_DimensionTextVerticalPosition)  (int)aPacked[it++];

        // arrow flags [9]
        aDiam->ArrowPos = (Prs3d_DimensionArrowOrientation) (int)aPacked[it++];

        // circle location [10,11,12]
        Standard_Real aLocX = (Standard_Real) aPacked[it++];
        Standard_Real aLocY = (Standard_Real) aPacked[it++];
        Standard_Real aLocZ = (Standard_Real) aPacked[it++];

        // circle normal [13,14,15]
        Standard_Real aNormX = (Standard_Real) aPacked[it++];
        Standard_Real aNormY = (Standard_Real) aPacked[it++];
        Standard_Real aNormZ = (Standard_Real) aPacked[it++];

        // x-direction [16,17,18]
        Standard_Real aXDirX = (Standard_Real) aPacked[it++];
        Standard_Real aXDirY = (Standard_Real) aPacked[it++];
        Standard_Real aXDirZ = (Standard_Real) aPacked[it++];

        // radius [19]
        Standard_Real aRadius = (Standard_Real) aPacked[it++];

        gp_Ax2 anAx( gp_Pnt( aLocX, aLocY, aLocZ ),
                     gp_Dir( aNormX, aNormY, aNormZ ),
                     gp_Dir( aXDirX, aXDirY, aXDirZ ) );

        aDiam->Circle = gp_Circ( anAx, aRadius );

        aRecord = RecordPtr( aDiam );
        break;
      }

      case DimensionType_Angle :
      {
        Angle* anAngle = new Angle;

        // flyout [2]
        anAngle->Flyout = (Standard_Real) aPacked[it++];

        // text flags [3,4]
        anAngle->TextHPos = (Prs3d_DimensionTextHorizontalPosition)(int)aPacked[it++];
        anAngle->TextVPos = (Prs3d_DimensionTextVerticalPosition)  (int)aPacked[it++];

        // arrow flags [5]
        anAngle->ArrowPos = (Prs3d_DimensionArrowOrientation) (int)aPacked[it++];

        // point 1 [6,7,8]
        Standard_Real aFirstX = (Standard_Real) aPacked[it++];
        Standard_Real aFirstY = (Standard_Real) aPacked[it++];
        Standard_Real aFirstZ = (Standard_Real) aPacked[it++];

        // point 2 [9,10,11]
        Standard_Real aSecondX = (Standard_Real) aPacked[it++];
        Standard_Real aSecondY = (Standard_Real) aPacked[it++];
        Standard_Real aSecondZ = (Standard_Real) aPacked[it++];

        // center [12,13,14]
        Standard_Real aCenterX = (Standard_Real) aPacked[it++];
        Standard_Real aCenterY = (Standard_Real) aPacked[it++];
        Standard_Real aCenterZ = (Standard_Real) aPacked[it++];

        anAngle->FirstPoint  = gp_Pnt( aFirstX, aFirstY, aFirstZ );
        anAngle->SecondPoint = gp_Pnt( aSecondX, aSecondY, aSecondZ );
        anAngle->CenterPoint = gp_Pnt( aCenterX, aCenterY, aCenterZ );

        aRecord = RecordPtr( anAngle );
        break;
      }
    }

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

    switch ( aRecord->Type() )
    {
      case DimensionType_Length:
      {
        Length* aProps = aRecord->AsLength();

        // custom plane [2,3,4,5]
        Standard_Real A, B, C, D;
        aProps->Plane.Coefficients( A, B, C, D );
        aPacked.push_back( (double) A );
        aPacked.push_back( (double) B );
        aPacked.push_back( (double) C );
        aPacked.push_back( (double) D );

        // flyout size [6]
        aPacked.push_back( (double) aProps->Flyout );

        // text flags [7,8]
        aPacked.push_back( (double) aProps->TextHPos );
        aPacked.push_back( (double) aProps->TextVPos );

        // arrow flags [9]
        aPacked.push_back( (double) aProps->ArrowPos );

        // point 1 [10,11,12]
        aPacked.push_back( (double) aProps->FirstPoint.X() );
        aPacked.push_back( (double) aProps->FirstPoint.Y() );
        aPacked.push_back( (double) aProps->FirstPoint.Z() );

        // point 2 [13,14,15]
        aPacked.push_back( (double) aProps->SecondPoint.X() );
        aPacked.push_back( (double) aProps->SecondPoint.Y() );
        aPacked.push_back( (double) aProps->SecondPoint.Z() );
        break;
      }

      case DimensionType_Diameter:
      {
        Diameter* aProps = aRecord->AsDiameter();

        // custom plane [2,3,4,5]
        Standard_Real A, B, C, D;
        aProps->Plane.Coefficients( A, B, C, D );
        aPacked.push_back( (double) A );
        aPacked.push_back( (double) B );
        aPacked.push_back( (double) C );
        aPacked.push_back( (double) D );

        // flyout size [6]
        aPacked.push_back( (double) aProps->Flyout );

        // text flags [7,8]
        aPacked.push_back( (double) aProps->TextHPos );
        aPacked.push_back( (double) aProps->TextVPos );

        // arrow flags [9]
        aPacked.push_back( (double) aProps->ArrowPos );

        // circle location [10,11,12]
        aPacked.push_back( (double) aProps->Circle.Location().X() );
        aPacked.push_back( (double) aProps->Circle.Location().Y() );
        aPacked.push_back( (double) aProps->Circle.Location().Z() );

        // circle normal [13,14,15]
        aPacked.push_back( (double) aProps->Circle.Axis().Direction().X() );
        aPacked.push_back( (double) aProps->Circle.Axis().Direction().Y() );
        aPacked.push_back( (double) aProps->Circle.Axis().Direction().Z() );

        // x-direction [16,17,18]
        aPacked.push_back( (double) aProps->Circle.XAxis().Direction().X() );
        aPacked.push_back( (double) aProps->Circle.XAxis().Direction().Y() );
        aPacked.push_back( (double) aProps->Circle.XAxis().Direction().Z() );

        // radius [19]
        aPacked.push_back( (double) aProps->Circle.Radius() );
        break;
      }

      case DimensionType_Angle:
      {
        Angle* aProps = aRecord->AsAngle();

        // flyout [2]
        aPacked.push_back( (double) aProps->Flyout );

        // text flags [3,4]
        aPacked.push_back( (double) aProps->TextHPos );
        aPacked.push_back( (double) aProps->TextVPos );

        // arrow flags [5]
        aPacked.push_back( (double) aProps->ArrowPos );

        // point 1 [6,7,8]
        aPacked.push_back( (double) aProps->FirstPoint.X() );
        aPacked.push_back( (double) aProps->FirstPoint.Y() );
        aPacked.push_back( (double) aProps->FirstPoint.Z() );

        // point 2 [9,10,11]
        aPacked.push_back( (double) aProps->SecondPoint.X() );
        aPacked.push_back( (double) aProps->SecondPoint.Y() );
        aPacked.push_back( (double) aProps->SecondPoint.Z() );

        // center [12,13,14]
        aPacked.push_back( (double) aProps->CenterPoint.X() );
        aPacked.push_back( (double) aProps->CenterPoint.Y() );
        aPacked.push_back( (double) aProps->CenterPoint.Z() );
        break;
      }
    }

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
