#ifndef GEOMGUI_OCCSELECTOR_H
#define GEOMGUI_OCCSELECTOR_H

#include <SalomeApp_OCCSelector.h>

class GEOMGUI_OCCSelector : public SalomeApp_OCCSelector
{
public:
  GEOMGUI_OCCSelector( OCCViewer_Viewer*, SUIT_SelectionMgr* );
  virtual ~GEOMGUI_OCCSelector();

protected:
  virtual void      getSelection( SUIT_DataOwnerPtrList& ) const;
  virtual void      setSelection( const SUIT_DataOwnerPtrList& );

};

#endif
