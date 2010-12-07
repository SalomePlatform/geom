//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

//  GEOM GEOMClient : tool to transfer BREP files from GEOM server to GEOM client
//  File   : GEOM_Client.cxx
//  Author : Yves FRICAUD/Lucien PIGNOLONI
//  Module : GEOM
//  $Header$
//
#include <Standard_Stream.hxx>

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

#ifdef WNT
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
#ifdef WNT
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
  myIORs = client.myIORs;
  myShapes = client.myShapes;
  _myIndexes = client._myIndexes;
  _mySubShapes = client._mySubShapes;
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
// function : Find()
// purpose  :
//=======================================================================
Standard_Integer GEOM_Client::Find( const TCollection_AsciiString& IOR, TopoDS_Shape& S )
{
  if(_myIndexes.count(IOR) != 0)
    {
      Standard_Integer i =_myIndexes[IOR];
      S = myShapes.Value(i);
      return i;
    }
  return 0;
}

//=======================================================================
// function : Find()
// purpose  :
//=======================================================================
Standard_Integer GEOM_Client::Find( const TopoDS_Shape& S, TCollection_AsciiString& IOR )
{
  for ( Standard_Integer i = 1; i<= myShapes.Length(); i++ ) {
    if (myShapes.Value(i) == S) {
      IOR = myIORs.Value(i);
      return i;
    }
  }
  return 0;
}

//=======================================================================
// function : Bind()
// purpose  :
//=======================================================================
void GEOM_Client::Bind( const TCollection_AsciiString& IOR, const TopoDS_Shape& S )
{
  myIORs.Append(IOR);
  myShapes.Append(S);
  _myIndexes[IOR]=myIORs.Length();
}

//=======================================================================
// function : RemoveShapeFromBuffer()
// purpose  : Remove shape from Client Buffer
//=======================================================================
void GEOM_Client::RemoveShapeFromBuffer( const TCollection_AsciiString& IOR)
{
  if( myIORs.IsEmpty() )
    return;

  TopoDS_Shape S;
  Standard_Integer anIndex = Find( IOR, S );
  if( anIndex != 0 ) {
    myIORs.Remove(anIndex);
    myShapes.Remove(anIndex);
    _myIndexes.erase(IOR);
    _mySubShapes.erase(IOR);
  }
  return;
}

//=======================================================================
// function : ClearClientBuffer()
// purpose  : purge buffer
//=======================================================================
void GEOM_Client::ClearClientBuffer()
{
  if( myIORs.IsEmpty() )
    return;
  myIORs.Clear();
  myShapes.Clear();
  _myIndexes.clear();
  _mySubShapes.clear();
  return;
}

//=======================================================================
// function : BufferLength()
// purpose  :
//=======================================================================
unsigned int GEOM_Client::BufferLength()
{
  return myIORs.Length();
}

//=======================================================================
// function : GetShape()
// purpose  :
//=======================================================================
TopoDS_Shape GEOM_Client::GetShape( GEOM::GEOM_Gen_ptr geom, GEOM::GEOM_Object_ptr aShape )
{
  TopoDS_Shape S;
  CORBA::String_var anIOR = geom->GetStringFromIOR(aShape);
  TCollection_AsciiString IOR = (char*)anIOR.in();
  Standard_Integer anIndex = Find(IOR, S);

  if (anIndex != 0) return S;

  /******* in case of a MAIN GEOM::SHAPE ********/
  if (aShape->IsMainShape()) {
    S = Load(geom, aShape);
    Bind(IOR, S);
    return S;
  }

  /******* in case of SUB GEOM::SHAPE ***********/
  // Load and Explore the Main Shape
  TopoDS_Shape aMainShape = GetShape (geom, aShape->GetMainShape());
  GEOM::ListOfLong_var list = aShape->GetSubShapeIndices();

  CORBA::String_var aMainIOR = geom->GetStringFromIOR(aShape->GetMainShape());
  TCollection_AsciiString mainIOR = (char*)aMainIOR.in();

  //find subshapes only one time
  if(_mySubShapes.count(mainIOR)==0)
  {
    TopTools_IndexedMapOfShape anIndices;
    TopExp::MapShapes(aMainShape, anIndices);
    Standard_Integer ii = 1, nbSubSh = anIndices.Extent();
    for (; ii <= nbSubSh; ii++) 
    {
      _mySubShapes[mainIOR].push_back(anIndices.FindKey(ii));
    }
  }

  /* Case of only one subshape */
  if (list->length() == 1 && list[0] > 0) {
    S = _mySubShapes[mainIOR][list[0]-1];
  }
  else {
    BRep_Builder B;
    TopoDS_Compound aCompound;
    B.MakeCompound(aCompound);
    for (int i = 0; i < list->length(); i++) {
      if (0 < list[i] && list[i] <= _mySubShapes[mainIOR].size()) {
        TopoDS_Shape aSubShape = _mySubShapes[mainIOR][list[i]-1];
        B.Add(aCompound, aSubShape);
      }
    }

    S = aCompound;
  }
  Bind(IOR, S);
  return S;
}
