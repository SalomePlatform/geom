// Copyright (C) 2015-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// File   : GEOMGUI_TextTreeSelector.h
// Author :
//
#ifndef GEOMGUI_TEXTTREESELECTOR_H
#define GEOMGUI_TEXTTREESELECTOR_H

#include "GEOM_GEOMGUI.hxx"

#include <SUIT_Selector.h>
#include <SUIT_DataOwner.h>

#include <QObject>

class GEOMGUI_TextTreeWdg;
class GEOMGUI_AnnotationMgr;

class GEOMGUI_EXPORT GEOMGUI_TextTreeSelector : public QObject, public SUIT_Selector
{
  Q_OBJECT

public:
  GEOMGUI_TextTreeSelector( GEOMGUI_TextTreeWdg* theWidget,
                            GEOMGUI_AnnotationMgr* theAnnotationMgr,
                            SUIT_SelectionMgr* theManager );
  virtual ~GEOMGUI_TextTreeSelector();

  GEOMGUI_TextTreeWdg* textTree() const;

  virtual QString type() const;

private slots:
  void onSelectionChanged();

protected:
  virtual void getSelection( SUIT_DataOwnerPtrList& ) const;
  virtual void setSelection( const SUIT_DataOwnerPtrList& );

private:
  GEOMGUI_TextTreeWdg* myTextTree;
  GEOMGUI_AnnotationMgr* myAnnotationMgr;
  SUIT_DataOwnerPtrList mySelectedList;
};

#endif
