// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#include <QtxDialog.h>
#include <QFrame>
#include <QMap>

class QCheckBox;
class QGroupBox;
class QLabel;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QSpinBox;
class QTabWidget;

class QtxColorButton;
class QtxDoubleSpinBox;

class Material_Model;
class Material_ResourceMgr;

class GEOMTOOLSGUI_EXPORT GEOMToolsGUI_MaterialPropertiesDlg : public QtxDialog
{
  Q_OBJECT

  enum { Current, Default, Global, User };
  enum { TypeRole = Qt::UserRole + 123, NameRole  };

  //! Enumeration of viewer types
  typedef enum {
    OCC,  //!< OCC viewer
    VTK  //!< VTK viewer
  } ViewerType;

public:
  GEOMToolsGUI_MaterialPropertiesDlg( QWidget* = 0 );
  ~GEOMToolsGUI_MaterialPropertiesDlg();  

  void                  accept();

  bool                  eventFilter( QObject*, QEvent* );

private:
  Material_ResourceMgr* resourceMgr();

  void                  fromModel( Material_Model* );
  void                  toModel( Material_Model* ) const;
  void                  toFrontModel( Material_Model* ) const;
  void                  toBackModel( Material_Model* ) const;
  
  QString               findUniqueName( const QString&,
					QListWidgetItem* = 0,
					bool = false );

  bool                  isFrontTabActive() const;

signals:
  void                  materialChanged();
  void                  changed();

private slots:

  void                  onApply();
  void                  onHelp();

  void                  onBackMaterialChecked( bool );
  void                  onCurrentTabChanged( int );
  void                  onMaterialChanged();
  void                  onChanged();
  void                  onItemChanged( QListWidgetItem* );
  void                  onReflectionTypeToggled( bool );

private:
  
  Material_ResourceMgr* myResMgr;

  QCheckBox*            myBackMaterialCheck;

  //! Current material model for front material
  Material_Model*       myCurrentModelF;

  //! Current material model for back material
  Material_Model*       myCurrentModelB;

  QListWidget*          myMaterialList;
  int                   myMaterialListFId;
  int                   myMaterialListBId;

  QTabWidget*           myMaterialTab;
  QWidget*              myMaterialBWidget;
  bool                  myIsBTabWasActivated;

  //! Controls defining front material properties
  QGroupBox*            myAmbientGroupF;
  QtxColorButton*       myAmbientColorF;
  QtxDoubleSpinBox*     myAmbientCoefntF;

  QGroupBox*            myDiffuseGroupF;
  QtxColorButton*       myDiffuseColorF;
  QtxDoubleSpinBox*     myDiffuseCoefntF;

  QGroupBox*            mySpecularGroupF;
  QtxColorButton*       mySpecularColorF;
  QtxDoubleSpinBox*     mySpecularCoefntF;

  QGroupBox*            myEmissionGroupF;
  QtxColorButton*       myEmissionColorF;
  QtxDoubleSpinBox*     myEmissionCoefntF;

  QtxDoubleSpinBox*     myShininessF;

  //! Controls defining back material properties
  QGroupBox*            myAmbientGroupB;
  QtxColorButton*       myAmbientColorB;
  QtxDoubleSpinBox*     myAmbientCoefntB;

  QGroupBox*            myDiffuseGroupB;
  QtxColorButton*       myDiffuseColorB;
  QtxDoubleSpinBox*     myDiffuseCoefntB;

  QGroupBox*            mySpecularGroupB;
  QtxColorButton*       mySpecularColorB;
  QtxDoubleSpinBox*     mySpecularCoefntB;

  QGroupBox*            myEmissionGroupB;
  QtxColorButton*       myEmissionColorB;
  QtxDoubleSpinBox*     myEmissionCoefntB;

  QtxDoubleSpinBox*     myShininessB;

  QString               myHelpFileName;

  ViewerType            myViewerType;
  
};

#endif // GEOMTOOLSGUI_MATERIALPROPERTIESDLG_H
