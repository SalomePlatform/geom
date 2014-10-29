#include "CurveCreator_Displayer.hxx"

CurveCreator_Displayer::CurveCreator_Displayer( Handle_AIS_InteractiveContext theContext,
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
