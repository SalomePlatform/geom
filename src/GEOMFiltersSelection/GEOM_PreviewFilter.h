#ifndef GEOM_PREVIEW_FILTER_H
#define GEOM_PREVIEW_FILTER_H

#include <SalomeApp_Filter.h>
#include <Standard_Macro.hxx>

class Standard_EXPORT GEOM_PreviewFilter : public SalomeApp_Filter
{
public:
  GEOM_PreviewFilter( SalomeApp_Study* study );
  ~GEOM_PreviewFilter();

  virtual bool                   isOk( const SUIT_DataOwner* ) const;
};

#endif
