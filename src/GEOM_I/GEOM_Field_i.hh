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

#ifndef _GEOM_Field_i_HeaderFile
#define _GEOM_Field_i_HeaderFile

#include "GEOMImpl_Gen.hxx"
#include "GEOM_BaseObject_i.hh"
#include "GEOM_Field.hxx"

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Gen)

#include <TopoDS_Shape.hxx>

//================================================================================
/*!
 * \brief Class implementing GEOM::GEOM_Field interface
 */
class GEOM_I_EXPORT GEOM_Field_i : public virtual POA_GEOM::GEOM_Field, public virtual GEOM_BaseObject_i
{
 public:
  GEOM_Field_i(PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine, HANDLE_NAMESPACE(GEOM_Field) theImpl);
  ~GEOM_Field_i();

  virtual GEOM::GEOM_Object_ptr GetShape();
  virtual GEOM::field_data_type GetDataType();
  virtual CORBA::Short GetDimension();
  virtual GEOM::string_array* GetComponents();
  virtual GEOM::GEOM_FieldStep_ptr AddStep(::CORBA::Long stepID, ::CORBA::Long stamp);
  virtual void RemoveStep(::CORBA::Long stepID);
  virtual CORBA::Long CountSteps();
  virtual GEOM::ListOfLong* GetSteps();
  virtual GEOM::GEOM_FieldStep_ptr GetStep(CORBA::Long stepID);
  virtual ::CORBA::Boolean IsShape() { return false; }
  virtual CORBA::Long GetArraySize();

 private:

  HANDLE_NAMESPACE(GEOM_Field) _impl;
};

//================================================================================
/*!
 * \brief Class implementing GEOM::GEOM_FieldStep interface
 */
class GEOM_I_EXPORT GEOM_FieldStep_i : public virtual POA_GEOM::GEOM_FieldStep, public virtual GEOM_BaseObject_i
{
 public:
  GEOM_FieldStep_i(PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine, HANDLE_NAMESPACE(GEOM_FieldStep) theImpl);
  ~GEOM_FieldStep_i();

  virtual void SetStamp(::CORBA::Long stamp);
  virtual ::CORBA::Long GetStamp();
  virtual ::CORBA::Long GetID();
  virtual GEOM::GEOM_Field_ptr GetField();
  virtual ::CORBA::Boolean IsShape() { return false; }

 protected:

  HANDLE_NAMESPACE(GEOM_FieldStep) _impl;
};

//================================================================================
/*!
 * \brief Class implementing GEOM::GEOM_BoolFieldStep interface
 */
class GEOM_I_EXPORT GEOM_BoolFieldStep_i : public virtual POA_GEOM::GEOM_BoolFieldStep, public virtual GEOM_FieldStep_i
{
 public:
  GEOM_BoolFieldStep_i(PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine, HANDLE_NAMESPACE(GEOM_FieldStep) theImpl);

  virtual ::CORBA::Boolean SetValues(const ::GEOM::short_array& boolValues);
  virtual GEOM::short_array* GetValues();
};

//================================================================================
/*!
 * \brief Class implementing GEOM::GEOM_IntFieldStep interface
 */
class GEOM_I_EXPORT GEOM_IntFieldStep_i : public virtual POA_GEOM::GEOM_IntFieldStep, public virtual GEOM_FieldStep_i
{
 public:
  GEOM_IntFieldStep_i(PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine, HANDLE_NAMESPACE(GEOM_FieldStep) theImpl);

  virtual ::CORBA::Boolean SetValues(const ::GEOM::ListOfLong& intValues);
  virtual GEOM::ListOfLong* GetValues();
};

//================================================================================
/*!
 * \brief Class implementing GEOM::GEOM_DoubleFieldStep interface
 */
class GEOM_I_EXPORT GEOM_DoubleFieldStep_i : public virtual POA_GEOM::GEOM_DoubleFieldStep, public virtual GEOM_FieldStep_i
{
 public:
  GEOM_DoubleFieldStep_i(PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine, HANDLE_NAMESPACE(GEOM_FieldStep) theImpl);

  virtual ::CORBA::Boolean SetValues(const ::GEOM::ListOfDouble& doubleValues);
  virtual GEOM::ListOfDouble* GetValues();
};

//================================================================================
/*!
 * \brief Class implementing GEOM::GEOM_StringFieldStep interface
 */
class GEOM_I_EXPORT GEOM_StringFieldStep_i : public virtual POA_GEOM::GEOM_StringFieldStep, public virtual GEOM_FieldStep_i
{
 public:
  GEOM_StringFieldStep_i(PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine, HANDLE_NAMESPACE(GEOM_FieldStep) theImpl);

  virtual ::CORBA::Boolean SetValues(const ::GEOM::string_array& strValues);
  virtual GEOM::string_array* GetValues();
};

#endif
