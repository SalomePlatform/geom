// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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

// GEOM GEOMGUI : GUI for Geometry component
// File   : GEOM_Displayer.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#ifndef GEOM_DISPLAYER_H
#define GEOM_DISPLAYER_H

#include "GEOM_GEOMGUI.hxx"

#include <SALOME_InteractiveObject.hxx>

class SALOME_ListIO;
class SALOME_View;
class SALOME_Prs;
class SALOME_OCCPrs;
class SALOME_VTKPrs;
class SALOME_OCCViewType;

#include <TopoDS_Shape.hxx>
#include <Quantity_Color.hxx>
#include <LightApp_Displayer.h>
#include <LightApp_Study.h>
#include <Aspect_TypeOfMarker.hxx>

#include <QList>

#include <list>

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(GEOM_Gen)

#define GEOM_ALLOBJECTS -1 // Selection of all objects is activated
#define GEOM_PREVIEW    -2 // Definition for preview selection
#define GEOM_ALLSHAPES  -3 // Selection of all shapes is activated
#define GEOM_ALLGEOM    -4 // Selection of all geom objects is activated

typedef std::list<GEOM::GEOM_Object_ptr> ObjectList;

class TColStd_MapOfInteger;
class LightApp_SelectionMgr;
class SalomeApp_Study;
class SalomeApp_Application;
class SUIT_SelectionFilter;
//class SALOME_Selection;

class GEOMGUI_EXPORT GEOM_Displayer : public LightApp_Displayer
{

public:
  /* Constructor */
  GEOM_Displayer( SalomeApp_Study* app );
  /* Destructor */
  virtual ~GEOM_Displayer();

  virtual bool canBeDisplayed( const QString& /*entry*/, const QString& /*viewer_type*/ ) const;

  /* Display/Erase object methods */
  void          Display   ( const Handle(SALOME_InteractiveObject)& theIO,
                            const bool updateViewer = true,
                            SALOME_View* theViewFrame = 0 );

  // This overloaded Display() method can be useful for operations
  // not using dialog boxes.
  void          Display   ( GEOM::GEOM_Object_ptr theObj,
                            const bool updateViewer = true );

  void          Redisplay ( const Handle(SALOME_InteractiveObject)& theIO,
                            const bool updateViewer = true );

  void          Erase     ( const Handle(SALOME_InteractiveObject)& theIO,
                            const bool forced = false,
                            const bool updateViewer = true,
                            SALOME_View* theViewFrame = 0 );

  void          Erase     ( GEOM::GEOM_Object_ptr theObj,
                            const bool forced = false,
                            const bool updateViewer = true );

  void          EraseWithChildren(const Handle(SALOME_InteractiveObject)& theIO,
				  const bool eraseOnlyChildren = false);

  /* Display/Erase list of objects methods */

  void          Display   ( const SALOME_ListIO& theIOList,
                            const bool updateViewer = true );

  void          Erase     ( const SALOME_ListIO& theIOList,
                            const bool forced = false,
                            const bool updateViewer = true );

  void          Redisplay ( const SALOME_ListIO& theIOList,
                            const bool updateViewer = true );

  /* build presentation accordint to the current viewer type*/
  SALOME_Prs*   BuildPrs  ( GEOM::GEOM_Object_ptr );
  SALOME_Prs*   BuildPrs  ( const TopoDS_Shape& );

  /* Set color for shape displaying. If it is equal -1 then default color is used.
     Available values are from Quantity_NameOfColor enumeration */
  void          SetColor  ( const int );
  void          UnsetColor();
  int           GetColor  () const;
  bool          HasColor  () const;
  
  /* Set texture for shape displaying. */
  void          SetTexture  ( const std::string& );
  bool          HasTexture  () const;
  std::string   GetTexture  () const;

  /* Set width for shape displaying. If it is equal -1 then default width is used. */
  void          SetWidth  ( const double );
  void          UnsetWidth();
  double        GetWidth  () const;
  bool          HasWidth  () const;
  
  /* Set display mode shape displaying. If it is equal -1 then display mode is used. */
  int           SetDisplayMode( const int );
  int           GetDisplayMode() const;
  int           UnsetDisplayMode();


  /* Sets name - for temporary objects only */
  void          SetName( const char* theName );
  void          UnsetName();

  /* Reimplemented from SALOME_Displayer */
  virtual void  Update( SALOME_OCCPrs* );
  virtual void  Update( SALOME_VTKPrs* );
  virtual void  BeforeDisplay( SALOME_View*, const SALOME_OCCPrs* );
  virtual void  AfterDisplay ( SALOME_View*, const SALOME_OCCPrs* );

  /* This methos is used for activisation/deactivisation of objects to be displayed*/
  void          SetToActivate( const bool );
  bool          ToActivate() const;

  /* Activate/Deactivate selection*/
  void         LocalSelection( const Handle(SALOME_InteractiveObject)&, const int );
  void         LocalSelection( const SALOME_ListIO& theIOList, const int );
  void         GlobalSelection( const int = GEOM_ALLOBJECTS, const bool = false );
  void         GlobalSelection( const TColStd_MapOfInteger&, const bool = false, const QList<int>* = 0 );

  SalomeApp_Study* getStudy() const;

  static SALOMEDS::Color getUniqueColor( const QList<SALOMEDS::Color>& );

  static PropMap getDefaultPropertyMap(const QString& viewer_type);
  
  static bool MergePropertyMaps(PropMap& theOrigin, PropMap& theDefault);
  
  /*Get color of the geom object*/
  static SALOMEDS::Color getColor(GEOM::GEOM_Object_var aGeomObject, bool& hasColor);

  /* Get minimum or maximum enclosed shape type */
  static int getMinMaxShapeType( const TopoDS_Shape& shape, bool ismin );

  /* Check if the object is a vertex or a compound of vertices */
  static bool isCompoundOfVertices( const TopoDS_Shape& theShape );


  /* Builds presentation of not published object */
  virtual SALOME_Prs* buildSubshapePresentation(const TopoDS_Shape& aShape,
                                                const QString&,
                                                SALOME_View* = 0);

protected:
  /* internal methods */
  /* Builds presentation according to the current viewer type */
  virtual SALOME_Prs* buildPresentation( const QString&, SALOME_View* = 0 );

  /* Sets interactive object */
  void        setIO( const Handle(SALOME_InteractiveObject)& theIO );

  /* Sets shape */
  void        setShape( const TopoDS_Shape& theShape );

  /* Resets internal data */
  void        internalReset();

  void        clearTemporary( LightApp_SelectionMgr* theSelMgr );

  SUIT_SelectionFilter* getFilter( const int theMode );
  SUIT_SelectionFilter* getComplexFilter( const QList<int>* );

protected:
  Handle(SALOME_InteractiveObject) myIO;
  TopoDS_Shape                     myShape;
  std::string                      myName;
  std::string                      myTexture;
  int                              myType;
  SALOME_View*                     myViewFrame;

  // Attributes
  Quantity_Color                   myShadingColor;
  int                              myColor;
  double                           myWidth;
  bool                             myToActivate;
  int                              myDisplayMode;
  Aspect_TypeOfMarker              myTypeOfMarker;
  double                           myScaleOfMarker;

private:
  SalomeApp_Application* myApp;
};

#endif // GEOM_DISPLAYER_H

