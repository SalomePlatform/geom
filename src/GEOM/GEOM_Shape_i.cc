using namespace std;
// File :      GEOM_Shape_i.cc
// Created :   
// Author :    Lucien PIGNOLONI
// Project :   SALOME
// Copyright : OPEN CASCADE
// $Header$

#include <BRepTools_ShapeSet.hxx>
#include "GEOM_Shape_i.hh"
#include "SALOME_NamingService.hxx"
#include <fstream.h>
#include <BRepTools.hxx>



//=================================================================================
// function : GEOM_Shape_i() constructor (no arguments)
// purpose  : for what now ?
//=================================================================================
GEOM_Shape_i::GEOM_Shape_i() { }



//=================================================================================
// function : constructor
// purpose  : constructor for servant creation
//=================================================================================
GEOM_Shape_i::GEOM_Shape_i(TopoDS_Shape geom,
			   CORBA::ORB_ptr orb,
			   GEOM::GEOM_Gen_ptr engine,
			   const GEOM::GEOM_Shape::ListOfSubShapeID& index,
			   GEOM::shape_type sht,
			   bool ismain) {
  _geom = geom;
  _orb = orb;
  _engine = engine;
  _shapetype = sht ;
  _ismain = ismain;
  _index = index ;

  _shapeid = "";
  _studyshapeid = "";

  _name = "";
  _mainname ="";
  _nametype ="";

}



//=================================================================================
// function : destructor
// purpose  : deleting the internal geometry structure
//=================================================================================
GEOM_Shape_i::~GEOM_Shape_i() { delete &_geom; }


//=================================================================================
// function : Name (set method)
// purpose  : to set the attribute 'name' of this shape.
//          : WARNING : Register to naming service actually removed !
//=================================================================================
void GEOM_Shape_i::Name(const char* name) {
  _name = strdup(name);
  GEOM::GEOM_Shape_ptr g = GEOM::GEOM_Shape::_narrow(_this());

  // Removed declaration of shapes to naming service
  //SALOME_NamingService * ns = new SALOME_NamingService(_orb);
  //ns->Register(g, _name);
}


//=================================================================================
// function : Name (get method)
// purpose  : to get the attribute 'name' of this shape
//=================================================================================
char* GEOM_Shape_i::Name() { return strdup(_name); }

//=================================================================================
// function : MainName (set method)
// purpose  : to set the attribute 'name' of this mainshape. 
//=================================================================================
void GEOM_Shape_i::MainName(const char* name) {
  _mainname = strdup(name);
}


//=================================================================================
// function : MainName (get method)
// purpose  : to get the attribute 'name' of this shape
//=================================================================================
char* GEOM_Shape_i::MainName() { return strdup(_mainname); }

//=================================================================================
// function : IsMainShape (get method)
// purpose  : return 'true' if this is a main shape (not a sub shape)
//=================================================================================
bool GEOM_Shape_i::IsMainShape() { return _ismain ; }


//=================================================================================
// function : IsMainShape (set method)
// purpose  : to set the property 'ismain' true or false of this shape
//=================================================================================
void GEOM_Shape_i::IsMainShape(const bool abool) { _ismain = abool ; }


//=================================================================================
// function : ShapeId
// purpose  : to get the id of this shape from GEOM (OCAF entry)
//=================================================================================
char* GEOM_Shape_i::ShapeId() { return strdup(_shapeid) ; }


//=================================================================================
// function : ShapeId (set method) 
// purpose  : to set the id of this shape in GEOM/OCAF doc
//=================================================================================
void GEOM_Shape_i::ShapeId(const char * shapeid) { _shapeid = strdup(shapeid) ; }



//=================================================================================
// function : StudyShapeId (get method)
// purpose  : to get the id of this shape from the study document (OCAF entry)
//=================================================================================
char* GEOM_Shape_i::StudyShapeId() { return strdup(_studyshapeid) ; }


//=================================================================================
// function : StudyShapeId (set method) 
// purpose  : to set the id of this shape in the Study document (OCAF entry)
//=================================================================================
void GEOM_Shape_i::StudyShapeId(const char * studyshapeid)
{ _studyshapeid = strdup(studyshapeid) ; }



//=================================================================================
// function : Index (get method)
// purpose  : to get the index of this sub shape in its main shape
//=================================================================================
GEOM::GEOM_Shape::ListOfSubShapeID* GEOM_Shape_i::Index() { 
  unsigned int _length = _index.length();
  GEOM::GEOM_Shape::ListOfSubShapeID_var _list = new GEOM::GEOM_Shape::ListOfSubShapeID;
  _list->length(_length);

  for (unsigned int ind = 0; ind < _length; ind++) {
    _list[ind] = _index[ind];
  }

  return _list._retn() ; 
}


//=================================================================================
// function : Index (set method) 
// purpose  : to set the index of this sub shape (in a main shape)
//=================================================================================
void GEOM_Shape_i::Index(const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfSubShapeID) 
{ _index = ListOfSubShapeID ; }


//=================================================================================
// function : ShapeType (get method)
// purpose  : to get the topological type of this shape as defined in enum of IDL file
//=================================================================================
GEOM::shape_type GEOM_Shape_i::ShapeType() { return _shapetype ; }


//=================================================================================
// function : SetType (set method)
// purpose  : to set the topological type of this shape (see GetType)
//=================================================================================
void GEOM_Shape_i::ShapeType(GEOM::shape_type sht) { _shapetype = sht ; }


//=================================================================================
// function : NameType (set method)
// purpose  : to set the attribute 'nametype' of this shape. 
//=================================================================================
void GEOM_Shape_i::NameType(const char* name) {
}

//=================================================================================
// function : NameType (get method)
// purpose  : to get the attribute 'nametype' of this shape
//=================================================================================
char* GEOM_Shape_i::NameType() { return strdup(_nametype); }

//=================================================================================
// function : GetShapeStream
// Transfer resulting shape to client as sequence of bytes
//client can extract shape from stream using BrepTools::Read function
//=================================================================================
GEOM::GEOM_Shape::TMPFile* GEOM_Shape_i::GetShapeStream()
{
  ostrstream streamShape;
  //Write TopoDS_Shape in ASCII format to the stream
  BRepTools::Write(_geom, streamShape);
  //Returns the number of bytes that have been stored in the stream's buffer.
  int size = streamShape.pcount();
  char* buf = new char [size];
  //Get pointer on internal character array in ostrstream
  char* valueOfStream = streamShape.str();
  //Create copy of ostrstream content
  memcpy(buf, valueOfStream, size);
  //Allow automatic deletion of ostrstream content 
  streamShape.rdbuf()->freeze(0);

  CORBA::Octet* OctetBuf =  (CORBA::Octet*)buf;
  GEOM::GEOM_Shape::TMPFile_var SeqFile = new GEOM::GEOM_Shape::TMPFile(size,size,OctetBuf,1);
  return SeqFile._retn();

}

//=======================================================================
//function : Engine
//purpose  : return generator engine
//=======================================================================

GEOM::GEOM_Gen_ptr GEOM_Shape_i::Engine()
{
  return _engine;
}
