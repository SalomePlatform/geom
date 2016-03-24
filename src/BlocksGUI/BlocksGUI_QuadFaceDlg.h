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
// File   : BlocksGUI_QuadFaceDlg.h
// Author : Julia DOROVSKIKH, Open CASCADE S.A.S. (julia.dorovskikh@opencascade.com)
//
#ifndef BLOCKSGUI_QUADFACEDLG_H
#define BLOCKSGUI_QUADFACEDLG_H

#include "GEOMBase_Skeleton.h"
#include "GEOM_GenericObjPtr.h"

#include <QMap>

class QGroupBox;
class QPushButton;
class QLineEdit;

//=================================================================================
// class    : BlocksGUI_QuadFaceDlg
// purpose  :
//=================================================================================
class BlocksGUI_QuadFaceDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

  enum { Vertex1, Vertex2, Vertex3, Vertex4,
         Edge12, Edge22,
         Edge14, Edge24, Edge34, Edge44 };

public:
  BlocksGUI_QuadFaceDlg (GeometryGUI*, QWidget*);
  ~BlocksGUI_QuadFaceDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid (QString&);
  virtual bool                       execute (ObjectList&);
  virtual void                       addSubshapesToStudy();
  virtual QList<GEOM::GeomObjPtr>    getSourceObjects();

private:
  void                               Init();
  void                               enterEvent (QEvent*);

  void                               createSelWg (const QString&, QPixmap&,
                                                  QWidget*, const int);
  void                               activateSelection();

private:
  int                                myConstructorId;

  GEOM::GeomObjPtr                   myShape1;
  GEOM::GeomObjPtr                   myShape2;
  GEOM::GeomObjPtr                   myShape3;
  GEOM::GeomObjPtr                   myShape4;

  // to initialize the first selection field with a selected object on the dialog creation
  bool                               myInitial;

  QGroupBox*                         myGrp1;
  QGroupBox*                         myGrp2;
  QGroupBox*                         myGrp3;

  QMap<int, QPushButton*>            mySelBtn;
  QMap<int, QLineEdit*>              mySelName;

private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();
  void                               ActivateThisDialog();
  void                               ConstructorsClicked (int);

  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();
};

#endif // BLOCKSGUI_QUADFACEDLG_H
