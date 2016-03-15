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

#ifndef OPERATIONGUI_EXTRACTIONDLG_H
#define OPERATIONGUI_EXTRACTIONDLG_H

#include <GEOMBase_Skeleton.h>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TColStd_MapOfInteger.hxx>

class QComboBox;
class QListWidget;
class QTreeWidget;
class QTreeWidgetItem;


//=================================================================================
// class    : OperationGUI_ExtractionDlg
// purpose  :
//=================================================================================
class OperationGUI_ExtractionDlg : public GEOMBase_Skeleton
{

  Q_OBJECT

public:

  OperationGUI_ExtractionDlg( GeometryGUI*, QWidget* );
  ~OperationGUI_ExtractionDlg();

protected:

  // redefined from GEOMBase_Helper and MeasureGUI_Skeleton
  virtual GEOM::GEOM_IOperations_ptr  createOperation();
  virtual bool                        execute(ObjectList &);
  virtual void                        activateSelection();
  virtual bool                        isValid( QString& );

private slots:

  void                                SelectionIntoArgument();
  void                                ClickOnOk();
  bool                                ClickOnApply();
  void                                onShapeTypeChanged();
  void                                onAddExtracted();
  void                                onRemoveExtracted();
  void                                onListSelectionChanged();
  void                                showOnlySelected();
  void                                hideSelected();
  void                                showAllSelected();
  void                                onRebuild();
  void                                ActivateThisDialog();
  void                                SetEditCurrentArgument();

private:

  void                                Init();
  bool                                updateSubShTypeCompo();
  void                                updateFilteredList();
  void                                resetBuildData(const bool isEnableBuild);
  bool                                isEmptyExtracted();
  void                                selectMainShape();
  void                                selectSubShapes();
  void                                enterEvent(QEvent *);
  bool                                getResult
             (GEOM::GEOM_Object_ptr                         &theResult,
              GEOM::GEOM_IShapesOperations::ExtractionStats &theStats);
  QString                             getSubShapeEntry(const int theId);
  Handle_SALOME_InteractiveObject     createIO(const char *theEntry);
  void                                displaySubShape(const int theId);
  void                                eraseSubShape(const int theId);
  void                                eraseAll();
  void                                restoreViewer();

private:

  GEOM::GEOM_Object_var       myObj;
  QPushButton                *mySelBtn;
  QLineEdit                  *myMainShapeEdit;
  QComboBox                  *mySubShTypeCompo;
  QListWidget                *myFilteredList;
  QTreeWidget                *myExtractedTree;
  QListWidget                *myRemovedList;
  QListWidget                *myModifiedList;
  QListWidget                *myAddedList;
  QTreeWidgetItem            *myTopItems[8];
  QPushButton                *myRebuildBtn;
  TColStd_MapOfInteger        myMapExtractedIDs;
  bool                        myIsHiddenMain;
  TColStd_MapOfInteger        myMapDisplayedIDs;
  TopTools_IndexedMapOfShape  myIndices;

};

#endif // OPERATIONGUI_EXTRACTIONDLG_H
