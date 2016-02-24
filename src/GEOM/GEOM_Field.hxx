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

#ifndef _GEOM_Field_HeaderFile
#define _GEOM_Field_HeaderFile

#include "GEOM_Object.hxx"

#include <list>


const int GEOM_FIELD_OBJTYPE      = 52; // same value #defined in GEOMImpl_Types.hxx
const int GEOM_FIELD_STEP_OBJTYPE = 53;

class GEOM_Field;
class GEOM_FieldStep;
DEFINE_STANDARD_HANDLE( GEOM_Field, GEOM_BaseObject );
DEFINE_STANDARD_HANDLE( GEOM_FieldStep, GEOM_BaseObject );

class GEOM_Field : public GEOM_BaseObject
{
  GEOM_Field(const TDF_Label& theLabel);
  friend class GEOM_Engine;

 public:

  // Function types
  enum { FUN_ADD_FIELD = 1,
         FUN_ADD_STEP,
         FUN_CHANGE_COMP_NAMES,
         //FUN_REMOVE_STEP,
         FUN_CHANGE_STEP_STAMP,
         FUN_CHANGE_VALUE
  };

  // Creates a GEOM_Field on an empty Label
  Standard_EXPORT GEOM_Field(const TDF_Label& theLabel, int );
  Standard_EXPORT ~GEOM_Field();

  // Finds a GEOM_Field on theLabel
  Standard_EXPORT static Handle(GEOM_Field) GetField(const TDF_Label& theLabel);

  // Returns a GUID associated with a field object (GEOM_Field or GEOM_FieldStep)
  // This GUID corresponds to GEOMImpl_FieldDriver::GetID() and
  // it also marks TDataStd_TreeNode on a label of GEOM_FieldStep
  Standard_EXPORT static const Standard_GUID& GetFieldID();

  // Sets the basic data that do not change (except compNames?)
  Standard_EXPORT void Init(const Handle(GEOM_Object)&                     theShape,
                            const char*                                    theName,
                            const int                                      theDataType,
                            const int                                      theDimension,
                            const Handle(TColStd_HArray1OfExtendedString)& theCompNames);

  // Returns a shape this GEOM_Field lies on
  Standard_EXPORT Handle(GEOM_Object) GetShape();

  // Returns a data type of this GEOM_Field
  // 0 - bool, 1 - int, 2 - double, 3 - string
  Standard_EXPORT int GetDataType();

  // Returns one of "Bool","Int","Double","String"
  Standard_EXPORT static TCollection_AsciiString GetDataTypeString(int type);

  // Returns dimension of the shape the field lies on:
  // 0 - VERTEX, 1 - EDGE, 2 - FACE, 3 - SOLID, -1 - whole shape
  Standard_EXPORT int GetDimension();

  // Sets names of components
  Standard_EXPORT void SetComponents( const Handle(TColStd_HArray1OfExtendedString)& compNames );

  // Returns names of components
  Standard_EXPORT Handle(TColStd_HArray1OfExtendedString) GetComponents();

  // Returns names of components in a python syntax
  Standard_EXPORT TCollection_AsciiString GetComponentsForPython();

  // Returns number of components
  Standard_EXPORT int GetNbComponents();

  // Returns number of sub-shapes.
  // Size of data arrays == GetNbSubShapes() * GetComponents()->Extent()
  Standard_EXPORT int GetNbSubShapes();

  // Returns number of sub-shapes of given dimension
  Standard_EXPORT static int GetNbSubShapes(const Handle(GEOM_Object)& shape,
                                            const int                  dim);

  // Returns size of data array == GetNbSubShapes() * GetComponents()->Extent()
  Standard_EXPORT int GetArrayLength();

  // Removes a component. Number counts from one.
  //Standard_EXPORT void RemoveComponent(const int number);

  // Adds a step
  Standard_EXPORT Handle(GEOM_FieldStep) AddStep(const int stepID, const int stamp);

  // Removes a step
  Standard_EXPORT void RemoveStep(const int stepID);

  // Returns a step
  Standard_EXPORT Handle(GEOM_FieldStep) GetStep(const int stepID);
 
  // Returns all steps
  Standard_EXPORT std::list< Handle(GEOM_FieldStep)> GetSteps();

  OCCT_DEFINE_STANDARD_RTTIEXT(GEOM_Field,GEOM_BaseObject)

private:

  int nbSubShapes; // not to explode the shape each time nbSubShapes is needed
};


class GEOM_FieldStep : public GEOM_BaseObject
{
  GEOM_FieldStep(const TDF_Label& theLabel);
  friend class GEOM_Engine;

public:
  // Creates a GEOM_FieldStep on an empty Label
  Standard_EXPORT GEOM_FieldStep(const TDF_Label& theLabel, int );
  Standard_EXPORT ~GEOM_FieldStep();

  // Sets the basic data
  Standard_EXPORT void Init(const Handle(GEOM_Field)& theField,
                            const int                 theID,
                            const int                 theStamp);

  // Finds a GEOM_FieldStep on the label theLabel
  Standard_EXPORT static Handle(GEOM_FieldStep) GetFieldStep(const TDF_Label& theLabel);

  // Returns the Field this GEOM_FieldStep belongs to
  Standard_EXPORT Handle(GEOM_Field) GetField();

  // Returns the stamp step id
  Standard_EXPORT int GetID();

  // Removes a component. Number counts from one.
  //Standard_EXPORT void RemoveComponent(const int number);

  // Sets the stamp of the step
  Standard_EXPORT void SetStamp(const int stamp);

  // Returns the stamp of the step
  Standard_EXPORT int GetStamp();

  // Sets int or bool values. Returns false if number of values is wrong
  Standard_EXPORT bool SetValues( const Handle(TColStd_HArray1OfInteger)& values );

  // Sets double values. Returns false if number of values is wrong
  Standard_EXPORT bool SetValues( const Handle(TColStd_HArray1OfReal)& values );

  // Sets string values. Returns false if number of values is wrong
  Standard_EXPORT bool SetValues( const Handle(TColStd_HArray1OfExtendedString)& values );

  // Returns int or bool values
  Standard_EXPORT Handle(TColStd_HArray1OfInteger) GetIntValues();

  // Returns double values
  Standard_EXPORT Handle(TColStd_HArray1OfReal) GetDoubleValues();

  // Returns string values
  Standard_EXPORT Handle(TColStd_HArray1OfExtendedString) GetStringValues();

  // Returns GUID of CAF data array
  const Standard_GUID& GetDataID();

  OCCT_DEFINE_STANDARD_RTTIEXT(GEOM_FieldStep,GEOM_BaseObject)

private:

  Handle(GEOM_Function) getFunctionToSetValuesAndDump(const TCollection_AsciiString& valueStr);

};


#endif
