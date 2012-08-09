// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

// GEOM GEOMGUI : GUI for Geometry component
// File   : EntityGUI_3DSketcherDlg.h
// Author : DMV, OCN
//
#ifndef ENTITYGUI_3DSKETCHERDLG_H
#define ENTITYGUI_3DSKETCHERDLG_H

#include <GEOMBase_Skeleton.h>

class QButtonGroup;
class QDoubleSpinBox;
class EntityGUI_3Spin;
class EntityGUI_Angles;
class EntityGUI_Controls;
class DlgRef_3Radio;
class SOCC_Prs;

#include <AIS_ListOfInteractive.hxx>

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

  struct XYZ
  {
    XYZ() { x = y = z = 0.0; xt = yt = zt = "0.0"; }
    double  x,  y,  z;
    QString xt, yt, zt;
  };
  typedef QList<XYZ> XYZList;

public:
  EntityGUI_3DSketcherDlg( GeometryGUI*, QWidget* = 0, bool = false, Qt::WindowFlags = 0, const double = 2. );
  ~EntityGUI_3DSketcherDlg();

protected:
 
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
  
  void                               displayTrihedron( int );
  
  void                               displayAngle( double, double, double, int, bool store = false );
  void                               displayLength( double theLength = -1.0, bool store = false );

  bool                               createShapes( GEOM::GEOM_Object_ptr,
                                                   TopoDS_Shape&,
                                                   TopoDS_Shape& );

  XYZ                                getLastPoint() const;
  XYZ                                getPenultimatePoint() const;
  XYZ                                getCurrentPoint() const;
  
  std::string                        doubleToString( double );
  
  void                               removeLastIOFromPrs( int );
  void                               restoreLastIOToPrs( int );

private:
  XYZList                            myPointsList;
  XYZList                            myRedoList;
  AIS_ListOfInteractive              myLengthIORedoList;
  AIS_ListOfInteractive              myAngleIORedoList;
  int                                myPrsType;
  std::list<int>                     myPrsTypeList;
  std::list<int>                     myPrsTypeRedoList;

  EntityGUI_3Spin*                   Group3Spin;
  EntityGUI_Angles*                  GroupAngles;
  EntityGUI_Controls*                GroupControls;
  DlgRef_3Radio*                     GroupType;
  QButtonGroup*                      myTypeGroup;

  int                                myMode;
  int                                myOrientation;
  bool                               myOK;
  bool                               isLengthVisible;
  bool                               isAngleVisible;
  
  double                             myLineWidth;
  GeometryGUI*                       myGeometryGUI;
  SOCC_Prs*                          myAnglePrs;
  SOCC_Prs*                          myLengthPrs;

private slots:
  void                               ClickOnOk();
  void                               ClickOnAddPoint();
  bool                               ClickOnApply();
  //  bool                               isSameAsPrevious();
  void                               UpdateButtonsState();

  void                               ClickOnUndo();
  void                               ClickOnRedo();

  void                               SelectionIntoArgument();
  void                               DeactivateActiveDialog();
  void                               ActivateThisDialog();
  void                               TypeClicked( int );
  void                               ValueChangedInSpinBox( double );
  void                               ButtonClicked( bool );
  void                               BoxChecked ( bool );
  void                               SetDoubleSpinBoxStep( double );
};

#endif // ENTITYGUI_SKETCHERDLG_H
