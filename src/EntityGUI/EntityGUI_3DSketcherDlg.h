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

// GEOM GEOMGUI : GUI for Geometry component
// File   : EntityGUI_3DSketcherDlg.h
// Author : DMV, OCN

#ifndef ENTITYGUI_3DSKETCHERDLG_H
#define ENTITYGUI_3DSKETCHERDLG_H

#include <GEOMBase_Skeleton.h>

class QButtonGroup;
class QDoubleSpinBox;
class EntityGUI_3Spin;
class EntityGUI_Type;
class EntityGUI_Angles;
class EntityGUI_Controls;
class SOCC_Prs;
class gp_Dir;

#include <AIS_ListOfInteractive.hxx>
#include <AIS_AngleDimension.hxx>
#include <AIS_LengthDimension.hxx>

#ifndef COORD_MIN
#  define COORD_MIN -1e+15
#  define COORD_MAX +1e+15
#  define MAX_NUMBER 100000
#  define DBL_DIGITS_DISPLAY 16
#endif // COORD_MIN

// For convenience
bool isSame (double d1, double d2);

//=================================================================================
// class    : EntityGUI_Dlg
// purpose  :
//=================================================================================
class EntityGUI_3DSketcherDlg : public GEOMBase_Skeleton
{ 
  Q_OBJECT

  struct XYZ
  {
    XYZ() { x = y = z = 0.0; command = params = ""; L=A=T=0; }
    double  x,  y,  z; // for preview only
    int L, A, T;       // for preview only
    QString command;
    QString params;
  };
  
  struct prsType
  {
    prsType(){L=A=T=0;}
    int L;
    int A;
    int T;
  };
  
  typedef QList<XYZ> XYZList;

public:
  EntityGUI_3DSketcherDlg (GeometryGUI*, QWidget* = 0, bool = false, Qt::WindowFlags = 0, const double = 2.);
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
  
  Handle(AIS_LengthDimension)        createAISLengthDimension(double theLength, 
                                                              gp_Pnt P1, 
                                                              gp_Pnt P2,
                                                              gp_Dir theNormal);  
  
  Handle(AIS_AngleDimension)         createAISAngleDimension(double theAngle, 
                                                             gp_Pnt P0, 
                                                             gp_Pnt P1,
                                                             gp_Pnt P2);
  
  void                               displayAngle(double theAngle, 
                                                  gp_Pnt P0, 
                                                  gp_Pnt P1,
                                                  gp_Pnt P2, 
                                                  bool store = false);
  
  void                               displayLength(gp_Pnt P1,
                                                   gp_Pnt P2,
                                                   gp_Dir theNormal,
                                                   bool store = false);
  
  void                               displayText(std::string theText,
                                                 gp_Pnt P,
                                                 bool store = false);
  
  void                               displayTrihedron( int );
  
  void                               displayDimensions(bool store = false);

  bool                               createShapes( GEOM::GEOM_Object_ptr,
                                                   TopoDS_Shape&,
                                                   TopoDS_Shape& );

  XYZ                                getLastPoint() const;
  XYZ                                getPenultimatePoint() const;
  XYZ                                getCurrentPoint() const;
  
  gp_Dir                             getPresentationPlane() const;
  
  std::string                        doubleToString( double );
  
  gp_Trsf                            toReferenceSystem(gp_Pnt origin) const;
  gp_Trsf                            toCurrentSystem(gp_Pnt origin) const;
  
  void                               removeLastIOFromPrs();
  void                               restoreLastIOToPrs();
  
  double                             radius(gp_Pnt) const;
  double                             height(gp_Pnt) const;
  double                             longitude(gp_Pnt) const;
  double                             latitude(gp_Pnt) const;

private:
  XYZList                            myPointsList;
  XYZList                            myRedoList;
  AIS_ListOfInteractive              myLengthIORedoList;
  AIS_ListOfInteractive              myAngleIORedoList;
  AIS_ListOfInteractive              myTextIORedoList;
  prsType                            myPrsType;

  EntityGUI_3Spin*                   Group3Spin;
  EntityGUI_Angles*                  GroupAngles;
  EntityGUI_Controls*                GroupControls;
  EntityGUI_Type*                    GroupType;
  QButtonGroup*                      myTypeGroup1;
  QButtonGroup*                      myTypeGroup2;

  int                                myMode;
  int                                myCoordType;
  int                                myOrientation;
  bool                               myOK;
  bool                               isLengthVisible;
  bool                               isAngleVisible;
  
  double                             myLineWidth;
  GeometryGUI*                       myGeometryGUI;
  SOCC_Prs*                          myAnglePrs;
  SOCC_Prs*                          myLengthPrs;
  SOCC_Prs*                          myTextPrs;
  bool                               myIsUndoRedo;
  XYZ                                myWorkPoint;

private slots:
  void                               ClickOnOk();
  void                               ClickOnAddPoint();
  bool                               ClickOnApply();
  //  bool                               isSameAsPrevious();
  void                               UpdateButtonsState();
  void                               UpdatePointCoordinates();

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
