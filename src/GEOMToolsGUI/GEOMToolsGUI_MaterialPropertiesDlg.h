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

// File   : GEOMToolsGUI_MaterialPropertiesDlg.h
// Author : Margarita KARPUNINA, Open CASCADE S.A.S. (margarita.karpunina@opencascade.com)
//
#ifndef GEOMTOOLSGUI_MATERIALPROPERTIESDLG_H
#define GEOMTOOLSGUI_MATERIALPROPERTIESDLG_H

#include "GEOM_ToolsGUI.hxx"

#include <QListWidget>
#include <QtxDialog.h>
#include "Material_Model.h"
#include "Material_ResourceMgr.h"

class QCheckBox;
class QLabel;
class QPushButton;
class QtxDoubleSpinBox;
class QtxColorButton;
class GEOMToolsGUI_MaterialList;

class GEOMTOOLSGUI_EXPORT GEOMToolsGUI_MaterialPropertiesDlg : public QtxDialog
{
  Q_OBJECT

  enum { Current, Global, User };
  enum { TypeRole = Qt::UserRole + 123, NameRole  };

public:
  GEOMToolsGUI_MaterialPropertiesDlg(  QWidget* = 0 , bool  = true, bool  = false,const int  = Standard  );
  ~GEOMToolsGUI_MaterialPropertiesDlg();  

  void                  accept();

  bool                  eventFilter( QObject*, QEvent* );

private:
  void                  updateState();
  void                  toModel( Material_Model& ) const;
  void                  fromModel( const Material_Model& );
  QString               findUniqueName( const QString&,
                                        QListWidgetItem* = 0,
                                        bool = false );

signals:
  void                  changed();

private slots:
  void                  onChanged();
  void                  onMaterialChanged();
  void                  onItemChanged( QListWidgetItem* );
  void                  onContextMenu( QContextMenuEvent* );
  void                  onDeleteMaterial();
  void                  onAddMaterial();
  void                  onApply();
  void                  onHelp();
  void                  SetEditCurrentArgument();
  void                  SelectionIntoArgument();

private:
  typedef struct
  {
    QCheckBox*        enabled;
    QLabel*           label;
    QtxColorButton*   color;
    QtxDoubleSpinBox* front_coef;
    QtxDoubleSpinBox* back_coef;
  } Reflection;

  GEOMToolsGUI_MaterialList* myMaterials;
  QCheckBox*                 myPhysical;
  QList<Reflection>          myReflection;
  QtxDoubleSpinBox*          myFrontShininess;  
  QtxDoubleSpinBox*          myBackShininess;
  QLabel*                    myColorLab;
  QtxColorButton*            myColor;
  QPushButton*               myAddButton;
  QPushButton*               myDelButton;
  Material_ResourceMgr       myResourceMgr;
  Material_Model             myCurrentModel;
  bool                       myShowSelWidget;
  QLineEdit*                 myLineEditCurArg;
  QPushButton*               myPushBtn;
};

class GEOMToolsGUI_MaterialList : public QListWidget
{
Q_OBJECT
public:
  GEOMToolsGUI_MaterialList( QWidget* parent );
protected:
  void contextMenuEvent( QContextMenuEvent* e );
signals:
  void contextMenu( QContextMenuEvent* );
};

#endif // GEOMTOOLSGUI_MATERIALPROPERTIESDLG_H
