// GEOM GEOMGUI : GUI for Geometry component
//
// Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
// File   : EntityGUI_3DSketcherDlg.h
// Author : DMV, OCN
//

#ifndef ENTITYGUI_3DSKETCHERDLG_H
#define ENTITYGUI_3DSKETCHERDLG_H

#include <GEOMBase_Skeleton.h>

class QDoubleSpinBox;
class EntityGUI_3Spin;
class DlgRef_3Radio;

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
class EntityGUI_3DSketcherDlg : public GEOMBase_Skeleton
{ 
  Q_OBJECT

public:
  EntityGUI_3DSketcherDlg( GeometryGUI*, QWidget* = 0, bool = false, Qt::WindowFlags = 0, const double = 2. );
  ~EntityGUI_3DSketcherDlg();

protected:
  void                               initSpinBox( QDoubleSpinBox*, 
						  double, double, double = 0.1, 
						  int = 3 );

  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid( QString& );
  virtual bool                       execute( ObjectList& );

private:
  void                               Init();
  void                               enterEvent( QEvent* );

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
  QList<double>                      myPointsList;
  QList<double>                      myRedoList;

  EntityGUI_3Spin*                   Group3Spin;
  DlgRef_3Radio*                     GroupType;

  bool                               myOK;
  double                             myLineWidth;
  GeometryGUI*                       myGeometryGUI;

private slots:
  void                               ClickOnOk();
  void                               ClickOnAddPoint();
  bool                               ClickOnApply();
  //  bool                               isSameAsPrevious();
  void                               UpdateButtonsState();
  void                               GetLastPoints(double&, double&, double&);
  void                               GetCurrentPoints(double&, double&, double&);

  void                               ClickOnUndo();
  void                               ClickOnRedo();

  void                               SelectionIntoArgument();
  void                               DeactivateActiveDialog();
  void                               ActivateThisDialog();
  void                               TypeClicked();
  void                               ValueChangedInSpinBox( double );
  void                               SetDoubleSpinBoxStep( double );
};

#endif // ENTITYGUI_SKETCHERDLG_H
