#ifndef CURVECREATOR_DISPLAYER_H
#define CURVECREATOR_DISPLAYER_H

#include "CurveCreator_Macro.hxx"

#include <AIS_InteractiveObject.hxx>
#include <AIS_InteractiveContext.hxx>

#include <vector>

class CURVECREATOR_EXPORT CurveCreator_Displayer
{
typedef std::vector<Handle_AIS_InteractiveObject> AISObjectsList;

public:
  CurveCreator_Displayer( Handle_AIS_InteractiveContext theContext,
                          const int theZLayer = -1 );
  ~CurveCreator_Displayer(void);

  void display( const Handle_AIS_InteractiveObject& theObject, bool isUpdate );
  void eraseAll( bool isUpdate );
  //void highlight( const AISObjectsList& theObjects, bool isHL );

protected:
  Quantity_Color getActiveColor( bool isHL );

private:
  Handle_AIS_InteractiveContext myContext;
  AISObjectsList                myObjects;
  int                           myZLayer;
};

#endif
