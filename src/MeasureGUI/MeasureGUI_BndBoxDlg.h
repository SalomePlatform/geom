// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
// File   : MeasureGUI_BndBoxDlg.h
// Author : Nicolas REJNERI, Open CASCADE S.A.S.

#ifndef MEASUREGUI_BNDBOXDLG_H
#define MEASUREGUI_BNDBOXDLG_H

#include "GEOMBase_Skeleton.h"

class MeasureGUI_1Sel6LineEdit;

//=================================================================================
// class    : DialogBox_PROPERTIES
// purpose  :
//=================================================================================
class MeasureGUI_BndBoxDlg : public GEOMBase_Skeleton
{ 
  Q_OBJECT

public:
  MeasureGUI_BndBoxDlg (GeometryGUI*, QWidget*);
  ~MeasureGUI_BndBoxDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid (QString&);
  virtual bool                       execute (ObjectList&);
  virtual void                       addSubshapesToStudy();
  void                               redisplayPreview();
  virtual void                       processObject();
  virtual SALOME_Prs*                buildPrs();
  virtual QList<GEOM::GeomObjPtr>    getSourceObjects();

private:
  void                               Init();
  void                               enterEvent (QEvent*);
  void                               activateSelection();
  bool                               getParameters (double&, double&, double&, double&, double&, double&);

private:
  GEOM::GeomObjPtr                   myObj;
  MeasureGUI_1Sel6LineEdit*          myGrp;

private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();
  void                               SelectionIntoArgument();
  void                               ActivateThisDialog();
};

#endif // MEASUREGUI_BNDBOXDLG_H
