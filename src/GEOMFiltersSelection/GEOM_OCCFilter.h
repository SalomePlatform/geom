#ifndef GEOM_OCCFILTER_H
#define GEOM_OCCFILTER_H

#include <SelectMgr_Filter.hxx>

#include <Standard_DefineHandle.hxx>

class SalomeApp_SelectionMgr;

class GEOM_OCCFilter : public SelectMgr_Filter
{
public:
  GEOM_OCCFilter( SalomeApp_SelectionMgr* theSelMgr);
  ~GEOM_OCCFilter();
  
  virtual Standard_Boolean IsOk( const Handle(SelectMgr_EntityOwner)& anObj ) const;
    
private:
  SalomeApp_SelectionMgr* mySelMgr;

public:
  
  DEFINE_STANDARD_RTTI(GEOM_OCCFilter);

};

DEFINE_STANDARD_HANDLE(GEOM_OCCFilter, SelectMgr_Filter)

#endif
