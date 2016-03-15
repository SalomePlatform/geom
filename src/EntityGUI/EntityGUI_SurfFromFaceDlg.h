// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// GEOM GEOMGUI : GUI for Geometry component
// File   : EntityGUI_SurfFromFaceDlg.h

#ifndef ENTITYGUI_SURFFROMFACEDLG_H
#define ENTITYGUI_SURFFROMFACEDLG_H


#include <GEOMBase_Skeleton.h>

class DlgRef_1Sel;

//=================================================================================
// class    : EntityGUI_SurfFromFaceDlg
// purpose  :
//=================================================================================
class EntityGUI_SurfFromFaceDlg : public GEOMBase_Skeleton
{ 
  Q_OBJECT

public:

  EntityGUI_SurfFromFaceDlg (GeometryGUI     *theGeometryGUI,
                             QWidget         *parent = 0,
                             bool             modal  = false,
                             Qt::WindowFlags  fl     = 0);

  ~EntityGUI_SurfFromFaceDlg();

protected:
 
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid( QString& );
  virtual bool                       execute( ObjectList& );
  virtual QList<GEOM::GeomObjPtr>    getSourceObjects();

private:

  void                               Init();
  void                               enterEvent( QEvent* );

private:

  DlgRef_1Sel                       *myGroup;
  GEOM::GeomObjPtr                   myFace;

private slots:

  void                               ClickOnOk();
  bool                               ClickOnApply();
  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();
  void                               ActivateThisDialog();

};

#endif // ENTITYGUI_SURFFROMFACEDLG_H
