//  File      : GEOM_Client.hxx
//  Created   : 
//  Author    : Yves FRICAUD
//  Project   : SALOME
//  Module    : GEOM
//  Copyright : Open CASCADE
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
  Standard_EXPORT   Standard_Integer Find( const TCollection_AsciiString& ShapeIOR, TopoDS_Shape& S ) ;
  Standard_EXPORT   void Bind( const TCollection_AsciiString& ShapeIOR, const TopoDS_Shape& S ) ;
  Standard_EXPORT   TopoDS_Shape GetShape( GEOM::GEOM_Gen_ptr geom, GEOM::GEOM_Shape_ptr aShape );
  Standard_EXPORT   void RemoveShapeFromBuffer( const TCollection_AsciiString& shapeIOR ) ;
  Standard_EXPORT   void ClearClientBuffer() ;
  Standard_EXPORT   unsigned int BufferLength() ;

private: 
  // Fields PRIVATE
  //
  TColStd_SequenceOfAsciiString myIORs ;
  TopTools_SequenceOfShape myShapes ;
};


#endif
