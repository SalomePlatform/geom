// Copyright (C) 2014-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// internal includes
#include "XAOPlugin_IOperations.hxx"
#include "XAOPlugin_Driver.hxx"
#include "XAOPlugin_IImportExport.hxx"

// KERNEL includes
#include <utilities.h>
#include <Utils_SALOME_Exception.hxx>

// GEOM includes
#include <GEOM_PythonDump.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOMImpl_IGroupOperations.hxx>
#include <GEOMImpl_IShapesOperations.hxx>
#include <GEOMImpl_IFieldOperations.hxx>
#include <GEOM_ISubShape.hxx>
#include <GEOM_Object.hxx>
#include <GEOM_Field.hxx>

#include <XAO_Geometry.hxx>
#include <XAO_BrepGeometry.hxx>
#include <XAO_Xao.hxx>
#include <XAO_Group.hxx>
#include <XAO_Field.hxx>
#include <XAO_BooleanField.hxx>
#include <XAO_IntegerField.hxx>
#include <XAO_DoubleField.hxx>
#include <XAO_StringField.hxx>
#include <XAO_DoubleStep.hxx>
#include <XAO_StringStep.hxx>

#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC

// OCC includes
#include <TColStd_HArray1OfByte.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TDataStd_Integer.hxx>


XAO::Dimension shapeEnumToDimension(const TopAbs_ShapeEnum& shape)
{
  XAO::Dimension dim;
  switch( shape ) {
  case TopAbs_VERTEX:
    dim = XAO::VERTEX; break;
  case TopAbs_EDGE:
    dim = XAO::EDGE; break;
  case TopAbs_FACE:
    dim = XAO::FACE; break;
  case TopAbs_SOLID:
    dim = XAO::SOLID; break;
  default:
    throw SALOME_Exception("Bad type"); // TODO
  }
  return dim;
}

TopAbs_ShapeEnum getGroupDimension(XAO::Group* group)
{
  XAO::Dimension dim = group->getDimension();
  TopAbs_ShapeEnum rdim;
  switch ( dim )
  {
  case XAO::VERTEX:
    rdim = TopAbs_VERTEX; break;
  case XAO::EDGE:
    rdim = TopAbs_EDGE; break;
  case XAO::FACE:
    rdim = TopAbs_FACE; break;
  case XAO::SOLID:
    rdim = TopAbs_SOLID; break;
  default:
    rdim = TopAbs_COMPOUND; break;
  }
  return rdim;
}

//=============================================================================
/*!
 *  Constructor
 */
//=============================================================================
XAOPlugin_IOperations::XAOPlugin_IOperations( GEOM_Engine* theEngine, int theDocID )
: GEOMImpl_IBaseIEOperations( theEngine, theDocID )
{
  MESSAGE( "XAOPlugin_IOperations::XAOPlugin_IOperations" );
}

//=============================================================================
/*!
 *  Destructor
 */
//=============================================================================
XAOPlugin_IOperations::~XAOPlugin_IOperations()
{
  MESSAGE( "XAOPlugin_IOperations::~XAOPlugin_IOperations" );
}

bool XAOPlugin_IOperations::exportGroups( std::list<Handle(GEOM_Object)> groupList,
                                          XAO::Xao* xaoObject,
                                          XAO::BrepGeometry* geometry )
{
  // add the groups
  std::list<Handle(GEOM_Object)>::iterator groupIterator = groupList.begin();
  while (groupIterator != groupList.end())
  {
    Handle(GEOM_Object) currGroup = (*groupIterator++);
    if (currGroup->GetType() != GEOM_GROUP) {
      SetErrorCode("Error when export groups: you could perform this operation only with group.");
      return false;
    }
    Handle(TColStd_HArray1OfInteger) groupIds = myGroupOperations->GetObjects(currGroup);

    TopAbs_ShapeEnum shapeGroup = myGroupOperations->GetType(currGroup);
    XAO::Dimension dim = shapeEnumToDimension(shapeGroup);
    XAO::Group* group = xaoObject->addGroup(dim, currGroup->GetName().ToCString());

    switch (shapeGroup)
    {
    case TopAbs_VERTEX:
      for (int i = 1; i <= groupIds->Length(); i++)
      {
        std::string ref = XAO::XaoUtils::intToString(groupIds->Value(i));
        int index = geometry->getVertexIndexByReference(ref);
        group->add(index);
      }
      break;
    case TopAbs_EDGE:
      for (int i = 1; i <= groupIds->Length(); i++)
      {
        std::string ref = XAO::XaoUtils::intToString(groupIds->Value(i));
        int index = geometry->getEdgeIndexByReference(ref);
        group->add(index);
      }
      break;
    case TopAbs_FACE:
      for (int i = 1; i <= groupIds->Length(); i++)
      {
        std::string ref = XAO::XaoUtils::intToString(groupIds->Value(i));
        int index = geometry->getFaceIndexByReference(ref);
        group->add(index);
      }
      break;
    case TopAbs_SOLID:
      for (int i = 1; i <= groupIds->Length(); i++)
      {
        std::string ref = XAO::XaoUtils::intToString(groupIds->Value(i));
        int index = geometry->getSolidIndexByReference(ref);
        group->add(index);
      }
      break;
    }
  }
  return true;
}

void XAOPlugin_IOperations::exportFields( std::list<Handle(GEOM_Field)> fieldList,
                                          XAO::Xao* xaoObject,
                                          XAO::BrepGeometry* geometry )
{
  std::list<Handle(GEOM_Field)>::iterator fieldIterator = fieldList.begin();
  while (fieldIterator != fieldList.end())
  {
    Handle(GEOM_Field) currField = (*fieldIterator++);

    int fdim = currField->GetDimension();
    int ftype = currField->GetDataType();
    int nbComponents = currField->GetNbComponents();
    std::string name = currField->GetName().ToCString();

    XAO::Field* field = xaoObject->addField((XAO::Type)ftype, (XAO::Dimension)fdim, nbComponents, name);

    Handle(TColStd_HArray1OfExtendedString) components = currField->GetComponents();
    for (int i = components->Lower(), j = 0; i <= components->Upper(); ++i, ++j)
    {
      field->setComponentName(j, TCollection_AsciiString(components->Value(i)).ToCString());
    }

    std::list< Handle(GEOM_FieldStep)> steps = currField->GetSteps();
    std::list<Handle(GEOM_FieldStep)>::iterator stepIterator = steps.begin();
    while (stepIterator != steps.end())
    {
      Handle(GEOM_FieldStep) currStep = (*stepIterator++);

      XAO::Step* step = field->addNewStep(currStep->GetID());
      step->setStamp(currStep->GetStamp());

      switch (ftype)
      {
        case 0: // bool
        {
          XAO::BooleanStep* bs = (XAO::BooleanStep*)step;
          Handle(TColStd_HArray1OfInteger) bvalues = currStep->GetIntValues();
          std::vector<bool> bv;
          bv.reserve(bvalues->Upper());
          for ( int i = bvalues->Lower(), nb = bvalues->Upper(); i <= nb; ++i )
          {
            bv.push_back(bvalues->Value(i) != 0);
          }
          bs->setValues(bv);
          break;
        }
        case 1: // integer
        {
          XAO::IntegerStep* is = (XAO::IntegerStep*)step;
          Handle(TColStd_HArray1OfInteger) ivalues = currStep->GetIntValues();
          std::vector<int> iv;
          iv.reserve(ivalues->Upper());
          for ( int i = ivalues->Lower(), nb = ivalues->Upper(); i <= nb; ++i )
          {
            iv.push_back(ivalues->Value(i));
          }
          is->setValues(iv);
          break;
        }
        case 2: // double
        {
          XAO::DoubleStep* ds = (XAO::DoubleStep*)step;
          Handle(TColStd_HArray1OfReal) dvalues = currStep->GetDoubleValues();
          std::vector<double> dv;
          dv.reserve(dvalues->Upper());
          for ( int i = dvalues->Lower(), nb = dvalues->Upper(); i <= nb; ++i )
          {
            dv.push_back(dvalues->Value(i));
          }
          ds->setValues(dv);
            break;
        }
        case 3: // string
        {
          XAO::StringStep* ss = (XAO::StringStep*)step;
          Handle(TColStd_HArray1OfExtendedString) svalues = currStep->GetStringValues();
          std::vector<std::string> sv;
          sv.reserve(svalues->Upper());
          for ( int i = svalues->Lower(), nb = svalues->Upper(); i <= nb; ++i )
          {
            sv.push_back(TCollection_AsciiString(svalues->Value(i)).ToCString());
          }
          ss->setValues(sv);
          break;
        }
      }
    }
  }
}

void XAOPlugin_IOperations::exportSubshapes( const Handle(GEOM_Object)& shape, XAO::BrepGeometry* geometry )
{
  Handle(TColStd_HSequenceOfTransient) subObjects = myShapesOperations->GetExistingSubObjects( shape, GEOMImpl_IShapesOperations::SubShapes );
  int nbSubObjects = subObjects->Length();
  // set the names of the sub shapes
  for (int i = 1; i <= nbSubObjects; i++)
  {
    Handle(Standard_Transient) transientSubObject = subObjects->Value(i);
    if (transientSubObject.IsNull())
      continue;

    Handle(GEOM_Object) subObject = Handle(GEOM_Object)::DownCast( transientSubObject );
    if (subObject->GetType() != GEOM_GROUP)
    {
      int subIndex = myShapesOperations->GetSubShapeIndex( shape, subObject );
      switch (subObject->GetValue().ShapeType())
      {
      case TopAbs_VERTEX:
        geometry->changeVertexName(subIndex, subObject->GetName().ToCString());
        break;
      case TopAbs_EDGE:
        geometry->changeEdgeName(subIndex, subObject->GetName().ToCString());
        break;
      case TopAbs_FACE:
        geometry->changeFaceName(subIndex, subObject->GetName().ToCString());
        break;
      case TopAbs_SOLID:
        geometry->changeSolidName(subIndex, subObject->GetName().ToCString());
        break;
      }
    }
  }
}

//=============================================================================
/*!
 *  Export a shape to XAO format
 *  \param shape The shape to export
 *  \param groups The list of groups to export
 *  \param fields The list of fields to export
 *  \param fileName The name of the file to exported
 *  \return boolean indicating if export was succeful.
 */
//=============================================================================
bool XAOPlugin_IOperations::ExportXAO( Handle(GEOM_Object) shape,
                                       std::list<Handle(GEOM_Object)> groupList,
                                       std::list<Handle(GEOM_Field)> fieldList,
                                       const char* author,
                                       const char* fileName )
{
  SetErrorCode(KO);

  if (shape.IsNull()) return false;

  // add a new shape function with parameters
  Handle(GEOM_Function) lastFunction = shape->GetLastFunction();
  if (lastFunction.IsNull()) return false;

  // add a new result object
  Handle(GEOM_Object) result = GetEngine()->AddObject(GetDocID(), GEOM_IMPORT);

  // add an Export function
  Handle(GEOM_Function) exportFunction = result->AddFunction(XAOPlugin_Driver::GetID(), EXPORT_SHAPE);
  if (exportFunction.IsNull()) return false;
  if (exportFunction->GetDriverGUID() != XAOPlugin_Driver::GetID()) return false;

  // create the XAO object
  XAO::Xao* xaoObject = new XAO::Xao();
  xaoObject->setAuthor(author);

  // add the geometry
  XAO::BrepGeometry* geometry = (XAO::BrepGeometry*)XAO::Geometry::createGeometry(XAO::BREP);
  TopoDS_Shape topoShape = shape->GetValue();
  exportFunction->SetValue(topoShape);
  XAO::BrepGeometry* brep = (XAO::BrepGeometry*)geometry;
  brep->setTopoDS_Shape(topoShape);

  geometry->setName(shape->GetName().ToCString());
  exportSubshapes(shape, geometry);
  xaoObject->setGeometry(geometry);

  if (!exportGroups(groupList, xaoObject, geometry)) return false;
  exportFields(fieldList, xaoObject, geometry);

  // export the XAO to the file
  xaoObject->exportXAO(fileName);

  // make a Python command
  GEOM::TPythonDump pd(exportFunction);
  pd << "exported = geompy.ExportXAO(" << shape;

  // list of groups
  pd << ", [";
  if (groupList.size() > 0)
  {
    std::list<Handle(GEOM_Object)>::iterator itGroup = groupList.begin();
    pd << (*itGroup++);
    while (itGroup != groupList.end())
    {
      pd << ", " << (*itGroup++);
    }
  }

  // list of fields
  pd << "], [";
  if (fieldList.size() > 0)
  {
    std::list<Handle(GEOM_Field)>::iterator itField = fieldList.begin();
    pd << (*itField++);
    while (itField != fieldList.end())
    {
      pd << ", " << (*itField++);
    }
  }
  pd << "], ";
  pd << "\"" << author << "\", \"" << fileName << "\")";

  SetErrorCode(OK);
  delete xaoObject;

  return true;
}

void XAOPlugin_IOperations::importSubShapes( XAO::Geometry* xaoGeometry,
                                             Handle(GEOM_Function) function, int shapeType, int dim,
                                             Handle(TColStd_HSequenceOfTransient)& subShapeList )
{
  Handle(GEOM_Object) subShape;
  Handle(GEOM_Function) aFunction;
  Handle(TColStd_HArray1OfInteger) anArray;

  XAO::GeometricElementList::iterator elementIterator = xaoGeometry->begin((XAO::Dimension)dim);
  for (; elementIterator != xaoGeometry->end((XAO::Dimension)dim); elementIterator++)
  {
    XAO::GeometricElement element = elementIterator->second;
    if (!element.hasName())
      continue;

    std::string name = element.getName();
    std::string ref = element.getReference();
    int iref = XAO::XaoUtils::stringToInt(ref);

    anArray = new TColStd_HArray1OfInteger(1, 1);
    anArray->SetValue(1, iref);

    subShape = GetEngine()->AddObject(GetDocID(), GEOM_SUBSHAPE);
    Handle(GEOM_Function) aFunction = subShape->AddFunction(GEOM_Object::GetSubShapeID(), 1);
    if (aFunction.IsNull())
      return;

    subShape->SetName(name.c_str());
    subShape->SetType(shapeType);

    GEOM_ISubShape aSSI(aFunction);
    aSSI.SetMainShape(function);
    aSSI.SetIndices(anArray);

    //aFunction->SetValue(aValue);
    subShapeList->Append(subShape);

    // Put this subshape in the list of sub-shapes of theMainShape
    function->AddSubShapeReference(aFunction);
  }
}

//=============================================================================
/*!
 *  Import a shape from XAO format
 *  \param fileName The name of the file to import
 *  \param shape The imported shape
 *  \param subShapes The list of imported groups
 *  \param groups The list of imported groups
 *  \param fields The list of imported fields
 *  \return boolean indicating if import was succeful.
 */
//=============================================================================
bool XAOPlugin_IOperations::ImportXAO( const char* fileName,
                                       Handle(GEOM_Object)& shape,
                                       Handle(TColStd_HSequenceOfTransient)& subShapes,
                                       Handle(TColStd_HSequenceOfTransient)& groups,
                                       Handle(TColStd_HSequenceOfTransient)& fields )
{
  SetErrorCode(KO);

  if (fileName == NULL || groups.IsNull() || fields.IsNull())
    return false;

  // Read the XAO
  XAO::Xao* xaoObject = new XAO::Xao();
  try
  {
    xaoObject->importXAO(fileName);
  }
  catch (XAO::XAO_Exception& exc)
  {
    delete xaoObject;
    SetErrorCode(exc.what());
    return false;
  }

  XAO::Geometry* xaoGeometry = xaoObject->getGeometry();
  if (xaoGeometry == NULL)
  {
    delete xaoObject;
    SetErrorCode("Cannot import XAO: geometry format not supported.");
    return false;
  }

  // create the shape
  shape = GetEngine()->AddObject(GetDocID(), GEOM_IMPORT);
  Handle(GEOM_Function) function = shape->AddFunction(XAOPlugin_Driver::GetID(), IMPORT_SHAPE);
  if (function.IsNull()) return false;
  if (function->GetDriverGUID() != XAOPlugin_Driver::GetID()) return false;

  function->SetString( XAOPlugin_Driver::GetFileNameTag(), fileName );

  // set the geometry
  if (xaoGeometry->getFormat() == XAO::BREP)
  {
    XAO::BrepGeometry* brep = (XAO::BrepGeometry*)xaoGeometry;
    TopoDS_Shape geomShape = brep->getTopoDS_Shape();
    function->SetValue(geomShape);
    shape->SetName(xaoGeometry->getName().c_str());
  }
  else
  {
    delete xaoObject;
    SetErrorCode("Cannot import XAO: geometry format not supported.");
    return false;
  }

  // create sub shapes with names
  importSubShapes(xaoGeometry, function, GEOM_POINT, XAO::VERTEX, subShapes);
  importSubShapes(xaoGeometry, function, GEOM_EDGE, XAO::EDGE, subShapes);
  importSubShapes(xaoGeometry, function, GEOM_FACE, XAO::FACE, subShapes);
  importSubShapes(xaoGeometry, function, GEOM_SOLID, XAO::SOLID, subShapes);

  // create groups
  int nbGroups = xaoObject->countGroups();
  for (int i = 0; i < nbGroups; ++i)
  {
    XAO::Group* xaoGroup = xaoObject->getGroup(i);

    // build an array with the indexes of the sub shapes
    int nbElt = xaoGroup->count();
    Handle(TColStd_HArray1OfInteger) array = new TColStd_HArray1OfInteger(1, nbElt);
    int j = 0;
    for (std::set<int>::iterator it = xaoGroup->begin(); it != xaoGroup->end(); ++it)
    {
      int index = (*it);
      std::string ref = xaoGeometry->getElementReference(xaoGroup->getDimension(), index);
      array->SetValue(++j, XAO::XaoUtils::stringToInt(ref));
    }

    // create the group with the array of sub shapes indexes
    Handle(GEOM_Object) group = GetEngine()->AddSubShape(shape, array);
    group->SetType(GEOM_GROUP);
    group->SetName(xaoGroup->getName().c_str());

    // Set a sub-shape type
    TDF_Label freeLabel = group->GetFreeLabel();
    TDataStd_Integer::Set(freeLabel, (Standard_Integer) getGroupDimension(xaoGroup));
    groups->Append(group);

    function = group->GetLastFunction();
  }

  // create the fields
  int nbFields = xaoObject->countFields();
  for (int i = 0; i < nbFields; ++i)
  {
    XAO::Field* xaoField = xaoObject->getField(i);

    Handle(TColStd_HArray1OfExtendedString) components = new TColStd_HArray1OfExtendedString(0, xaoField->countComponents()-1);
    for (int j = 0; j < xaoField->countComponents(); ++j)
    {
        components->SetValue(j, (TCollection_ExtendedString)xaoField->getComponentName(j).c_str());
    }

    Handle(GEOM_Field) field = myFieldOperations->CreateField(shape,
                 xaoField->getName().c_str(),
                 (int)xaoField->getType(),
                 (int)xaoField->getDimension(),
                 components);

    switch (xaoField->getType())
    {
        case XAO::BOOLEAN:
        {
            XAO::BooleanField* bfield = (XAO::BooleanField*)xaoField;
            for (int j = 0; j < xaoField->countSteps(); ++j)
            {
                XAO::BooleanStep* bstep = bfield->getStep(j);
                Handle(GEOM_FieldStep) step = field->AddStep(bstep->getStep(), bstep->getStamp());

                Handle(TColStd_HArray1OfInteger) values = new TColStd_HArray1OfInteger(0, bstep->countValues()-1);
                std::vector<bool> bvalues = bstep->getValues();
                for (int k = 0; k < bstep->countValues(); ++k)
                {
                    values->SetValue(k, bvalues[k] ? 1 : 0);
                }
                step->SetValues(values);
            }
            break;
        }
        case XAO::INTEGER:
        {
            XAO::IntegerField* ifield = (XAO::IntegerField*)xaoField;
            for (int j = 0; j < xaoField->countSteps(); ++j)
            {
                XAO::IntegerStep* istep = ifield->getStep(j);
                Handle(GEOM_FieldStep) step = field->AddStep(istep->getStep(), istep->getStamp());

                Handle(TColStd_HArray1OfInteger) values = new TColStd_HArray1OfInteger(0, istep->countValues()-1);
                std::vector<int> ivalues = istep->getValues();
                for (int k = 0; k < istep->countValues(); ++k)
                {
                    values->SetValue(k, ivalues[k]);
                }
                step->SetValues(values);
            }
            break;
        }
        case XAO::DOUBLE:
        {
            XAO::DoubleField* dfield = (XAO::DoubleField*)xaoField;
            for (int j = 0; j < xaoField->countSteps(); ++j)
            {
                XAO::DoubleStep* dstep = dfield->getStep(j);
                Handle(GEOM_FieldStep) step = field->AddStep(dstep->getStep(), dstep->getStamp());

                Handle(TColStd_HArray1OfReal) values = new TColStd_HArray1OfReal(0, dstep->countValues()-1);
                std::vector<double> dvalues = dstep->getValues();
                for (int k = 0; k < dstep->countValues(); ++k)
                {
                    values->SetValue(k, dvalues[k]);
                }
                step->SetValues(values);
            }
            break;
        }
        case XAO::STRING:
        {
            XAO::StringField* sfield = (XAO::StringField*)xaoField;
            for (int j = 0; j < xaoField->countSteps(); ++j)
            {
                XAO::StringStep* sstep = sfield->getStep(j);
                Handle(GEOM_FieldStep) step = field->AddStep(sstep->getStep(), sstep->getStamp());

                Handle(TColStd_HArray1OfExtendedString) values = new TColStd_HArray1OfExtendedString(0, sstep->countValues()-1);
                std::vector<std::string> svalues = sstep->getValues();
                for (int k = 0; k < sstep->countValues(); ++k)
                {
                    values->SetValue(k, TCollection_ExtendedString(svalues[k].c_str()));
                }
                step->SetValues(values);
            }
            break;
        }
    }

    fields->Append(field);
  }

  // make a Python command
  GEOM::TPythonDump pd(function);
  pd << "(imported, " << shape << ", ";

  // list of sub shapes
  pd << "[";
  int nbSubshapes = subShapes->Length();
  if (nbSubshapes > 0)
  {
    for (int i = 1; i <= nbSubshapes; i++)
    {
      Handle(GEOM_Object) obj = Handle(GEOM_Object)::DownCast(subShapes->Value(i));
      pd << obj << ((i < nbSubshapes) ? ", " : "");
    }
  }
  pd << "], [";

  // list of groups
  if (nbGroups > 0)
  {
    for (int i = 1; i <= nbGroups; i++)
    {
      Handle(GEOM_Object) obj = Handle(GEOM_Object)::DownCast(groups->Value(i));
      pd << obj << ((i < nbGroups) ? ", " : "");
    }
  }

  pd << "], [";

  // list of fields
  if (nbFields > 0)
  {
    for (int i = 1; i <= nbFields; i++)
    {
      Handle(GEOM_Field) obj = Handle(GEOM_Field)::DownCast(fields->Value(i));
      pd << obj << ((i < nbFields) ? ", " : "");
    }
  }
  pd << "]";
  pd << ") = geompy.ImportXAO(\"" << fileName << "\")";

  delete xaoObject;
  SetErrorCode(OK);

  return true;
}
