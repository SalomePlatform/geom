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

#ifndef _GEOM_BaseObject_HeaderFile
#define _GEOM_BaseObject_HeaderFile

#include "GEOM_Function.hxx"

#include <Standard_GUID.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Label.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TFunction_Driver.hxx>

#include <vector>

class GEOM_BaseObject;
class GEOM_Engine;

DEFINE_STANDARD_HANDLE( GEOM_BaseObject, Standard_Transient );

class GEOM_BaseObject : public Standard_Transient
{
  friend class GEOM_Engine;

protected:
  Standard_EXPORT GEOM_BaseObject(const TDF_Label& theLabel);

public:

  Standard_EXPORT GEOM_BaseObject(const TDF_Label& theEntry, int theType);
  Standard_EXPORT ~GEOM_BaseObject();

  //Finds a GEOM_BaseObject on the label theLabel
  Standard_EXPORT static Handle(GEOM_BaseObject) GetObject(const TDF_Label& theLabel);

  //Finds a GEOM_BaseObject by a reference, stored on the label theLabel
  Standard_EXPORT static Handle(GEOM_BaseObject) GetReferencedObject(const TDF_Label& theLabel);

  // Returns type of a object (GEOM_POINT, GEOM_VECTOR...) on theLabel, -1 if no object is there
  Standard_EXPORT static int GetType(const TDF_Label& theLabel);

  //Returns a GEOM_BaseObject common GUID.
  // This GUID marks the label of any object in GEOM module
  Standard_EXPORT static const Standard_GUID& GetObjectID();

  //Returns a GUID associated with a sub-shape object
  // This GUID corresponds to GEOM_SubShapeDriver
  Standard_EXPORT static const Standard_GUID& GetSubShapeID();

  //###########################################################
  //Access to properties
  //###########################################################

  //Returns a TreeNode that presents a root of a function tree for this GEOM_BaseObject
  Standard_EXPORT Handle(TDataStd_TreeNode) GetRootNode() { return _root; }

  //Returns a label of this GEOM_BaseObject
  Standard_EXPORT TDF_Label GetEntry() const { return _label; }

  //Returns an entry of this GEOM_BaseObject
  Standard_EXPORT TCollection_AsciiString GetEntryString();

  //Returns a type of this GEOM_BaseObject (GEOM_POINT, GEOM_VECTOR...)
  Standard_EXPORT int GetType();

  //Sets the type of this GEOM_BaseObject
  Standard_EXPORT void SetType(int theType);

  //Modifications counter management
  Standard_EXPORT int  GetTic();
  Standard_EXPORT void SetTic(int theTic);
  Standard_EXPORT void IncrementTic();

  //Returns an ID of the OCAF document where this GEOM_BaseObject is stored
  Standard_EXPORT int GetDocID();

  //Sets a name of this GEOM_BaseObject
  Standard_EXPORT void SetName(const char* theName);

  //Returns a name of this GEOM_BaseObject
  Standard_EXPORT TCollection_AsciiString GetName();

  //Sets an auxiliary data
  Standard_EXPORT void SetAuxData(const char* theData);

  //Returns an auxiliary data
  Standard_EXPORT TCollection_AsciiString GetAuxData();

  //Set a notebook variables used for object creation
  Standard_EXPORT void SetParameters(const TCollection_AsciiString& theParameters);

  //Get a notebook variables used for object creation
  Standard_EXPORT TCollection_AsciiString GetParameters() const;

  //###########################################################
  // CORBA related methods
  //###########################################################

  //Sets an IOR of CORBA GEOM_BaseObject_i which refers to this object
  Standard_EXPORT void SetIOR(TCollection_AsciiString& theIOR) { _ior = theIOR; }

  //Returns an IOR of CORBA GEOM_BaseObject_i which refers to this object
  Standard_EXPORT TCollection_AsciiString GetIOR() { return _ior; }

  //###########################################################
  //Functions methods
  //###########################################################

  //Adds a function with a driver GUID = theGUID and a type theFunctionType
  //to the function tree of this GEOM_BaseObject
  Standard_EXPORT Handle(GEOM_Function) AddFunction(const Standard_GUID& theGUID,
                                                    int                  theFunctionType,
                                                    bool                 allowSubShape=false);

  //Returns a number of functions of this GEOM_BaseObject
  Standard_EXPORT int GetNbFunctions();

  //Returns a function with given number theFunctionNumber
  Standard_EXPORT Handle(GEOM_Function) GetFunction(int theFunctionNumber);

  //Return the last function of this GEOM_BaseObject
  Standard_EXPORT Handle(GEOM_Function) GetLastFunction();

  //Returns all dependencies of the object
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GetAllDependency();

  //Returns the dependencies of the last function
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GetLastDependency();

  //Returns drivers creators of this object
  Standard_EXPORT Handle(TFunction_Driver) GetCreationDriver(int funNb);

  //###########################################################
  // Internal methods
  //###########################################################

  //Returns a label which could be used to store some additional data
  Standard_EXPORT TDF_Label GetFreeLabel();

 protected:
  Handle(TDataStd_TreeNode) _root;
  TDF_Label                 _label;
  TCollection_AsciiString   _ior;
  TCollection_AsciiString   _parameters;
  int                       _docID;

public:
  OCCT_DEFINE_STANDARD_RTTIEXT(GEOM_BaseObject,Standard_Transient)
};

#endif
