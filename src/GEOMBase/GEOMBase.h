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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : GEOMBase.h
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

#ifndef GEOMBASE_H
#define GEOMBASE_H

// SALOME Includes
#include "GEOM_AISShape.hxx"

// IDL Headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOMEDS)
#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)
#include CORBA_SERVER_HEADER(GEOM_Gen)

// QT Includes
#include <qstring.h>

class GEOM_Actor;
class SALOME_Selection;
class SALOME_ListIO;
class QAD_Desktop;
class TColStd_MapOfInteger;

class QWidget;

//=================================================================================
// class    : GEOMBase
// purpose  :
//=================================================================================
class GEOMBase
{
public :
  GEOMBase();
  ~GEOMBase();

  // SAN -- TO BE REMOVED !!!
  static bool Display( GEOM::GEOM_Object_ptr ) {return false;}
  static bool AddInStudy( GEOM::GEOM_Object_ptr ) {return false;}
  static void DisplaySimulationShape(const TopoDS_Shape& S) {}; 
  static void EraseSimulationShape() {};
  // SAN -- TO BE REMOVED !!!

  /* Selection and objects management */
  static int GetIndex(const TopoDS_Shape& subshape, const TopoDS_Shape& shape, int ShapeType);
  static TopoDS_Shape GetShapeFromIOR(QString IOR);
  static bool GetShape( const GEOM::GEOM_Object_ptr&, TopoDS_Shape&, const TopAbs_ShapeEnum = TopAbs_SHAPE );
  static bool GetTopoFromSelection(SALOME_Selection *Sel, TopoDS_Shape& tds);
  static int GetNameOfSelectedIObjects(SALOME_Selection* Sel, QString& aName, const bool theShapesOnly = false );
  static bool GetShapeTypeString(const TopoDS_Shape& aShape, Standard_CString& aTypeString);

  /* Convertions */
  static GEOM::GEOM_Object_ptr ConvertIOinGEOMShape(const Handle(SALOME_InteractiveObject)& IO, 
					    Standard_Boolean& testResult);
  static Handle(GEOM_AISShape) ConvertIOinGEOMAISShape(const Handle(SALOME_InteractiveObject)& IO,
						Standard_Boolean& testResult,
						bool onlyInActiveView = false);

  static Handle(AIS_InteractiveObject) GetAIS( const Handle(SALOME_InteractiveObject)& theIO,
                                               const bool                              isOnlyInActiveView = false ); 
  static void ConvertListOfIOInListOfIOR(const SALOME_ListIO& aList,
				  GEOM::string_array& listIOR); 

  static Handle(GEOM_AISShape) ConvertIORinGEOMAISShape(const char * IOR,
						 Standard_Boolean& testResult,
						 bool onlyInActiveView = false);
  static GEOM_Actor* ConvertIORinGEOMActor(const char * IOR, Standard_Boolean& testResult,
				    bool onlyInActiveView = false);

  static GEOM::GEOM_Object_ptr ConvertIOinGEOMObject(const Handle(SALOME_InteractiveObject)& IO, 
					      Standard_Boolean& testResult);

  static void ConvertListOfIOInListOfGO( const SALOME_ListIO& aList,
                                         GEOM::ListOfGO& listGO,
                                         const bool theShapesOnly = false ); 

  static GEOM::GEOM_Object_ptr GetObjectFromIOR( const char* theIOR );

  static char* GetIORFromObject( const GEOM::GEOM_Object_ptr& theObject );
  
  /* Geometry */
  static bool VertexToPoint(const TopoDS_Shape& S, gp_Pnt& P);

  /* Used just by Plane and Prism */
  static bool LinearEdgeExtremities(const TopoDS_Shape& S, gp_Pnt& P1, gp_Pnt& P2);
  static void GetBipointDxDyDz(gp_Pnt P1, gp_Pnt P2, double& dx, double& dy, double& dz);

  /* User dialog 1 parameter returned */
  static double Parameter(Standard_Boolean& res,
		   const char* aValue1 = 0, const char* aTitle1 = 0,
		   const char* aTitle = 0, const double bottom = -1E6,
		   const double top = +1E6, const int decimals = 6);

  /* Simulation management */
  static bool CreateArrowForLinearEdge(const TopoDS_Shape& tds, TopoDS_Shape& ArrowCone);

  /*  Generates default names */
  static bool SelectionByNameInDialogs(QWidget* aWidget, const QString& userObjectName, SALOME_Selection *Sel);
  /* Shows message box with error code */
  static bool DefineDlgPosition(QWidget* aDlg, int& x, int& y);

  /* This method generates default names for results of geometrical operations */
  static QString GetDefaultName(const QString& theOperation);   
  /* Shows message box with error code and comment */
  static void ShowErrorMessage(const char* theErrorCode, const char* theComment = 0);

  /* Gets name of object */
  static const char* GetName( GEOM::GEOM_Object_ptr );

  static bool IsShape( GEOM::GEOM_Object_ptr theObj );
};

#endif

