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

#ifndef _GEOM_PYTHONDUMP_HXX_
#define _GEOM_PYTHONDUMP_HXX_

#include <SALOMEconfig.h>

#include "GEOM_Object.hxx"
#include "GEOM_Function.hxx"

#include <TCollection_AsciiString.hxx>

namespace GEOM
{
  class TPythonDump
  {
    std::ostringstream myStream;
    static size_t myCounter;

    Handle(GEOM_Function) myFunction;

  public:
    TPythonDump (Handle(GEOM_Function)& theFunction);
    virtual ~TPythonDump();

//    operator TCollection_AsciiString () const;

    TPythonDump& operator<< (long int theArg);
    TPythonDump& operator<< (int theArg);
    TPythonDump& operator<< (double theArg);
    TPythonDump& operator<< (float theArg);
    TPythonDump& operator<< (const void* theArg);
    TPythonDump& operator<< (const char* theArg);
    TPythonDump& operator<< (const Handle(GEOM_Object)& theObject);
  };
}

#endif
