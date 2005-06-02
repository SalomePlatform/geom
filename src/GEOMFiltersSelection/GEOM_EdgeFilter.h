#ifndef GEOM_EDGEFILTER_H
#define GEOM_EDGEFILTER_H

#include "GEOM_SelectionFilter.h"

class GEOM_EdgeFilter : public GEOM_SelectionFilter
{
public:
  GEOM_EdgeFilter( SalomeApp_Study* study, const int kind );
  ~GEOM_EdgeFilter();

protected:
  bool                  isShapeOk( const TopoDS_Shape& ) const;

private:
  int                   myKind;

};

#endif
