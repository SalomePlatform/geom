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
//  File   : MeasureGUI_AnnotationDlg.h

#ifndef MeasureGUI_AnnotationDlg_H
#define MeasureGUI_AnnotationDlg_H

#include <GEOMBase_Skeleton.h>

#include <GEOM_Annotation.hxx>
#include <GEOM_Constants.h>
#include <GEOMGUI_AnnotationAttrs.h>

#include <TopAbs_ShapeEnum.hxx>
//#include <TColStd_DataMapOfIntegerInteger.hxx>
//#include <TColStd_IndexedMapOfInteger.hxx>
//#include <TopTools_IndexedMapOfShape.hxx>

//#include <QMap>
//#include <QSet>
//#include <QVector>
//#include <QTableWidget>

class QGroupBox;
class QLineEdit;
class QPushButton;
class QComboBox;
class QCheckBox;
class SalomeApp_IntSpinBox;
class MeasureGUI_AnnotationInteractor;

//=================================================================================
// class    : MeasureGUI_AnnotationDlg
// purpose  :
//=================================================================================
class MeasureGUI_AnnotationDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

public:
  MeasureGUI_AnnotationDlg ( GeometryGUI* theGeometryGUI,
                             const bool theIsCreate,
                             QWidget* parent=0,
                             bool modal=false, Qt::WindowFlags fl=0 );
  ~MeasureGUI_AnnotationDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual                             GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                        isValid ( QString& );
  virtual bool                        execute ();

  SALOME_Prs*                         buildPrs();
  void                                updateSubShapeEnableState();
  void                                redisplayPreview();

  void                                closeEvent( QCloseEvent* theEv );

private slots:
  void                                ClickOnOk();
  bool                                ClickOnApply();
  void                                SelectionIntoArgument();
  void                                SetEditCurrentArgument();

  void                                onTextChange();
  void                                onTypeChange();
  void                                onSubShapeTypeChange();

  void                                onDragged( Handle_GEOM_Annotation theAnnotation );

private:
  void                                Init();

  void                                activateSelectionArgument( QPushButton* theSelectionButton );
  void                                activateSelection();
  TopAbs_ShapeEnum                    getShapeType() const;

  bool                                getPickedPoint( gp_Pnt& thePnt, const TopoDS_Shape& theShape );
  gp_Pnt                              getAttachPoint( const TopoDS_Shape& theShape, bool& theIsOk );
  gp_Pnt                              getDefaultPosition( const gp_Pnt& theAttach );

private:
  TopAbs_ShapeEnum                    mySelectionMode;
  QString                             myEditAnnotationEntry;
  int                                 myEditAnnotationIndex;
  GEOMGUI_AnnotationAttrs::Properties myAnnotationProperties;
  bool                                myIsPositionDefined;
  /// an index of edited annotation in the list shape annotations, -1 in create operation
  bool                                myIsCreation;
  GEOM::GeomObjPtr                    myShape;

  QLineEdit*                          myTextEdit;

  QPushButton*                        myShapeSelBtn;
  // update shape name by shape selection if it has not been manually modified yet
  bool                                myShapeNameModified;
  QLineEdit*                          myShapeName;
  QCheckBox*                          myIsScreenFixed;

  QComboBox*                          mySubShapeTypeCombo;

  QPushButton*                        mySubShapeSelBtn;
  QLineEdit*                          mySubShapeName;

  MeasureGUI_AnnotationInteractor*    myInteractor;
};

#endif
