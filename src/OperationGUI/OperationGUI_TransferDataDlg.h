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


#ifndef OPERATIONGUI_TRANSFERDATADLG_H
#define OPERATIONGUI_TRANSFERDATADLG_H


#include <GEOMBase_Skeleton.h>

class DlgRef_2Sel1List;

//=================================================================================
// class    : OperationGUI_TransferDataDlg
// purpose  :
//=================================================================================
class OperationGUI_TransferDataDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

public:
  OperationGUI_TransferDataDlg
            ( GeometryGUI* theGeometryGUI, QWidget* parent = 0 );
  ~OperationGUI_TransferDataDlg();

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
  GEOM::GeomObjPtr                   myObject1;
  GEOM::GeomObjPtr                   myObject2;
  int                                myShapeType;

  DlgRef_2Sel1List*                  myGroup;

private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();
  void                               SetEditCurrentArgument();
  void                               SelectionIntoArgument();
  void                               LineEditReturnPressed();
  void                               ActivateThisDialog();
};

#endif // OPERATIONGUI_GETSHAPESONSHAPEDLG_H
