// Copyright (C) 2012-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

%module xao
%{
#include "XAO.hxx"
#include "XAO_Exception.hxx"
#include "XAO_XaoUtils.hxx"
#include "XAO_Xao.hxx"
#include "XAO_GeometricElement.hxx"
#include "XAO_Geometry.hxx"
#include "XAO_BrepGeometry.hxx"
#include "XAO_Group.hxx"
#include "XAO_Field.hxx"
#include "XAO_Step.hxx"
#include "XAO_BooleanField.hxx"
#include "XAO_DoubleField.hxx"
#include "XAO_IntegerField.hxx"
#include "XAO_StringField.hxx"
#include "XAO_BooleanStep.hxx"
#include "XAO_DoubleStep.hxx"
#include "XAO_IntegerStep.hxx"
#include "XAO_StringStep.hxx"
%}

%include "std_string.i"
%include "std_vector.i"
%include "std_list.i"
%include "std_map.i"
%include "std_set.i"
%include "exception.i"
namespace std
{
%template(ListGroup)    list<XAO::Group*>;
%template(ListField)    list<XAO::Field*>;
%template(VectorStep)   vector<XAO::Step*>;
%template(SetInt)       set<int>;

%template(VectorBoolean)    vector<bool>;
%template(VectorInteger)    vector<int>;
%template(VectorDouble)     vector<double>;
%template(VectorString)     vector<string>;

%template(VectorVectorBoolean)  vector< vector<bool> >;
%template(VectorVectorInteger)  vector< vector<int> >;
%template(VectorVectorDouble)   vector< vector<double> >;
%template(VectorVectorString)   vector< vector<string> >;
}

%ignore XAO::Xao::addField;
%ignore XAO::Xao::getField;
%ignore XAO::MsgBuilder;
%ignore XAO::XaoUtils;

%include "XAO.hxx"
%include XAO_Exception.hxx
%include XAO_XaoUtils.hxx

%include XAO_Step.hxx
%include XAO_BooleanStep.hxx
%include XAO_DoubleStep.hxx
%include XAO_IntegerStep.hxx
%include XAO_StringStep.hxx

%include XAO_Field.hxx
%include XAO_BooleanField.hxx
%include XAO_DoubleField.hxx
%include XAO_IntegerField.hxx
%include XAO_StringField.hxx

%include XAO_Group.hxx

%include XAO_GeometricElement.hxx
%include XAO_Geometry.hxx
%include XAO_BrepGeometry.hxx
%include XAO_Xao.hxx

