// Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef XAOPlugin_ExportDlg_H
#define XAOPlugin_ExportDlg_H

// GEOM includes
#include "GEOMBase_Skeleton.h"

class QLineEdit;
class QButtonGroup;
class QListWidget;
class QPushButton;

//=================================================================================
// class    : XAOPlugin_ExportDlg
// purpose  :
//=================================================================================
class XAOPlugin_ExportDlg: public GEOMBase_Skeleton
{
  Q_OBJECT

public:
  XAOPlugin_ExportDlg(GeometryGUI*, QWidget* = 0);
  ~XAOPlugin_ExportDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool isValid(QString&);
  virtual bool execute();

private:
  void Init();
  void enterEvent(QEvent*);
  void processObject();

private:
  GEOM::GEOM_Object_var m_mainObj;
  QList<GEOM::GeomObjPtr> m_groups;
  QList<GEOM::GeomFieldPtr> m_fields;
  QLineEdit* ledShape;
  QLineEdit* ledFileName;
  QLineEdit* ledAuthor;
  QListWidget* lstGroups;
  QListWidget* lstFields;
  QPushButton* btnShapeSelect;
  QPushButton* btnFileSelect;

private slots:
  void ClickOnOk();
  bool ClickOnApply();
  void ActivateThisDialog();
  void SelectionIntoArgument();
  void SetEditCurrentArgument();
  void btnFileSelectClicked();
};

#endif // XAOPlugin_ExportDlg_H
