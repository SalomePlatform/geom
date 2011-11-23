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
// File   : EntityGUI_SketcherDlg.cxx
// Author : Renaud NEDELEC, Open CASCADE S.A.S.


#include <GEOMBase_Skeleton.h>
#include <gp_Ax3.hxx>
#include <ShapeRec_FeatureDetector.hxx>
#include <QBitmap>

class QLineEdit;
class QCheckBox;
class QGroupBox;
class QPushButton;
class QLabel;
class QPoint;
class DlgRef_3Radio;
class DlgRef_1Sel;

class gp_Pnt;

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
//   void                               OnPointSelected( const gp_Pnt& );
  void                               setStartPnt    ( const gp_Pnt& );
  void                               setEndPnt      ( const gp_Pnt& );
  int                                getConstructorId() {return myConstructorId;};
  
protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       execute( ObjectList& );
  
private:
  void                               Init();
  void                               showImageSample();

  
private slots:
  void                               SetEditCurrentArgument();
  void                               SelectionIntoArgument();
  void                               ConstructorsClicked( int );
//   void                               onViewClicked( int );
  void                               onButtonToggled( bool ); 
  void                               ClickOnOk();
  bool                               ClickOnApply();
  
  
private:
  ShapeRec_FeatureDetector*          aDetector;
  
  GEOM::GeomObjPtr                   myFace;
  QString                            myFaceEntry;
  
  int                                myConstructorId;
  
  QGroupBox*                         mySelectionGroup;
  
  QLabel*                            mySnapshotLabel;
  QLabel*                            myImgSampleLabel;
  
  DlgRef_1Sel*                       mySelWidget;
  
  gp_Pnt                             myStartPnt;
  gp_Pnt                             myEndPnt;
  
  gp_Ax3                             myWPlane;
  gp_Ax3                             aGlobalCS;
  
  QPushButton*                       myPushButton;
  QPushButton*                       mySelButton;
  
  QLineEdit*                         myLineEdit;
  
  DlgRef_3Radio*                     myOutputGroup;;

};
