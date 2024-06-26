// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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

#ifndef _GEOM_Function_HeaderFile
#define _GEOM_Function_HeaderFile

#include <Standard_Transient.hxx>
#include <Standard_GUID.hxx>
#include <TColStd_HArray1OfExtendedString.hxx>
#include <TColStd_HArray1OfByte.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDataStd_ListOfExtendedString.hxx>
#include <TopoDS_Shape.hxx>

class GEOM_Function;
DEFINE_STANDARD_HANDLE (GEOM_Function, Standard_Transient)

class GEOM_Function : public Standard_Transient
{
  GEOM_Function(const TDF_Label& theEntry) { _label = theEntry; }

public:

  //Returns a GUID for a function tree
  Standard_EXPORT static const Standard_GUID& GetFunctionTreeID();

  //Returns the ID which is associated with a reference to another function
  Standard_EXPORT static const Standard_GUID& GetDependencyID();

  //Finds and returns a function located on a label theEntry
  Standard_EXPORT static Handle(GEOM_Function) GetFunction(const TDF_Label& theEntry);


  Standard_EXPORT GEOM_Function(const TDF_Label& theEntry, const Standard_GUID& theGUID, int theType);
  Standard_EXPORT ~GEOM_Function();

  Standard_EXPORT TDF_Label GetOwnerEntry();

  //Access to properties

  //Returns a result of the function built by the function Driver
  Standard_EXPORT TopoDS_Shape GetValue();

  //Sets the function result
  Standard_EXPORT void SetValue(TopoDS_Shape& theShape);

  //Returns a function entry in the OCAF document
  Standard_EXPORT TDF_Label& GetEntry() { return _label; }

  //Returns the type of the function
  Standard_EXPORT int GetType();

  //Returns a function Driver GUID
  Standard_EXPORT Standard_GUID GetDriverGUID();

  //Returns aPython description of the function
  Standard_EXPORT TCollection_AsciiString GetDescription();

  //Sets aPython description of the function
  Standard_EXPORT void SetDescription(const TCollection_AsciiString& theDescription);

  //Access to arguments

  //Sets a real argument at position thePosition
  Standard_EXPORT void SetReal(int thePosition, double theValue);

  //Returns a real argument at position thePosition
  Standard_EXPORT double GetReal(int thePosition);

  //Sets a real array argument at position thePosition
  Standard_EXPORT void SetRealArray(int thePosition, const Handle(TColStd_HArray1OfReal)& theArray);

  //Returns a real array argument at position thePosition
  Standard_EXPORT Handle(TColStd_HArray1OfReal) GetRealArray(int thePosition);

  //Sets an integer argument at position thePosition
  Standard_EXPORT void SetInteger(int thePosition, int theValue);

  //Returns an integer argument at position thePosition
  Standard_EXPORT int GetInteger(int thePosition);

  //Sets an integer array argument at position thePosition
  Standard_EXPORT void SetIntegerArray(int thePosition, const Handle(TColStd_HArray1OfInteger)& theArray);

  //Returns an integer array argument at position thePosition
  Standard_EXPORT Handle(TColStd_HArray1OfInteger) GetIntegerArray(int thePosition);

  //Sets a byte array argument at position thePosition
  Standard_EXPORT void SetByteArray(int thePosition, const Handle(TColStd_HArray1OfByte)& theArray);

  //Returns a byte array argument at position thePosition
  Standard_EXPORT Handle(TColStd_HArray1OfByte) GetByteArray(int thePosition);

  //Sets a boolean array argument at position thePosition
  Standard_EXPORT void SetBooleanArray(int thePosition, const Handle(TColStd_HArray1OfByte)& theArray);

  //Returns a boolean array argument at position thePosition
  Standard_EXPORT Handle(TColStd_HArray1OfByte) GetBooleanArray(int thePosition);

  //Sets a reference to other function argument at position thePosition
  Standard_EXPORT void SetReference(int thePosition, Handle(GEOM_Function) theReference);

  //Sets a string argument at position thePosition
  Standard_EXPORT void SetString(int thePosition, const TCollection_AsciiString& theValue);

  //Returns a string argument at position thePosition
  Standard_EXPORT TCollection_AsciiString GetString(int thePosition);

  //Returns a reference to other function argument at position thePosition
  Standard_EXPORT Handle(GEOM_Function) GetReference(int thePosition);

  //Set an array of ExtendedString
  Standard_EXPORT void SetStringArray(int thePosition, const Handle(TColStd_HArray1OfExtendedString)& theArray);

  //Returns the array of ExtendedString
  Standard_EXPORT Handle(TColStd_HArray1OfExtendedString) GetStringArray(int thePosition);

  //Returns true if data of given type already exists
  Standard_EXPORT bool HasData(int thePosition, const Standard_GUID& dataID);

  //Returns a GUID for a references tree
  Standard_EXPORT static const Standard_GUID& GetReferencesTreeID();

  //Sets a list of references to other function arguments at position thePosition
  Standard_EXPORT void SetReferenceList (int thePosition,
                         const Handle(TColStd_HSequenceOfTransient)& theRefList);

  //Returns a list of references to other function arguments at position thePosition
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GetReferenceList (int thePosition);

  // Save a pointer to a data holder intended to pass data Driver -> Operation.
  // This method should be called by Operation to set the data holder.
  // An instance of GEOM_Function that sets the data holder will remove the
  // corresponding OCAF attribute at it's destruction
  Standard_EXPORT void SetCallBackData( void* data );

  // Returns a pointer to a data holder intended to pass data Driver -> Operation.
  // This method should be called by Driver to get the data holder to fill it in.
  // Returns NULL if the Operation have not set the data holder.
  Standard_EXPORT void* GetCallBackData();

  //Sets a TopoDS_Shape argument at position thePosition
  //void SetShape(int thePosition, const TopoDS_Shape& theShape);

  //Returns a TopoDS_Shape argument at position thePosition
  //TopoDS_Shape GetShape(int thePosition);

  //Returns true if the last method succided
  Standard_EXPORT bool IsDone() { return _isDone; }

  //Returns true if this function is the last one in the study
  Standard_EXPORT bool IsLastFuntion();

  //Returns a sequence of the external dependencies of this function
  Standard_EXPORT void GetDependency(TDF_LabelSequence& theSeq);

  // Add/Remove/Check/Get subshape references
  Standard_EXPORT void AddSubShapeReference   (Handle(GEOM_Function) theSubShape);
  Standard_EXPORT void RemoveSubShapeReference(Handle(GEOM_Function) theSubShape);
  Standard_EXPORT bool HasSubShapeReferences();
  Standard_EXPORT const TDataStd_ListOfExtendedString& GetSubShapeReferences();

  //Returns top label of this function's history tree
  Standard_EXPORT TDF_Label GetHistoryEntry (const Standard_Boolean create = Standard_True);

  //Returns history label, corresponding to the label,
  //on which a reference on argument is stored
  Standard_EXPORT TDF_Label GetArgumentHistoryEntry (const TDF_Label&       theArgumentRefEntry,
                                                     const Standard_Boolean create = Standard_True);

  //Returns top label of this function's naming tree
  Standard_EXPORT TDF_Label GetNamingEntry (const Standard_Boolean create = Standard_True);

  DEFINE_STANDARD_RTTIEXT(GEOM_Function,Standard_Transient)

  private:

  TDF_Label _label;
  bool      _isDone;
  bool      _isCallBackData;
};

#endif
