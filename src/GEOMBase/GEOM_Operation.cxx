//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2004  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : GEOM_Operation.cxx
//  Author : Sergey ANIKIN
//  Module : GEOM
//  $Header$

#include "GEOM_Operation.h"

#include "SALOMEGUI_QtCatchCorbaException.hxx"

//================================================================
// Function : GEOM_Operation
// Purpose  : 
//================================================================
GEOM_Operation::GEOM_Operation( QAD_Study* doc, GEOM::GEOM_IOperations_ptr oper )
: QAD_Operation( doc )
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
void GEOM_Operation::onStartOperation()
{
  QAD_Operation::onStartOperation();

  if ( !myIOperation->_is_nil() ) {
    try {
      myIOperation->StartOperation();
    }
    catch( const SALOME::SALOME_Exception& e ) {
      QtCatchCorbaException( e );
    }
  }
}

//================================================================
// Function : onFinishOperation
// Purpose  : Commits an internal transaction in GEOM engine
//================================================================
void GEOM_Operation::onFinishOperation()
{
  QAD_Operation::onFinishOperation();

  if ( !myIOperation->_is_nil() ) {
    try {
      myIOperation->FinishOperation();
    }
    catch( const SALOME::SALOME_Exception& e ) {
      QtCatchCorbaException( e );
    }
  }
}

//================================================================
// Function : onSuspendOperation
// Purpose  : 
//================================================================
void GEOM_Operation::onSuspendOperation()
{
  QAD_Operation::onSuspendOperation();
}

//================================================================
// Function : onResumeOperation
// Purpose  : 
//================================================================
void GEOM_Operation::onResumeOperation()
{
  QAD_Operation::onResumeOperation();
}

//================================================================
// Function : onAbortOperation
// Purpose  : Aborts an internal transaction in GEOM engine
//================================================================
void GEOM_Operation::onAbortOperation()
{
  QAD_Operation::onAbortOperation();

  if ( !myIOperation->_is_nil() ) {
    try {
      myIOperation->AbortOperation();
    }
    catch( const SALOME::SALOME_Exception& e ) {
      QtCatchCorbaException( e );
    }
  }
}

