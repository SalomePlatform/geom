//  GEOM GEOMDS : implementation of Geometry component data structure and Geometry documents management
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
//  File   : GEOMDS_DataMapOfIntegerTransient_0.cxx
//  Module : GEOM

using namespace std;
#include "GEOMDS_DataMapOfIntegerTransient.hxx"

#ifndef _Standard_DomainError_HeaderFile
#include <Standard_DomainError.hxx>
#endif
#ifndef _Standard_NoSuchObject_HeaderFile
#include <Standard_NoSuchObject.hxx>
#endif
#ifndef _Standard_Transient_HeaderFile
#include <Standard_Transient.hxx>
#endif
#ifndef _TColStd_MapIntegerHasher_HeaderFile
#include <TColStd_MapIntegerHasher.hxx>
#endif
#ifndef _GEOMDS_DataMapNodeOfDataMapOfIntegerTransient_HeaderFile
#include "GEOMDS_DataMapNodeOfDataMapOfIntegerTransient.hxx"
#endif
#ifndef _GEOMDS_DataMapIteratorOfDataMapOfIntegerTransient_HeaderFile
#include "GEOMDS_DataMapIteratorOfDataMapOfIntegerTransient.hxx"
#endif
 

#define TheKey Standard_Integer
#define TheKey_hxx <Standard_Integer.hxx>
#define TheItem Handle_Standard_Transient
#define TheItem_hxx <Standard_Transient.hxx>
#define Hasher TColStd_MapIntegerHasher
#define Hasher_hxx <TColStd_MapIntegerHasher.hxx>
#define TCollection_DataMapNode GEOMDS_DataMapNodeOfDataMapOfIntegerTransient
#define TCollection_DataMapNode_hxx <GEOMDS_DataMapNodeOfDataMapOfIntegerTransient.hxx>
#define TCollection_DataMapIterator GEOMDS_DataMapIteratorOfDataMapOfIntegerTransient
#define TCollection_DataMapIterator_hxx <GEOMDS_DataMapIteratorOfDataMapOfIntegerTransient.hxx>
#define Handle_TCollection_DataMapNode Handle_GEOMDS_DataMapNodeOfDataMapOfIntegerTransient
#define TCollection_DataMapNode_Type_() GEOMDS_DataMapNodeOfDataMapOfIntegerTransient_Type_()
#define TCollection_DataMap GEOMDS_DataMapOfIntegerTransient
#define TCollection_DataMap_hxx <GEOMDS_DataMapOfIntegerTransient.hxx>
#include <TCollection_DataMap.gxx>

