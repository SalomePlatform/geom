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

// GEOM GEOMGUI : GUI for Geometry component
// File   : GEOM_Operation.h
// Author : Sergey ANIKIN, Open CASCADE S.A.S. (sergey.anikin@opencascade.com)
//
#ifndef GEOM_OPERATION_H
#define GEOM_OPERATION_H

#include "GEOM_GEOMBase.hxx"

#include <SUIT_Operation.h>
#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(GEOM_Gen)

class GEOMBASE_EXPORT GEOM_Operation : public SUIT_Operation
{
public:
  GEOM_Operation( SUIT_Application*, GEOM::GEOM_IOperations_ptr );
  virtual ~GEOM_Operation();

protected:
  // Reimplemented from SUIT_Operation
  virtual void               startOperation();        
  virtual void               commitOperation();
  virtual void               suspendOperation();
  virtual void               resumeOperation();
  virtual void               abortOperation();

private:
  GEOM::GEOM_IOperations_var myIOperation;
};

#endif // GEOM_OPERATION_H
