//  GEOM GEOM : implementaion of GEOM_Gen.idl and GEOM_Shape.idl
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
//  File   : GEOM_Shape_i.hh
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#ifndef __GEOM_SHAPE_I_H__
#define __GEOM_SHAPE_I_H__

// SALOME config header
#include <SALOMEconfig.h>

// Standard C++ headers
#include <iostream.h>

// IDL headers
#include CORBA_SERVER_HEADER(GEOM_Gen)
#include CORBA_SERVER_HEADER(GEOM_Shape)

// Cascade headers
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS.hxx>


//=====================================================================
// GEOM_Shape_i : class definition
//=====================================================================
class GEOM_Shape_i: public POA_GEOM::GEOM_Shape,
		    public PortableServer::RefCountServantBase {
private:
  
  TopoDS_Shape _geom;
  CORBA::ORB_ptr _orb;
  GEOM::GEOM_Gen_ptr _engine;
  
  GEOM::shape_type _shapetype ; // enum defined in the IDL file (Occ topol. type of a shape)
  bool _ismain;
  GEOM::GEOM_Shape::ListOfSubShapeID _index;
  char* _name;
  char* _mainname;
  char* _shapeid;
  char* _studyshapeid; // exists only if added in the study document
  char* _nametype;
 

public:

  // no-arg constructor, doing nothing (for now ?)
  GEOM_Shape_i();
  
  // constructor to be called for servant creation
  GEOM_Shape_i(TopoDS_Shape geom,
	       CORBA::ORB_ptr orb,
	       GEOM::GEOM_Gen_ptr engine,
	       const GEOM::GEOM_Shape::ListOfSubShapeID& index,
	       GEOM::shape_type sht = GEOM::SHAPE, 
	       bool ismain = true);
  
  // destructor deleting the internal geometry structure
  ~GEOM_Shape_i();
 
  char* Name();
  void  Name(const char* name);
  
  char* MainName();
  void  MainName(const char* name);

  char* NameType();
  void  NameType(const char* name);

  bool  IsMainShape(); 
  void  IsMainShape(const bool abool);

  char* ShapeId(); 
  void  ShapeId(const char* shapeid);

  char* StudyShapeId();
  void  StudyShapeId(const char* studyshapeid);

  GEOM::GEOM_Shape::ListOfSubShapeID* Index();
  void  Index(const GEOM::GEOM_Shape::ListOfSubShapeID&);

  GEOM::shape_type ShapeType();
  void  ShapeType(GEOM::shape_type sht);

  GEOM::GEOM_Shape::TMPFile* GetShapeStream();

  GEOM::GEOM_Gen_ptr Engine();

  long getShape();
};
  
#endif
