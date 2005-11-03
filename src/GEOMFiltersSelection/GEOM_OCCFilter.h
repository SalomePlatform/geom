#ifndef GEOM_OCCFILTER_H
#define GEOM_OCCFILTER_H

#include <SelectMgr_Filter.hxx>

#include <Standard_DefineHandle.hxx>

class LightApp_SelectionMgr;

class GEOM_OCCFilter : public SelectMgr_Filter
{
public:
  Standard_EXPORT GEOM_OCCFilter( LightApp_SelectionMgr* theSelMgr);
  Standard_EXPORT ~GEOM_OCCFilter();
  
  Standard_EXPORT virtual Standard_Boolean IsOk( const Handle(SelectMgr_EntityOwner)& anObj ) const;
    
private:
  LightApp_SelectionMgr* mySelMgr;

public:
  
  DEFINE_STANDARD_RTTI(GEOM_OCCFilter);

};

DEFINE_STANDARD_HANDLE(GEOM_OCCFilter, SelectMgr_Filter)

#endif
