//  GEOM GEOMClient : tool to transfer BREP files from GEOM server to GEOM client
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
//  File   : GEOM_Client.cxx
//  Author : Yves FRICAUD/Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "GEOM_Client.hxx"
#include <SALOMEconfig.h>
#include "utilities.h"

#include CORBA_SERVER_HEADER(GEOM_Gen)

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>


#include <unistd.h>

#define HST_CLIENT_LEN 256


//=======================================================================
// function : Load()
// purpose  : 
//=======================================================================
TopoDS_Shape GEOM_Client::Load( GEOM::GEOM_Gen_ptr geom, GEOM::GEOM_Shape_ptr aShape )
{
    char hst_client[HST_CLIENT_LEN];
    gethostname(hst_client, HST_CLIENT_LEN);

    Engines::Container_var ctn_server = geom->GetContainerRef();
    long                   pid_server = ctn_server->getPID();

    if ( (pid_client==pid_server) && (strcmp(hst_client, ctn_server->getHostName())==0) ) {
        TopoDS_Shape* S = (TopoDS_Shape*)(aShape->getShape());
        return(*S);
    } else {
        /* get sequence of bytes of resulting brep shape from GEOM server */
        TopoDS_Shape S;
        GEOM::GEOM_Shape::TMPFile_var SeqFile = aShape->GetShapeStream();
        int sizebuf = SeqFile->length();
        char* buf;
        buf = (char*) &SeqFile[0];
        istrstream streamBrep(buf,sizebuf);
        BRep_Builder aBuilder;
        BRepTools::Read(S, streamBrep, aBuilder);
        return(S);
    };
}


//=======================================================================
// function : Create()
// purpose  : Create in client not in a container
//=======================================================================
GEOM_Client::GEOM_Client()
{
  pid_client = (-1);
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
  for ( Standard_Integer i = 1; i<= myIORs.Length(); i++ ) {
    if (myIORs.Value(i).IsEqual(IOR)) {
      S = myShapes.Value(i);
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
}

//=======================================================================
// function : RemoveShapeFromBuffer()
// purpose  : Remove shape from Client Buffer
//=======================================================================
void GEOM_Client::RemoveShapeFromBuffer( const TCollection_AsciiString& shapeIOR )
{
  if( myIORs.IsEmpty() )
    return ;
  
  TopoDS_Shape S ;
  Standard_Integer anIndex = Find( shapeIOR, S ) ;
  if( anIndex != 0 ) {
    myIORs.Remove(anIndex) ;
    myShapes.Remove(anIndex) ;
  }
  return ;
}


//=======================================================================
// function : ClearClientBuffer()
// purpose  : purge buffer
//=======================================================================
void GEOM_Client::ClearClientBuffer()
{
  if( myIORs.IsEmpty() )
    return ;
  myIORs.Clear() ;
  myShapes.Clear() ;
  return ;
}

//=======================================================================
// function : BufferLength()
// purpose  :
//=======================================================================
unsigned int GEOM_Client::BufferLength()
{
  return myIORs.Length() ;
}


//=======================================================================
// function : GetShape()
// purpose  : 
//=======================================================================

TopoDS_Shape GEOM_Client::GetShape( GEOM::GEOM_Gen_ptr geom, GEOM::GEOM_Shape_ptr aShape ) 
{ 
  
  TopoDS_Shape            S; 
  TCollection_AsciiString IOR(aShape->Name()); 
  Standard_Integer        anIndex = Find(IOR, S); 
  
  BRep_Builder B; 

  if (anIndex !=0 ) { 
    return S ; 
   } 

  /******* in case of a MAIN GEOM::SHAPE ********/ 
  if (aShape->IsMainShape()) { 
    S = Load(geom, aShape); 
    Bind(IOR,S); 
    return S; 
  } 

  /******* in case of SUB GEOM::SHAPE ***********/ 
  // Load and Explore the Main Shape 
  TopoDS_Shape MainShape = GetShape (geom, geom->GetIORFromString(aShape->MainName())); 
  GEOM::GEOM_Shape::ListOfSubShapeID_var list = aShape->Index(); 

  Standard_Integer j = 1; 
  TopExp_Explorer exp; 
  TopAbs_ShapeEnum ShapeType = TopAbs_ShapeEnum(aShape->ShapeType()); 
  
  /* Case of only one subshape */ 
  if (list->length() == 1) 
  { 
    if (ShapeType == TopAbs_COMPOUND) 
      { 
	TopoDS_Iterator it; 
	TopTools_ListOfShape CL; 
	CL.Append( MainShape ); 
	TopTools_ListIteratorOfListOfShape itC; 
	for (itC.Initialize( CL ); itC.More(); itC.Next()) 
	  { 
	    for (it.Initialize( itC.Value() );  it.More(); it.Next()) 
	      { 
		if ( it.Value().ShapeType() == TopAbs_COMPOUND) 
		  {
		    if (j == list[0]) 
		      { 
			S = it.Value(); 
			Bind(IOR, S); 
			return S; 
		      } 
		    j++; 
		    CL.Append( it.Value() ); 
		  }
	      } 
	  } 
      } 
    else 
      { 
	TopTools_MapOfShape M; 
	for (exp.Init(MainShape, ShapeType); exp.More(); exp.Next()) { 
	  if ( M.Add(exp.Current()) ) 
	    { 
	      if (j == list[0]) 
		{ 
		  S = exp.Current(); 
		  Bind(IOR, S); 
		  return S; 
		} 
	      j++; 
	    } 
	} 
      } 
  } 
  
  /* Case of a compound containing two or more sub shapes (not a main shape compound !)   */ 

  /* Warning : the compound when representing sub shapes must be explored in a sub type   */ 
  /* that is NOT ShapeType=aShape->ShapeType()= TopAbs_COMPOUND !                         */ 
  /* We have to retrieve the exact sub type of shapes contained in the compound first !   */ 
  TopoDS_Iterator it ; 
  TopAbs_ShapeEnum exactSubType ; 
  S = Load( geom, aShape ); 
  it.Initialize( S, true, true ) ; 
  it.More(); 
  exactSubType = it.Value().ShapeType() ; 

  TColStd_MapOfInteger MapIndex; 
  Standard_Integer nbSS = list->length(); 
  TopoDS_Compound Comp; 
  B.MakeCompound(Comp); 
  
  for (Standard_Integer i=1; i<=nbSS; i++) 
    MapIndex.Add(list[i-1]); 
  
  for (exp.Init(MainShape, exactSubType), j=1; exp.More() ; exp.Next(), j++) { 
    if ( MapIndex.Contains(j) ) { 
      B.Add( Comp, exp.Current() ); 
    } 
  } 
  Bind(IOR, Comp); 
  return Comp; 
} 
