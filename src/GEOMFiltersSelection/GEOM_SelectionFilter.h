#ifndef GEOM_SELECTIONFILTER_H
#define GEOM_SELECTIONFILTER_H

#include <SalomeApp_Filter.h>

#include <TopoDS_Shape.hxx>

#include <qvaluelist.h>

// IDL Headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Gen)

class Standard_EXPORT GEOM_SelectionFilter : public SalomeApp_Filter
{
public:
  GEOM_SelectionFilter( SalomeApp_Study* study, const bool theAll = false );
  ~GEOM_SelectionFilter();

  virtual bool isOk( const SUIT_DataOwner* ) const;

protected:
  GEOM::GEOM_Object_ptr getObject( const SUIT_DataOwner*, const bool = true ) const;
  bool                  getShape( const GEOM::GEOM_Object_ptr&, TopoDS_Shape& ) const;
  
  bool                  contains( const int ) const;
  void                  add( const int );
  void                  remove( const int );

  void                  setAll( const bool );
  bool                  isAll() const;

  bool                  isShapeOk( const TopoDS_Shape& ) const;

private:
  QValueList<int>       myTypes;
  bool                  myAll;

};

#endif
