%module xao
%{
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

