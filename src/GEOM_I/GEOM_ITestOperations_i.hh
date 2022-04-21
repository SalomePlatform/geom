// Copyright (C) 2007-2021  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef _GEOM_ITestOperations_i_HeaderFile
#define _GEOM_ITestOperations_i_HeaderFile

#include "GEOMImpl_Gen.hxx"

#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include "GEOMImpl_ITestOperations.hxx"

class GEOM_I_EXPORT GEOM_ITestOperations_i :
    public virtual POA_GEOM::GEOM_ITestOperations,
    public virtual GEOM_IOperations_i
{
 public:
   GEOM_ITestOperations_i(PortableServer::POA_ptr thePOA,
                          GEOM::GEOM_Gen_ptr theEngine,
                          ::GEOMImpl_ITestOperations* theImpl);
   ~GEOM_ITestOperations_i();

   CORBA::Boolean Tesselate(GEOM::GEOM_Object_ptr theShape,
                            CORBA::Double theLinearDeflection,
                            CORBA::Boolean theIsRelative,
                            CORBA::Double theAngularDeflection);

   ::GEOMImpl_ITestOperations* GetOperations() { return (::GEOMImpl_ITestOperations*)GetImpl(); }
};

#endif // _GEOM_ITestOperations_i_HeaderFile
