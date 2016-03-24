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

#ifndef ENTITYGUI_POLYLINEDLG_H
#define ENTITYGUI_POLYLINEDLG_H

#include <GEOMBase_Skeleton.h>

class CurveCreator_Curve;
class CurveCreator_Widget;
class OCCViewer_ViewManager;
class QGroupBox;
class QComboBox;

//=================================================================================
// class    : EntityGUI_PolylineDlg
// purpose  :
//=================================================================================
class EntityGUI_PolylineDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

public:

  EntityGUI_PolylineDlg (GeometryGUI*, QWidget* = 0,
                         bool = false, Qt::WindowFlags = 0);

  virtual ~EntityGUI_PolylineDlg();

  void  deleteSelected();
  bool  deleteEnabled();

  void  setPreviewZLayer( int theLayer );
  int   getPreviewZLayer() const;

protected:

  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid( QString& );
  virtual bool                       execute( ObjectList& );
  virtual QList<GEOM::GeomObjPtr>    getSourceObjects();

  /**
   * This method sets/gets the view manager to control the temporary
   * displayed objects on Z layer. 
   * \param theManager the view manager.
   */
  void                   setPreviewManager( OCCViewer_ViewManager* theManager );
  OCCViewer_ViewManager* getPreviewManager();

  /**
   * This method defines a state of selection button.
   * \return true if selection button is checked, otherwise false.
   */
  bool isCheckToSelect();

private:

  void Init();
  void Clear();
  void enterEvent(QEvent *);

  /**
   * This method converts the curve into curve parameters required to
   * construct an object using the interface
   * GEOM_ICurvesOperations::MakePolyline2DOnPlane.
   *
   * \param theCurve a curve object, that contains data.
   *  \param theCoordsList the list of coordinates list. theCoordsList[0]
   *         is the coordinates list of the first section. theCoordsList[1]
   *         is for the second section etc. Output parameter.
   *  \param theNamesList the list of names. The order corresponds to
   *         theCoordsList. Output parameter.
   *  \param theTypesList the list of curve types. The order corresponds to
   *         theCoordsList. Output parameter.
   *  \param theClosedList the list of Closed flags. The order corresponds to
   *         theCoordsList. Output parameter.
   */
  void GetCurveParams(GEOM::ListOfListOfDouble &theCoords,
                      GEOM::string_array       &theNames,
                      GEOM::short_array        &theTypes,
                      GEOM::ListOfBool         &theCloseds);

  /**
   * This method returns the current local coordinate system.
   *
   * \return local coordinate system.
   */
  gp_Ax3 GetActiveLocalCS();

  /**
   * This method returns the current working plane. Can be null.
   *
   * \return the current working plane.
   */
  GEOM::GeomObjPtr GetActiveWPlane();

  /**
   * This method add a local coordinate system of the selected object.
   *
   * \param theSelectedObject the selected object. It can be a planar face
   *        or an imported polyline.
   * \param IsPlane true for planar face; false for imported polyline.
   * \param theLCS the local coordinate system.
   */
  void AddLocalCS(GEOM::GeomObjPtr  theSelectedObject,
                  const bool        IsPlane,
                  const gp_Ax3      &theLCS);

  /**
   * This method converts the working plane object into
   * the local coordinate system of the polyline.
   *
   * \param theGeomObj the working plane
   * \return the local coordinate system
   */
  gp_Ax3 WPlaneToLCS(GEOM::GeomObjPtr theGeomObj);

  /**
   * This method displays the AIS_InteractiveObject(s) to preview
   * on the Z layer and sets VIOLET color.
   */
  void displayPreview();

  /**
   * This method erases AIS_InteractiveObject(s) from
   * AIS_InteractiveContext and release memory.
   */
  void erasePreview();

protected slots:

  void ClickOnOk();
  bool ClickOnApply();
  void processStartedSubOperation( QWidget*, bool );
  void processFinishedSubOperation( QWidget* );
  void SetEditCurrentArgument( bool );
  void SelectionIntoArgument( bool isForced = false );
  void ActivateThisDialog();
  void ActivateLocalCS();

private:

  CurveCreator_Curve           *myCurve;
  CurveCreator_Widget          *myEditorWidget;
  QGroupBox                    *myAddElementBox;
  QComboBox                    *myPlnComboBox;
  QPushButton                  *myPlnButton;
  QPushButton                  *myPlnSelButton;
  QPushButton                  *myPolylineSelButton;
  QLineEdit                    *myWPlaneLineEdit;
  QLineEdit                    *myPolylineEdit;
  QLineEdit                    *myEditCurrentArgument;   /* Current LineEdit */
  QList<gp_Ax3>                 myLCSList;
  QList<GEOM::GeomObjPtr>       myWPlaneList;
  OCCViewer_ViewManager*        myPreviewManager;
  int                           myPreviewZLayer;

};

#endif // ENTITYGUI_POLYLINEDLG_H
