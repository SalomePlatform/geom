// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef _GEOM_IInsertOperations_i_HeaderFile
#define _GEOM_IInsertOperations_i_HeaderFile

#include "GEOMImpl_Gen.hxx"

#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include CORBA_CLIENT_HEADER(SALOMEDS)
#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include "GEOMImpl_IInsertOperations.hxx"

class GEOM_I_EXPORT GEOM_IInsertOperations_i :
    public virtual POA_GEOM::GEOM_IInsertOperations,
    public virtual GEOM_IOperations_i
{
 public:
  GEOM_IInsertOperations_i (PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine,
                           ::GEOMImpl_IInsertOperations* theImpl);
  ~GEOM_IInsertOperations_i();

  GEOM::GEOM_Object_ptr MakeCopy (GEOM::GEOM_Object_ptr theOriginal);

  void                  Export (GEOM::GEOM_Object_ptr theOriginal,
                                const char*           theFileName,
                                const char*           theFormatName);

  GEOM::ListOfGBO* ImportFile (const char* theFileName,
			       const char* theFormatName);

  char* ReadValue (const char* theFileName,
                   const char* theFormatName,
                   const char* theParameterName);

  GEOM::GEOM_Object_ptr RestoreShape (const SALOMEDS::TMPFile& theStream);

  CORBA::Long LoadTexture(const char* theTextureFile);
  CORBA::Long AddTexture(CORBA::Long theWidth, CORBA::Long theHeight,
                         const SALOMEDS::TMPFile& theTexture);
  SALOMEDS::TMPFile* GetTexture(CORBA::Long theID,
                                CORBA::Long& theWidth,
                                CORBA::Long& theHeight);

  GEOM::ListOfLong* GetAllTextures();

  CORBA::Boolean TransferData
        (GEOM::GEOM_Object_ptr                                 theObjectFrom,
         GEOM::GEOM_Object_ptr                                 theObjectTo,
         GEOM::find_shape_method                               theFindMethod,
         GEOM::GEOM_IInsertOperations::ListOfTransferDatum_out theResult);

  ::GEOMImpl_IInsertOperations* GetOperations()
  { return (::GEOMImpl_IInsertOperations*)GetImpl(); }
};

#endif
