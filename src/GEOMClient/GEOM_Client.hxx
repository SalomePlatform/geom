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
//  File   : GEOM_Client.hxx
//  Author : Yves FRICAUD
//  Module : GEOM
//  $Header$

#ifndef _GEOM_Client_HeaderFile
#define _GEOM_Client_HeaderFile

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Shape)
#include CORBA_SERVER_HEADER(GEOM_Gen)
#
#ifndef _TColStd_SequenceOfAsciiString_HeaderFile
#include <TColStd_SequenceOfAsciiString.hxx>
#endif
#ifndef _TopTools_SequenceOfShape_HeaderFile
#include <TopTools_SequenceOfShape.hxx>
#endif
#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif
class TCollection_AsciiString;
class TopoDS_Shape;


#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

//=====================================================================
// GEOM_Client : class definition
//=====================================================================
class GEOM_Client  {

public:
  
  inline void* operator new(size_t,void* anAddress) 
  {
    return anAddress;
  }
  inline void* operator new(size_t size) 
  { 
    return Standard::Allocate(size); 
  }
  inline void  operator delete(void *anAddress) 
  { 
    if (anAddress) Standard::Free((Standard_Address&)anAddress); 
  }
  // Methods PUBLIC
  // 
  Standard_EXPORT   GEOM_Client();
  Standard_EXPORT   GEOM_Client(Engines::Container_ptr client);
  Standard_EXPORT   Standard_Integer Find( const TCollection_AsciiString& ShapeIOR, TopoDS_Shape& S ) ;
  Standard_EXPORT   void Bind( const TCollection_AsciiString& ShapeIOR, const TopoDS_Shape& S ) ;
  Standard_EXPORT   TopoDS_Shape GetShape( GEOM::GEOM_Gen_ptr geom, GEOM::GEOM_Shape_ptr aShape );
  Standard_EXPORT   void RemoveShapeFromBuffer( const TCollection_AsciiString& shapeIOR ) ;
  Standard_EXPORT   void ClearClientBuffer() ;
  Standard_EXPORT   unsigned int BufferLength() ;
                    TopoDS_Shape Load( GEOM::GEOM_Gen_ptr geom, GEOM::GEOM_Shape_ptr aShape);

private: 
  // Fields PRIVATE
  //
  TColStd_SequenceOfAsciiString myIORs ;
  TopTools_SequenceOfShape myShapes ;
  long  pid_client;
};


#endif
