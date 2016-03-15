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
// File   : EntityGUI_SketcherDlg.h
// Author : Damien COQUERET, Open CASCADE S.A.S.
//
#ifndef ENTITYGUI_SKETCHERDLG_H
#define ENTITYGUI_SKETCHERDLG_H

#include <GEOMBase_Helper.h>

#include <QGroupBox>
#include <QComboBox>
#include <QDialog>
#include <QLabel>

#include <gp_Ax3.hxx>

class QLineEdit;
class SalomeApp_DoubleSpinBox;
class EntityGUI_1Sel;
class EntityGUI_2Sel1Check;
class EntityGUI_1Sel1Spin1Check;
class EntityGUI_1Spin;
class EntityGUI_2Spin;
class EntityGUI_3Spin1Check;
class EntityGUI_4Spin1Check;
class EntityGUI_Point;
class EntityGUI_Dir1;
class EntityGUI_Dir2;
class EntityGUI_Skeleton;
class DlgRef_4Spin;
class GeometryGUI;

#ifndef COORD_MIN
#  define COORD_MIN -1e+15
#  define COORD_MAX +1e+15
#  define MAX_NUMBER 100000
#  define DBL_DIGITS_DISPLAY 16
#endif // COORD_MIN

//=================================================================================
// class    : EntityGUI_Dlg
// purpose  :
//=================================================================================
class EntityGUI_SketcherDlg : public QDialog, GEOMBase_Helper
{ 
  Q_OBJECT

public:
  EntityGUI_SketcherDlg( GeometryGUI*, QWidget* = 0, bool = false, Qt::WindowFlags = 0,
                         const double = -1 );
  ~EntityGUI_SketcherDlg();

  bool eventFilter (QObject* object, QEvent* event);
  
  bool                               acceptMouseEvent() const;  // called by EntityGUI::OnMousePress()
  void                               OnPointSelected( Qt::KeyboardModifiers, const gp_Pnt&, bool isStart = true );             

protected:
  void                               initSpinBox( SalomeApp_DoubleSpinBox*, 
                                                  double, double, double = 0.1, 
                                                  const char* quantity = "length_precision" );

  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid( QString& );
  virtual bool                       execute( ObjectList& );
  virtual void                       addSubshapesToStudy();
  
  void                               closeEvent( QCloseEvent* );
  void                               keyPressEvent( QKeyEvent* );

private:
  void                               Init();
  void                               enterEvent( QEvent* );
  void                               InitClick();
  void                               setEnabledUndo( bool );
  void                               setEnabledRedo( bool );
    
  QString                            GetNewCommand( QString& );

  virtual void                       displayPreview( GEOM::GEOM_Object_ptr,
                                                     const bool = false,
                                                     const bool = false,
                                                     const bool = true,
                                                     const double = -1,
                                                     const int  = -1,
                                                     const int  = -1);
  
  void                               displayPntPreview( const double = 0.0,
                                                        const double = 0.0,
                                                        bool /*append*/ = true,
                                                        bool /*update*/ = true
                                                      );

  bool                               createShapes( GEOM::GEOM_Object_ptr,
                                                   TopoDS_Shape&,
                                                   TopoDS_Shape& );
  int                                getPnt1ConstructorId() const;
  int                                getPnt2ConstructorId() const;
  void                               OnFirstPoint();
  void                               OnNextPoint(); 


private:
  int                                myConstructorId;
  int                                myConstructorDirId;
  int                                myConstructorPntId;
  int                                mySketchType;
  int                                mySketchState;

  bool                               myIsAllAdded;
  bool                               myIsApply;
  bool                               autoApply;

  QLineEdit*                         myEditCurrentArgument;   /* Current LineEdit */

  QStringList                        myCommand;
  QStringList                        myUndoCommand;

  QStringList                        myParameters;
  QStringList                        myUndoParameters;

  Standard_Real                      myX, myY, myDX, myDY;
  Standard_Real                      myX1, myY1, myX2, myY2;
  Standard_Real                      myXc, myYc, myDXc, myDYc;
  Standard_Real                      myLength, myAngle, myRadius;
  Standard_Real                      myLastX1, myLastY1;
  Standard_Real                      myLastX2, myLastY2;

  QString                            myXStr, myYStr, myDXStr, myDYStr;
  QString                            myX1Str, myY1Str, myX2Str, myY2Str;
  QString                            myXcStr, myYcStr, myDXcStr, myDYcStr;
  QString                            myLengthStr, myAngleStr, myRadiusStr;
  QString                            myLastX1Str, myLastY1Str;
  QString                            myLastX2Str, myLastY2Str;                            

  EntityGUI_Skeleton*                MainWidget;

  EntityGUI_Point*                   GroupPt;
  EntityGUI_Point*                   GroupPt2;
  EntityGUI_Dir1*                    GroupD1;
  EntityGUI_Dir2*                    GroupD2;

  EntityGUI_1Sel*                    Group1Sel;
  EntityGUI_2Sel1Check*              Group2Sel;
  EntityGUI_1Sel1Spin1Check*         Group1Sel1Spin; 
  EntityGUI_1Spin*                   Group1Spin;
  EntityGUI_2Spin*                   Group2Spin;
  EntityGUI_3Spin1Check*             Group3Spin;
  EntityGUI_4Spin1Check*             Group4Spin;
  DlgRef_4Spin*                      GroupRect;

  QLabel*                            myErrLabel;

  QGroupBox*                         GroupBox1;
  QComboBox*                         ComboBox1;
  QPushButton*                       planeButton;
  QPushButton*                       selButton;
  
  QLineEdit*                         WPlaneLineEdit;

  GeometryGUI*                       myGeometryGUI;

  QString                            myHelpFileName;
  
  double                             myLineWidth;

  QList<gp_Ax3>                      myLCSList;
  GEOM::GEOM_Object_var              myGlobalCS;
  GEOM::GEOM_Object_var              myWPlane;
  QList< GEOM::GEOM_Object_var >     myWPlaneList;

  int                                myCheckFlag;
  
  TopAbs_ShapeEnum                   myNeedType;

private:
  enum SketchState { FIRST_POINT, NEXT_POINT };
  
  enum SketchType { PT_ABS, PT_RELATIVE, PT_SEL,
                    PT_ABS_RADIUS,PT_REL_RADIUS,PT_SEL_RADIUS,
                    PT_ABS_CENTER,PT_REL_CENTER,PT_SEL_CENTER,
                    DIR_ANGLE_LENGTH, DIR_ANGLE_X, DIR_ANGLE_Y,
                    DIR_PER_LENGTH, DIR_PER_X, DIR_PER_Y,
                    DIR_TAN_LENGTH, DIR_TAN_X, DIR_TAN_Y,
                    DIR_DXDY_LENGTH, DIR_DXDY_X, DIR_DXDY_Y };
  
private slots:
  void                               ClickOnEnd();
  void                               ClickOnCancel();
  bool                               ClickOnApply();
  void                               ClickOnUndo();
  void                               ClickOnRedo();
  void                               ClickOnHelp();
  void                               LineEditReturnPressed();
  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();
  void                               DeactivateActiveDialog();
  void                               ActivateThisDialog();
  void                               TypeClicked( int );
  void                               RectClicked();
  void                               DestClicked( int );
  void                               PointClicked( int );
  void                               Point2Clicked( int );
  void                               Dir1Clicked( int );
  void                               Dir2Clicked( int );
  void                               CheckBoxClicked( int );
  void                               ValueChangedInSpinBox( double );
  void                               SetDoubleSpinBoxStep( double );
  void                               AddLocalCS( GEOM::GEOM_Object_var );
  void                               FindLocalCS();
  gp_Ax3                             GetActiveLocalCS();
  gp_Ax3                             WPlaneToLCS( GEOM::GEOM_Object_var );
  void                               ActivateLocalCS();
};

#endif // ENTITYGUI_SKETCHERDLG_H
