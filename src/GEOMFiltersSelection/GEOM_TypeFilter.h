#ifndef GEOM_TYPEFILTER_H
#define GEOM_TYPEFILTER_H

#include "GEOM_SelectionFilter.h"

// IDL Headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Gen)

class GEOM_TypeFilter : public GEOM_SelectionFilter
{
public:
  GEOM_TypeFilter(SalomeApp_Study* study, const int type );
  ~GEOM_TypeFilter();

  virtual bool isOk( const SUIT_DataOwner* ) const;
  
  int          type() const;

private:
  int                   myType;

};

#endif
