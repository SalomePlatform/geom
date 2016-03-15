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
// File   : GEOM_Operation.cxx
// Author : Sergey ANIKIN, Open CASCADE S.A.S. (sergey.anikin@opencascade.com)
//
#include "GEOM_Operation.h"

#include <SUIT_Application.h>
#include <SalomeApp_Tools.h>


//================================================================
// Function : GEOM_Operation
// Purpose  : 
//================================================================
GEOM_Operation::GEOM_Operation( SUIT_Application* app, GEOM::GEOM_IOperations_ptr oper )
: SUIT_Operation( app )
{
  myIOperation = GEOM::GEOM_IOperations::_narrow( oper );
}

//================================================================
// Function : ~GEOM_Operation
// Purpose  : 
//================================================================
GEOM_Operation::~GEOM_Operation()
{
}

//================================================================
// Function : onStartOperation()
// Purpose  : Opens an internal transaction in GEOM engine
//================================================================
void GEOM_Operation::startOperation()
{
  SUIT_Operation::startOperation();

  if ( !myIOperation->_is_nil() ) {
    try {
      myIOperation->StartOperation();
      setState( Running );
    }
    catch ( const SALOME::SALOME_Exception& e ) {
      SalomeApp_Tools:: QtCatchCorbaException( e );
    }
  }
}

//================================================================
// Function : onFinishOperation
// Purpose  : Commits an internal transaction in GEOM engine
//================================================================
void GEOM_Operation::commitOperation()
{
  SUIT_Operation::commitOperation();

  if ( state() != Running )
    return;
  
  if ( !myIOperation->_is_nil() ) {
    try {
      myIOperation->FinishOperation();
      setState( Waiting );
    }
    catch ( const SALOME::SALOME_Exception& e ) {
      SalomeApp_Tools:: QtCatchCorbaException( e );
    }
  }
}

//================================================================
// Function : onSuspendOperation
// Purpose  : 
//================================================================
void GEOM_Operation::suspendOperation()
{
  SUIT_Operation::suspendOperation();
}

//================================================================
// Function : onResumeOperation
// Purpose  : 
//================================================================
void GEOM_Operation::resumeOperation()
{
  SUIT_Operation::resumeOperation();
}

//================================================================
// Function : onAbortOperation
// Purpose  : Aborts an internal transaction in GEOM engine
//================================================================
void GEOM_Operation::abortOperation()
{
  SUIT_Operation::abortOperation();

  if ( !myIOperation->_is_nil() ) {
    try {
      myIOperation->AbortOperation();
    }
    catch ( const SALOME::SALOME_Exception& e ) {
      SalomeApp_Tools::QtCatchCorbaException( e );
    }
  }
}

