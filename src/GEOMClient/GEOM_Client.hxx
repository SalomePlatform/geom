// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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
//  File   : GEOM_Client.hxx
//  Author : Yves FRICAUD
//  Module : GEOM

#ifndef _GEOM_Client_HeaderFile
#define _GEOM_Client_HeaderFile

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Gen)

#include <Standard_Boolean.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>

#include <map>
#include <vector>

#ifdef WIN32
  #if defined GEOMCLIENT_EXPORTS || defined GEOMClient_EXPORTS
    #define GEOMCLIENT_EXPORT __declspec( dllexport )
  #else
    #define GEOMCLIENT_EXPORT __declspec( dllimport )
  #endif
#else
   #define GEOMCLIENT_EXPORT
#endif

/*
 * if define SINGLE_CLIENT is not commented, the method get_client always returns the same GEOM_Client object (singleton object)
 * and the SHAPE_READER macro can be used to define an object that is always this singleton object
 * if define SINGLE_CLIENT is commented, we get the old way to define the GEOM_Client objects : get_client returns a new object
 * and the SHAPE_READER macro defines also a new object
 */
#define SINGLE_CLIENT

//=====================================================================
// GEOM_Client : class definition
//=====================================================================
class GEOMCLIENT_EXPORT GEOM_Client  {

 public:
  // Methods PUBLIC
  //
  GEOM_Client();
  GEOM_Client(Engines::Container_ptr client);
  GEOM_Client(const GEOM_Client& client);

  TopoDS_Shape GetShape( GEOM::GEOM_Gen_ptr geom, GEOM::GEOM_Object_ptr aShape );
  Standard_Boolean Find( const TopoDS_Shape& S, TCollection_AsciiString& IOR ) ;
  void RemoveShapeFromBuffer( const TCollection_AsciiString& IOR ) ;
  void ClearClientBuffer() ;

#ifdef SINGLE_CLIENT
  static GEOM_Client& get_client();
#else
  static GEOM_Client get_client();
#endif

 private:
  // Methods PRIVATE
  //
  Standard_Boolean Find( const TCollection_AsciiString& IOR, TopoDS_Shape& S ) ;
  void Bind( const TCollection_AsciiString& IOR, const TopoDS_Shape& S, int Tick ) ;
  TopoDS_Shape Load( GEOM::GEOM_Gen_ptr geom, GEOM::GEOM_Object_ptr aShape);

  // Fields PRIVATE
  //
  std::map< TCollection_AsciiString , std::vector<TopoDS_Shape> > _mySubShapes;
  std::map< TCollection_AsciiString , TopoDS_Shape > myShapesMap;
  std::map< TCollection_AsciiString , int > myTicksMap;
  long  pid_client;
};

#ifdef SINGLE_CLIENT
#define SHAPE_READER(obj) GEOM_Client& obj=GEOM_Client::get_client()
#else
#define SHAPE_READER(obj) GEOM_Client obj
#endif

#endif
