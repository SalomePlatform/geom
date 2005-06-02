#ifndef GEOM_FACEFILTER_H
#define GEOM_FACEFILTER_H

#include "GEOM_SelectionFilter.h"

class GEOM_FaceFilter : public GEOM_SelectionFilter
{
public:
  GEOM_FaceFilter( SalomeApp_Study* study, const int kind );
  ~GEOM_FaceFilter();

protected:
  bool                  isShapeOk( const TopoDS_Shape& ) const;

private:
  int                   myKind;

};

#endif
