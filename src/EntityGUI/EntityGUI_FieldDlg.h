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

//  GEOM GEOMGUI : GUI for Geometry component
//  File   : EntityGUI_FieldDlg.h

#ifndef EntityGUI_FieldDlg_H
#define EntityGUI_FieldDlg_H

#include <GEOMBase_Skeleton.h>

#include <TopAbs_ShapeEnum.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <QMap>
#include <QSet>
#include <QVector>
#include <QTableWidget>

class QGroupBox;
class QLineEdit;
class QPushButton;
class QComboBox;
class SalomeApp_IntSpinBox;

//=================================================================================
// class    : EntityGUI_FieldDlg
// purpose  :
//=================================================================================
class EntityGUI_FieldDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

  class StepTable;
  class IntSpinItem;
  class DoubleSpinItem;
  class CheckItem;
  class Delegate;

public:
  EntityGUI_FieldDlg (GeometryGUI* theGeometryGUI,
                      GEOM::GEOM_Field_ptr theField, int stepID=0,
                      QWidget* parent=0,
                      bool modal=false, Qt::WindowFlags fl=0);
  ~EntityGUI_FieldDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr  createOperation();
  virtual bool                        isValid (QString&);
  virtual bool                        execute ();
  virtual QList<GEOM::GeomObjPtr>     getSourceObjects();

private slots:
  void                                ClickOnOk();
  bool                                ClickOnApply();
  void                                ActivateThisDialog();
  void                                SelectionIntoArgument();
  void                                SetEditCurrentArgument();

  void                                onPrevStep();
  void                                onNextStep();
  void                                onAddStep();
  void                                onRmStep();
  void                                onStampChange();
  void                                onDimChange();
  void                                onTypeChange();
  void                                onNbCompsChange();
  void                                showCurStep();
  void                                highlightSubShapes();

private:
  void                                Init();
  void                                enterEvent (QEvent*);
  void                                activateSelection();

  TopAbs_ShapeEnum                    getShapeType(int* dim=0) const;
  int                                 getDim() const;
  int                                 getDataType() const;
  int                                 getCurStepID() const;
  int                                 getNbComps() const;
  void                                updateShapeIDs();
  void                                updateDims(int curDim=-1);
  int                                 getSelectedSubshapes (TColStd_IndexedMapOfInteger& map);

private:
  bool                                myIsCreation;
  GEOM::GEOM_Field_var                myField;
  GEOM::GEOM_Object_var               myShape;
  QVector< int >                      myShapeIDs;
  TopTools_IndexedMapOfShape          myShapeMap;

  int                                 myCurStepID;
  StepTable*                          myCurStepTable;
  QMap< int, StepTable* >             myStepTables;
  QSet< int >                         myRemovedSteps;

  int                                 myDmMode;
  bool                                myIsHiddenMain;

  QList<int>                          myHiddenFieldStepIds;

  QPushButton*                        myShapeSelBtn;
  QLineEdit*                          myShapeName;
  QComboBox*                          myTypeCombo;
  QComboBox*                          myDimCombo;
  QPushButton*                        myPrevStepBtn;
  QPushButton*                        myNextStepBtn;
  QPushButton*                        myRmStepBtn;

  SalomeApp_IntSpinBox*               myNbCompsSpin;
  QWidget*                            mySwitchTableWdg;
  QComboBox*                          myStepsCombo;
  QLineEdit*                          myStepEdit;
  SalomeApp_IntSpinBox*               myStampSpin;

};

/*
  Class       : EntityGUI_FieldDlg::StepTable
  Description : Table widget
*/

class EntityGUI_FieldDlg::StepTable : public QTableWidget
{
  Q_OBJECT

  int                      myDataType;
  int                      myStepID;
  int                      myStamp;
  GEOM::GEOM_FieldStep_var myStep;
  bool                     myIsChanged;

  QTableWidgetItem * newDefaultItem();

public:
  StepTable( int stepID, int dataType, int nbRows, int nbColumns,
             QString shapeName, QStringList headers,
             GEOM::GEOM_FieldStep_ptr stepVar, QWidget* = 0 );
  virtual ~StepTable();

  QSize                    minimumSizeHint() const;

  void                     setEditable( bool, int, int );
  bool                     isEditable( int, int ) const;

  void                     setReadOnly( bool );
  bool                     isReadOnly() const;

  void                     insertRows( int, int = 1 );
  QString                  text( int, int );

  QList<int>               selectedRows();
  void                     selectRows(const QList<int>& rows);

  void                     setDim( int nbRows, QString shapeName, bool setDefault=true );
  void                     setNbComps( int nbComps );
  void                     setDataType( int dataType );
  void                     setStamp( int stamp ) { myStamp = stamp; }
  int                      getStamp() { return myStamp; }
  int                      getStepID() { return myStepID; }
  QStringList              getHeaders();
  void                     setHeaders(const QStringList& headers);
  GEOM::GEOM_FieldStep_var getStep() { return myStep; }
  void                     setValues(GEOM::GEOM_FieldStep_var& step);

public slots:
  void                     setIsChanged() { myIsChanged = true; }
  void                     headerDblClicked( int );
};

#endif
