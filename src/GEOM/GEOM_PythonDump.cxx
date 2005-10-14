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

#include "GEOM_PythonDump.hxx"

#include <TDF_Tool.hxx>

namespace GEOM
{
  size_t TPythonDump::myCounter = 0;

  TPythonDump::TPythonDump (Handle(GEOM_Function)& theFunction, bool theAppend)
  {
    myFunction = theFunction;
    myCounter++;
    myAppend = theAppend;
  }

  TPythonDump::~TPythonDump()
  {
    if (--myCounter == 0) {
      TCollection_AsciiString aDescr;
      if ( myAppend )
        aDescr = myFunction->GetDescription() + "\n\t";
      aDescr += (char *)myStream.str().c_str();
      myFunction->SetDescription( aDescr );
    }
  }

//  TPythonDump::operator TCollection_AsciiString () const
//  {
//    if (myCounter == 1) {
//      return TCollection_AsciiString ((char *)myStream.str().c_str());
//    }
//    return TCollection_AsciiString ();
//  }

  TPythonDump& TPythonDump::operator<< (long int theArg)
  {
    myStream<<theArg;
    return *this;
  }

  TPythonDump& TPythonDump::operator<< (int theArg)
  {
    myStream<<theArg;
    return *this;
  }

  TPythonDump& TPythonDump::operator<< (double theArg)
  {
    myStream.precision(16);
    myStream<<theArg;
    return *this;
  }

  TPythonDump& TPythonDump::operator<< (float theArg)
  {
    myStream.precision(8);
    myStream<<theArg;
    return *this;
  }

  TPythonDump& TPythonDump::operator<< (const void* theArg)
  {
    myStream<<theArg;
    return *this;
  }

  TPythonDump& TPythonDump::operator<< (const char* theArg)
  {
    myStream<<theArg;
    return *this;
  }

  TPythonDump& TPythonDump::operator<< (const Handle(GEOM_Object)& theObject)
  {
    TCollection_AsciiString anEntry;
    TDF_Tool::Entry(theObject->GetEntry(), anEntry);
    myStream << anEntry.ToCString();
    return *this;
  }
}
