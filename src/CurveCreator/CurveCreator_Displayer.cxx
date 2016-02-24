// Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "CurveCreator_Displayer.hxx"

CurveCreator_Displayer::CurveCreator_Displayer( Handle(AIS_InteractiveContext) theContext,
                                                const int theZLayer ) :
  myContext( theContext ), myZLayer( theZLayer )
{
  myObjects.clear();
}

CurveCreator_Displayer::~CurveCreator_Displayer(void)
{
  eraseAll( true );
  for( int i = 0 ; i < myObjects.size() ; i++ ){
    myObjects[i].Nullify();
  }
  myObjects.clear();
}

void CurveCreator_Displayer::display( const Handle(AIS_InteractiveObject)& theObject, bool isUpdate )
{
  if ( theObject.IsNull() )
    return;

  myObjects.push_back( theObject );
  myContext->Display( theObject, Standard_False );

  if ( myZLayer >= 0 )
    myContext->SetZLayer( theObject, myZLayer );

  if( isUpdate )
    myContext->UpdateCurrentViewer();
}

void CurveCreator_Displayer::eraseAll( bool isUpdate )
{
  if(myObjects.empty())
    return;
  for( int i = 0 ; i < myObjects.size() ; i++ ){
    myContext->Erase(myObjects[i], Standard_False);
  }
  myObjects.clear();
  if( isUpdate )
    myContext->UpdateCurrentViewer();
}

Quantity_Color CurveCreator_Displayer::getActiveColor( bool isHL )
{
  if( isHL ){
    return Quantity_Color( 1., 0., 0., Quantity_TOC_RGB );
  }
  return Quantity_Color( 0., 1., 0., Quantity_TOC_RGB );
}

/*void CurveCreator_Displayer::highlight( const AISObjectsList& theObjects, bool isHL )
{
  return;
  //TODO:
  Quantity_Color aColor = getActiveColor( isHL );
  for( int i = 0 ; i < theObjects.size() ; i++ ){
    theObjects[i]->SetColor(aColor);
    myContext->Display(theObjects[i], Standard_False);
  }
  myContext->UpdateCurrentViewer();
}*/
