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

#ifndef XAOPlugin_ImportDlg_H
#define XAOPlugin_ImportDlg_H

// GEOM includes
#include "GEOMBase_Skeleton.h"

class QLineEdit;
class QButtonGroup;
class QListWidget;

//=================================================================================
// class    : XAOPlugin_ImportDlg
// purpose  :
//=================================================================================
class XAOPlugin_ImportDlg: public GEOMBase_Skeleton
{
  Q_OBJECT

public:
  XAOPlugin_ImportDlg(GeometryGUI*, QWidget* = 0);
  ~XAOPlugin_ImportDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid( QString& );
  virtual bool                       execute();
  virtual GEOM::GEOM_Object_ptr      getFather( GEOM::GEOM_Object_ptr object );
  virtual QString                    getObjectName( GEOM::GEOM_Object_ptr object ) const;
  virtual QString                    addFieldInStudy( GEOM::GEOM_Field_ptr theField,
                                                      GEOM::GEOM_Object_ptr theFather );

private:
  void Init();
  void enterEvent(QEvent*);

private:
  QLineEdit*            ledFileName;
  QPushButton*          btnFileSelect;
  GEOM::GEOM_Object_var m_mainShape;

private slots:
  void ClickOnOk();
  bool ClickOnApply();
  void ActivateThisDialog();
  void btnFileSelectClicked();
};

#endif // XAOPlugin_ImportDlg_H
