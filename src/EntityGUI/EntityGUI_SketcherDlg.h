//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// GEOM GEOMGUI : GUI for Geometry component
// File   : EntityGUI_SketcherDlg.h
// Author : Damien COQUERET, Open CASCADE S.A.S.
//
#ifndef ENTITYGUI_SKETCHERDLG_H
#define ENTITYGUI_SKETCHERDLG_H

#include <GEOMBase_Helper.h>

#include <QDialog>

class QLineEdit;
class SalomeApp_DoubleSpinBox;
class EntityGUI_1Sel;
class EntityGUI_1Spin;
class EntityGUI_2Spin;
class EntityGUI_3Spin;
class EntityGUI_4Spin;
class EntityGUI_Point;
class EntityGUI_Dir1;
class EntityGUI_Dir2;
class EntityGUI_Skeleton;
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
			 const double = 2. );
  ~EntityGUI_SketcherDlg();

  bool eventFilter (QObject* object, QEvent* event);

protected:
  void                               initSpinBox( SalomeApp_DoubleSpinBox*, 
						  double, double, double = 0.1, 
						  int = 3 );

  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid( QString& );
  virtual bool                       execute( ObjectList& );

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

  bool                               createShapes( GEOM::GEOM_Object_ptr,
						   TopoDS_Shape&,
						   TopoDS_Shape& );

private:
  int                                myConstructorId;
  int                                myConstructorDirId;
  int                                mySketchType;
  int                                mySketchState;

  bool                               myIsAllAdded;
  bool                               myIsApply;

  QLineEdit*                         myEditCurrentArgument;   /* Current LineEdit */

  QStringList                        myCommand;
  QStringList                        myUndoCommand;

  QStringList                        myParameters;
  QStringList                        myUndoParameters;

  Standard_Real                      myX, myY, myDX, myDY;
  Standard_Real                      myLength, myAngle, myRadius;
  Standard_Real                      myLastX1, myLastY1;
  Standard_Real                      myLastX2, myLastY2;

  QString                            myXStr, myYStr, myDXStr, myDYStr;
  QString                            myLengthStr, myAngleStr, myRadiusStr;
  QString                            myLastX1Str, myLastY1Str;
  QString                            myLastX2Str, myLastY2Str;                            

  EntityGUI_Skeleton*                MainWidget;

  EntityGUI_Point*                   GroupPt;
  EntityGUI_Dir1*                    GroupD1;
  EntityGUI_Dir2*                    GroupD2;

  EntityGUI_1Sel*                    Group1Sel;
  EntityGUI_1Spin*                   Group1Spin;
  EntityGUI_2Spin*                   Group2Spin;
  EntityGUI_3Spin*                   Group3Spin;
  EntityGUI_4Spin*                   Group4Spin;

  GeometryGUI*                       myGeometryGUI;

  QString                            myHelpFileName;
  
  double                             myLineWidth;

private:
  enum SketchState { FIRST_POINT, NEXT_POINT };
  
  enum SketchType { PT_ABS, PT_RELATIVE, PT_SEL,
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
  void                               DestClicked( int );
  void                               PointClicked( int );
  void                               Dir1Clicked( int );
  void                               Dir2Clicked( int );
  void                               ValueChangedInSpinBox( double );
  void                               SetDoubleSpinBoxStep( double );
};

#endif // ENTITYGUI_SKETCHERDLG_H
