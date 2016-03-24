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
// File   : EntityGUI_IsolineDlg.h

#ifndef ENTITYGUI_ISOLINEDLG_H
#define ENTITYGUI_ISOLINEDLG_H


#include <GEOMBase_Skeleton.h>

class DlgRef_3Radio1Sel1Spin;

//=================================================================================
// class    : EntityGUI_IsolineDlg
// purpose  :
//=================================================================================
class EntityGUI_IsolineDlg : public GEOMBase_Skeleton
{ 
  Q_OBJECT

public:

  EntityGUI_IsolineDlg (GeometryGUI     *theGeometryGUI,
                        QWidget         *parent = 0,
                        bool             modal  = false,
                        Qt::WindowFlags  fl     = 0);

  ~EntityGUI_IsolineDlg();

protected:
 
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();

  virtual bool                       isValid( QString& );

  virtual bool                       execute( ObjectList& );

  virtual void                       addSubshapesToStudy();

  virtual QList<GEOM::GeomObjPtr>    getSourceObjects();

private:

  void                               Init();
  void                               enterEvent( QEvent* );

private:

  DlgRef_3Radio1Sel1Spin            *myGroup;
  QButtonGroup                      *myRBGroup;
  GEOM::GeomObjPtr                   myFace;


private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();

  void                               SetEditFace();
  void                               ValueChangedInSpinBox( double );
  void                               SelectionIntoArgument();
  void                               ActivateThisDialog();
  void                               SetDoubleSpinBoxStep( double );
  void                               TypeChanged(int);
};

#endif // ENTITYGUI_ISOLINEDLG_H
