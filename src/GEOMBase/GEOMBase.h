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
// File   : GEOMBase.h
// Author : Damien COQUERET, Open CASCADE S.A.S.
//
#ifndef GEOMBASE_H
#define GEOMBASE_H

#include "GEOM_GEOMBase.hxx"

// SALOME Includes
#include <GEOM_AISShape.hxx>
#include "GEOM_GenericObjPtr.h"

// IDL Headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOMEDS)
#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)
#include CORBA_SERVER_HEADER(GEOM_Gen)

// QT Includes
#include <QString>

class GEOM_Actor;
class SALOME_ListIO;

class QWidget;

//=================================================================================
// class    : GEOMBase
// purpose  :
//=================================================================================
class GEOMBASE_EXPORT GEOMBase
{
public :
  /* Selection and objects management */
  static int          GetIndex( const TopoDS_Shape& subshape, const TopoDS_Shape& shape );
  static TopoDS_Shape GetShapeFromIOR( const QString& IOR );
  static bool         GetShape( GEOM::GEOM_Object_ptr object,
                                TopoDS_Shape& shape,
                                const TopAbs_ShapeEnum type = TopAbs_SHAPE );
  static TopoDS_Shape GetTopoFromSelection( const SALOME_ListIO& IObjects );
  static int          GetNameOfSelectedIObjects( const SALOME_ListIO& IObjects,
                                                 QString& name,
                                                 const bool shapesOnly = false );
  static QString      GetShapeTypeString( const TopoDS_Shape& shape );

  /* Convertions */
  static Handle(GEOM_AISShape)
                      ConvertIOinGEOMAISShape( const Handle(SALOME_InteractiveObject)& IO,
                                               bool onlyInActiveView = false );

  static Handle(AIS_InteractiveObject)
                      GetAIS( const Handle(SALOME_InteractiveObject)& IO,
                              bool onlyInActiveView = false,
                              bool onlyGeom = false ); 
  static QStringList  ConvertListOfIOInListOfIOR( const SALOME_ListIO& IObjects ); 

  static Handle(GEOM_AISShape) 
                      ConvertIORinGEOMAISShape( const QString& IOR,
                                                bool onlyInActiveView = false );
  static GEOM_Actor*  ConvertIORinGEOMActor( const QString& IOR,
                                             bool onlyInActiveView = false );

  static GEOM::GEOM_Object_ptr
                      ConvertIOinGEOMObject( const Handle(SALOME_InteractiveObject)& IO );

  static void         ConvertListOfIOInListOfGO( const SALOME_ListIO& IObjects,
                                                 GEOM::ListOfGO& geomObjects,
                                                 bool shapesOnly = false ); 

  static GEOM::GEOM_Object_ptr
                      GetObjectFromIOR( const QString& IOR );

  static QString      GetIORFromObject( GEOM::GEOM_Object_ptr object );

  /* Geometry */
  static bool         VertexToPoint( const TopoDS_Shape& shape, gp_Pnt& point );

  /* Used just by Plane and Prism */
  static bool         LinearEdgeExtremities( const TopoDS_Shape& shape,
                                             gp_Pnt& point1, gp_Pnt& point2 );
  static void         GetBipointDxDyDz( const gp_Pnt& point1, const gp_Pnt& point2,
                                        double& dx, double& dy, double& dz );

  /* Simulation management */
  static TopoDS_Shape CreateArrowForLinearEdge( const TopoDS_Shape& tds );

  /*  Generates default names */
  static bool         SelectionByNameInDialogs( QWidget* widget,
                                                const QString& userObjectName,
                                                const SALOME_ListIO& IObjects );
  /* Shows message box with error code */
  static void         DefineDlgPosition( QWidget* dlg, int& x, int& y );

  /* This method generates default names for results of geometrical operations */
  static QString      GetDefaultName( const QString& operation, bool extractPrefix = false );

  /* Shows message box with error code and comment */
  static void         ShowErrorMessage( const QString& errorCode, const QString& comment = QString() );

  /* Gets name of object */
  static QString      GetName( GEOM::GEOM_Object_ptr object );

  /* Check if object has shape */
  static bool         IsShape( GEOM::GEOM_Object_ptr object );

  /* Get string representation of shape type */
  static QString      TypeName( TopAbs_ShapeEnum type, bool capitalize = false );
  
  /* Get study entry for the given object  */
  static QString      GetEntry( GEOM::GEOM_Object_ptr object );

  /* Publish sub-shape under the main object */
  static void         PublishSubObject( GEOM::GEOM_Object_ptr object, const QString& name = QString() );

  static void         Synchronize( QList<GEOM::GeomObjPtr>& left, QList<GEOM::GeomObjPtr>& right );
};

#endif // GEOMBASE_H
