// Copyright (C) 2013-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include <Standard_Stream.hxx>

#include <GEOMImpl_XAODriver.hxx>
#include <GEOMImpl_IImportExportXAO.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOM_Function.hxx>

#include <TFunction_Logbook.hxx>
#include <StdFail_NotDone.hxx>

#include "XAO_Xao.hxx"
#include "XAO_Geometry.hxx"
#include "XAO_Group.hxx"
#include "XAO_XaoUtils.hxx"
//@@ include required header files here @@//

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_XAODriver::GetID()
{
    static Standard_GUID aGUID("FF1BBB71-5D14-4df2-980B-3A668264EA16");
    return aGUID;
}

//=======================================================================
//function : GEOMImpl_XAODriver
//purpose  :
//=======================================================================
GEOMImpl_XAODriver::GEOMImpl_XAODriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_XAODriver::Execute(TFunction_Logbook& log) const
{
    if (Label().IsNull()) return 0;
    Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

    GEOMImpl_IImportExportXAO iexao(function);
    TCollection_AsciiString xao   = iexao.GetData();

    TopoDS_Shape shape;

    Standard_Integer functionType = function->GetType();
    if (functionType == IMPORTEXPORT_EXPORTXAO)
    {
    }
    else if (functionType == IMPORTEXPORT_IMPORTXAO)
    {
    }
    else
    {
        // other construction modes here
    }

    if (shape.IsNull()) return 0;

    function->SetValue(shape);

    log.SetTouched(Label());

    return 1;
}

//=======================================================================
//function :  GEOMImpl_XAODriver_Type_
//purpose  :
//=======================================================================
Standard_EXPORT Handle_Standard_Type& GEOMImpl_XAODriver_Type_()
{
    static Handle_Standard_Type aType1 = STANDARD_TYPE(TFunction_Driver);
    if (aType1.IsNull()) aType1 = STANDARD_TYPE(TFunction_Driver);
    static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
    if (aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared);
    static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
    if (aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);

    static Handle_Standard_Transient _Ancestors[] = { aType1, aType2, aType3, NULL };
    static Handle_Standard_Type _aType = new Standard_Type("GEOMImpl_XAODriver",
            sizeof(GEOMImpl_XAODriver),
            1,
            (Standard_Address) _Ancestors,
            (Standard_Address) NULL);
    return _aType;
}

//=======================================================================
//function : DownCast
//purpose  :
//=======================================================================
const Handle(GEOMImpl_XAODriver) Handle(GEOMImpl_XAODriver)::DownCast(
        const Handle(Standard_Transient)& AnObject)
{
    Handle(GEOMImpl_XAODriver) _anOtherObject;

    if (!AnObject.IsNull())
    {
        if (AnObject->IsKind(STANDARD_TYPE(GEOMImpl_XAODriver)))
        {
            _anOtherObject = Handle(GEOMImpl_XAODriver)((Handle(GEOMImpl_XAODriver)&) AnObject);
        }
    }

    return _anOtherObject;
}
