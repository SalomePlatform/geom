// Copyright (C) 2007-2020  CEA/DEN, EDF R&D, OPEN CASCADE
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
//  File   : GEOM_Client.cxx
//  Author : Yves FRICAUD/Lucien PIGNOLONI
//  Module : GEOM

#include <Standard_Stream.hxx>

#include <sstream>

#include "GEOM_Client.hxx"
#include <SALOMEconfig.h>
#include "Basics_Utils.hxx"
#include "utilities.h"

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopExp_Explorer.hxx>
#include <TopExp.hxx>
#include <TopAbs.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#ifdef WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

#include CORBA_SERVER_HEADER(SALOMEDS)
#include CORBA_SERVER_HEADER(GEOM_Gen)

#define HST_CLIENT_LEN 256

//=======================================================================
// function : Load()
// purpose  :
//=======================================================================
TopoDS_Shape GEOM_Client::Load( GEOM::GEOM_Gen_ptr geom, GEOM::GEOM_Object_ptr aShape )
{
  std::string hst_client = Kernel_Utils::GetHostname();

  Engines::Container_var ctn_server = geom->GetContainerRef();
  long                   pid_server = ctn_server->getPID();

  CORBA::String_var hostname = ctn_server->getHostName();
  if ( pid_client == pid_server && !strcmp(hst_client.c_str(), hostname.in()) ) {
    TopoDS_Shape* S = (TopoDS_Shape*)(aShape->getShape());
    return(*S);
  } else {
    /* get sequence of bytes of resulting brep shape from GEOM server */
    TopoDS_Shape S;
    SALOMEDS::TMPFile_var SeqFile = aShape->GetShapeStream();
    /*int sizebuf = */SeqFile->length();
    char* buf;
    buf = (char*) &SeqFile[0];
    std::istringstream streamBrep(buf);
    BRep_Builder aBuilder;
    BRepTools::Read(S, streamBrep, aBuilder);
    return(S);
  }
}

//=======================================================================
// function : Create()
// purpose  : Create in client not in a container
//=======================================================================
GEOM_Client::GEOM_Client()
{
  pid_client =
#ifdef WIN32
    (long)_getpid();
#else
    (long)getpid();
#endif
}

//=======================================================================
// function : Create()
// purpose  : Copy constructor
//=======================================================================
GEOM_Client::GEOM_Client(const GEOM_Client& client)
{
  _mySubShapes = client._mySubShapes;
  myShapesMap = client.myShapesMap;
  pid_client = client.pid_client;
}

//=======================================================================
// function : Create()
// purpose  :
//=======================================================================
GEOM_Client::GEOM_Client(Engines::Container_ptr client)
{
  pid_client = client->getPID();
}

//=======================================================================
// function : get_client()
// purpose  : Static method to have the only one instance of GEOM_Client
//=======================================================================
#ifdef SINGLE_CLIENT
GEOM_Client& GEOM_Client::get_client()
{
  static GEOM_Client a;
  return a;
}
#else
GEOM_Client GEOM_Client::get_client()
{
  return GEOM_Client();
}
#endif

//=======================================================================
// function : Find()
// purpose  :
//=======================================================================
Standard_Boolean GEOM_Client::Find (const TCollection_AsciiString& IOR, TopoDS_Shape& S)
{
  std::map< TCollection_AsciiString , TopoDS_Shape >::iterator i2s = myShapesMap.find( IOR );
  if ( i2s != myShapesMap.end() ) {
    S = i2s->second;
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
// function : Find()
// purpose  :
//=======================================================================
Standard_Boolean GEOM_Client::Find (const TopoDS_Shape& S, TCollection_AsciiString& IOR)
{
  std::map< TCollection_AsciiString, TopoDS_Shape >::const_iterator it;
  for (it = myShapesMap.begin(); it != myShapesMap.end(); ++it) {
    if ((*it).second == S) {
      IOR = (*it).first;
      return Standard_True;
    }
  }
  return Standard_False;
}

//=======================================================================
// function : Bind()
// purpose  :
//=======================================================================
void GEOM_Client::Bind( const TCollection_AsciiString& IOR, const TopoDS_Shape& S, int Tick )
{
  myShapesMap[IOR] = S;
  myTicksMap[IOR] = Tick;
}

//=======================================================================
// function : RemoveShapeFromBuffer()
// purpose  : Remove shape from Client Buffer
//=======================================================================
void GEOM_Client::RemoveShapeFromBuffer( const TCollection_AsciiString& IOR)
{
  if ( myShapesMap.erase( IOR ))
    _mySubShapes.erase( IOR );
}

//=======================================================================
// function : ClearClientBuffer()
// purpose  : purge buffer
//=======================================================================
void GEOM_Client::ClearClientBuffer()
{
  _mySubShapes.clear();
  myShapesMap.clear();
}

//=======================================================================
// function : GetShape()
// purpose  :
//=======================================================================
TopoDS_Shape GEOM_Client::GetShape( GEOM::GEOM_Gen_ptr geom, GEOM::GEOM_Object_ptr aShape )
{
  CORBA::String_var     anIOR = geom->GetStringFromIOR(aShape);
  TCollection_AsciiString IOR = anIOR.in();
  int aShapeTick = aShape->GetTick();

  std::map< TCollection_AsciiString , int >::iterator i2t = myTicksMap.find( IOR );
  if ( i2t != myTicksMap.end() ) {
    // The shape was modified, clean the stored one
    if (i2t->second != aShapeTick)
      RemoveShapeFromBuffer(IOR);
  }

  TopoDS_Shape S;
  if ( Find( IOR, S ))
    return S;

  /******* in case of a MAIN GEOM::SHAPE ********/
  if ( aShape->IsMainShape() ) {
    S = Load(geom, aShape);
    Bind(IOR, S, aShapeTick);
    return S;
  }

  /******* in case of SUB GEOM::SHAPE ***********/
  // Load and Explore the Main Shape
  GEOM::GEOM_Object_var mainGO = aShape->GetMainShape();
  TopoDS_Shape      aMainShape = GetShape( geom, mainGO );
  GEOM::ListOfLong_var    list = aShape->GetSubShapeIndices();

  CORBA::String_var      aMainIOR = geom->GetStringFromIOR( mainGO );
  TCollection_AsciiString mainIOR = aMainIOR.in();

  //find subshapes only one time
  auto pos2isnew = _mySubShapes.insert( std::make_pair( mainIOR, std::vector<TopoDS_Shape>() ));
  std::vector<TopoDS_Shape> & subShapes = pos2isnew.first->second;
  if ( pos2isnew.second )
  {
    TopTools_IndexedMapOfShape anIndices;
    TopExp::MapShapes( aMainShape, anIndices );
    subShapes.insert( subShapes.end(), anIndices.cbegin(), anIndices.cend() );
  }

  /* Case of only one subshape */
  if ( list->length() == 1 && list[0] > 1 ) {
    S = subShapes[list[0]-1];
  }
  else {
    BRep_Builder B;
    TopoDS_Compound aCompound;
    B.MakeCompound(aCompound);
    CORBA::Long nbSub = subShapes.size();
    for ( size_t i = 0; i < list->length(); i++ )
      if ( 0 < list[i] && list[i] <= nbSub )
        B.Add(aCompound, subShapes[list[i]-1] );

    S = aCompound;
  }
  Bind(IOR, S, aShapeTick);
  return S;
}
