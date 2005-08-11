#ifndef GEOM_LOGICALFILTER_H
#define GEOM_LOGICALFILTER_H

#include <SUIT_SelectionFilter.h>
#include <Standard_Macro.hxx>

#include <qptrlist.h>

class Standard_EXPORT GEOM_LogicalFilter : public SUIT_SelectionFilter
{
public:
  enum { LO_OR, LO_AND, LO_NOT, LO_UNDEFINED };

public:
  GEOM_LogicalFilter( const QPtrList<SUIT_SelectionFilter>& lst, const int op );
  ~GEOM_LogicalFilter();

  virtual bool                   isOk( const SUIT_DataOwner* ) const;

  void                           setFilters( const QPtrList<SUIT_SelectionFilter>& lst );
  void                           setOperation( const int );
  QPtrList<SUIT_SelectionFilter> getFilters() const;
  int                            getOperation() const;

private:
  QPtrList<SUIT_SelectionFilter>  myFilters;
  int                             myOperation;

};

#endif
