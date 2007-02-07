//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : GEOM_Displayer.h
//  Author : Vadim SANDLER
//  Module : GEOM
//  $Header$

#if !defined (__GEOM_DISPLAYER_H)
#define __GEOM_DISPLAYER_H

#include "GEOM_GEOMGUI.hxx"

#include "SALOME_Prs.h"
#include "SALOME_InteractiveObject.hxx"
#include "SALOME_ListIO.hxx"
#include <TopoDS_Shape.hxx>
#include <Quantity_Color.hxx>
#include <LightApp_Displayer.h>

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

  /* Set width for shape displaying. If it is equal -1 then default width is used. */
  void          SetWidth  ( const double );
  void          UnsetWidth();
  double        GetWidth  () const;
  bool          HasWidth  () const;


  /* Sets name - for temporary objects only */
  void          SetName( const char* theName );
  void          UnsetName();

  /* Reimplemented from SALOME_Displayer */
  virtual void  Update( SALOME_OCCPrs* );
  virtual void  Update( SALOME_VTKPrs* );
  virtual void  BeforeDisplay( SALOME_View*, const SALOME_OCCViewType& );
  virtual void  AfterDisplay ( SALOME_View*, const SALOME_OCCViewType& );

  /* This methos is used for activisation/deactivisation of objects to be displayed*/
  void          SetToActivate( const bool );
  bool          ToActivate() const;

  /* Activate/Deactivate selection*/
  void         LocalSelection( const Handle(SALOME_InteractiveObject)&, const int );
  void         LocalSelection( const SALOME_ListIO& theIOList, const int );
  void         GlobalSelection( const int = GEOM_ALLOBJECTS, const bool = false );
  void         GlobalSelection( const TColStd_MapOfInteger&, const bool = false );

  SalomeApp_Study* getStudy() const;

protected:
  /* internal methods */
  /* Builds presentation accordint to the current viewer type */
  virtual SALOME_Prs* buildPresentation( const QString&, SALOME_View* = 0 );
  
  /* Sets interactive object */
  void        setIO( const Handle(SALOME_InteractiveObject)& theIO );
  
  /* Sets shape */
  void        setShape( const TopoDS_Shape& theShape );
  
  /* Resets internal data */
  void        internalReset();

  void        clearTemporary( LightApp_SelectionMgr* theSelMgr );

  SUIT_SelectionFilter* getFilter( const int theMode );
  
protected:
  Handle(SALOME_InteractiveObject) myIO;
  TopoDS_Shape                     myShape;
  string                           myName;
  int                              myType;
  SALOME_View*                     myViewFrame;

  // Attributes
  Quantity_Color                   myShadingColor;
  int                              myColor;
  double                           myWidth;
  bool                             myToActivate;
  int                              myDisplayMode;

private:
  SalomeApp_Application* myApp;
};

#endif // __GEOM_DISPLAYER_H

