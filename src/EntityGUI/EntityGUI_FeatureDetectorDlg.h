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
// File   : EntityGUI_SketcherDlg.cxx
// Author : Renaud NEDELEC, Open CASCADE S.A.S.


#include <GEOMBase_Skeleton.h>
#include <gp_Ax3.hxx>
#include <QBitmap>

class QLineEdit;
class QCheckBox;
class QGroupBox;
class QPushButton;
class QLabel;
class QFrame;
class QPoint;
class DlgRef_3Radio;
class DlgRef_1Sel;
class DlgRef_1Sel1Frame;

class gp_Pnt;
class ShapeRec_FeatureDetector;
class ShapeRec_Parameters;

//=================================================================================
// class    : EntityGUI_Dlg
// purpose  :
//=================================================================================
class EntityGUI_FeatureDetectorDlg : public GEOMBase_Skeleton
{ 
  Q_OBJECT

public:
  EntityGUI_FeatureDetectorDlg(GeometryGUI*, QWidget* = 0, bool = false, Qt::WindowFlags = 0);
  ~EntityGUI_FeatureDetectorDlg();
  
  bool                               acceptMouseEvent() const;
  void                               setStartPnt     ( const gp_Pnt& );
  void                               setEndPnt       ( const gp_Pnt& );
  int                                getConstructorId() {return myConstructorId;};
  
protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       execute( ObjectList& );
  virtual QList<GEOM::GeomObjPtr>    getSourceObjects();
  
private:
  void                               Init();
  bool                               setSelectionRect();
  void                               showImageSample();
  ShapeRec_Parameters*               parametersChanged();

  
private slots:
  void                               SelectionIntoArgument();
  void                               ConstructorsClicked( int );
//   void                               onViewClicked( int );
  void                               onButtonClicked(); 
  void                               onCheckBoxClicked( bool );
  void                               ClickOnOk();
  bool                               ClickOnApply();
  
  
private:
  ShapeRec_FeatureDetector*          myDetector;
  
  gp_Ax3                             myWPlane;
  gp_Ax3                             aGlobalCS;
  
  GEOM::GeomObjPtr                   myFace;
  QString                            myFaceEntry;
  
  int                                myConstructorId;
  
  DlgRef_1Sel1Frame*                 mySelectionGroup; 
  DlgRef_1Sel*                       mySelWidget;

  QCheckBox*                         myUseROI;
  QFrame*                            myCornersParameters;
  QFrame*                            myContoursParameters;
  QFrame*                            myCannyParameters;
  QFrame*                            myColorFilterParameters;
  QList<QWidget*>                    myWidgets;
  
  // Output typeselection widget
  DlgRef_3Radio*                     myOutputGroup;;
  
  gp_Pnt                             myStartPnt;
  gp_Pnt                             myEndPnt;
  QRect                              myRect;
  
  // Picture dimensions and position
  int                                height;
  int                                width;
  double                             pictureLeft;
  double                             pictureTop;
};
