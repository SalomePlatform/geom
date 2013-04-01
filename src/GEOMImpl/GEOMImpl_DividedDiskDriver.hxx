// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#ifndef _GEOMImpl_DividedDiskDriver_HXX
#define _GEOMImpl_DividedDiskDriver_HXX

#include <TFunction_Driver.hxx>

class Handle_Standard_Type;
class GEOMImpl_DividedDiskDriver;
class TopoDS_Shape;
class TopoDS_Shell;
class gp_Pnt;
class gp_Dir;
class gp_Ax3;

Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(GEOMImpl_DividedDiskDriver);

class Handle(GEOMImpl_DividedDiskDriver) : public Handle(TFunction_Driver) {
public:
  inline void* operator new(size_t,void* anAddress) 
  {
    return anAddress;
  }
  inline void* operator new(size_t size) 
  { 
    return Standard::Allocate(size); 
  }
  inline void  operator delete(void *anAddress) 
  { 
    if (anAddress) Standard::Free((Standard_Address&)anAddress); 
  }
  
  Handle(GEOMImpl_DividedDiskDriver)():Handle(TFunction_Driver)() {} 
  Handle(GEOMImpl_DividedDiskDriver)(const Handle(GEOMImpl_DividedDiskDriver)& aHandle) : Handle(TFunction_Driver)(aHandle) 
  {}

  Handle(GEOMImpl_DividedDiskDriver)(const GEOMImpl_DividedDiskDriver* anItem) : Handle(TFunction_Driver)((TFunction_Driver *)anItem) 
  {}

  Handle(GEOMImpl_DividedDiskDriver)& operator=(const Handle(GEOMImpl_DividedDiskDriver)& aHandle)
  {
    Assign(aHandle.Access());
    return *this;
  }
  
  Handle(GEOMImpl_DividedDiskDriver)& operator=(const GEOMImpl_DividedDiskDriver* anItem)
  {
    Assign((Standard_Transient *)anItem);
    return *this;
  }
  
  GEOMImpl_DividedDiskDriver* operator->() 
  {
    return (GEOMImpl_DividedDiskDriver *)ControlAccess();
  }
  
  GEOMImpl_DividedDiskDriver* operator->() const 
  {
    return (GEOMImpl_DividedDiskDriver *)ControlAccess();
  }
  
  Standard_EXPORT ~Handle(GEOMImpl_DividedDiskDriver)() {};
  
  Standard_EXPORT static const Handle(GEOMImpl_DividedDiskDriver) DownCast(const Handle(Standard_Transient)& AnObject);
};

class GEOMImpl_DividedDiskDriver : public TFunction_Driver {
public:
  inline void* operator new(size_t,void* anAddress) 
  {
    return anAddress;
  }
  inline void* operator new(size_t size) 
  { 
    return Standard::Allocate(size); 
  }
  inline void  operator delete(void *anAddress) 
  { 
    if (anAddress) Standard::Free((Standard_Address&)anAddress); 
  }
  
  // Methods PUBLIC
  // 
  Standard_EXPORT GEOMImpl_DividedDiskDriver();
  Standard_EXPORT virtual  Standard_Integer Execute(TFunction_Logbook& log) const; 
  Standard_EXPORT virtual void Validate(TFunction_Logbook&) const {}
  Standard_EXPORT Standard_Boolean MustExecute(const TFunction_Logbook&) const
  {
    return Standard_True;
  }
  Standard_EXPORT static const Standard_GUID& GetID();
  Standard_EXPORT ~GEOMImpl_DividedDiskDriver() {};
  
  // Type management
  //
  Standard_EXPORT friend Handle_Standard_Type& GEOMImpl_DividedDiskDriver_Type_();
  Standard_EXPORT const Handle(Standard_Type)& DynamicType() const
  {
    return STANDARD_TYPE(GEOMImpl_DividedDiskDriver);
  }
  Standard_EXPORT Standard_Boolean IsKind(const Handle(Standard_Type)& AType) const
  {
    return (STANDARD_TYPE(GEOMImpl_DividedDiskDriver) == AType || TFunction_Driver::IsKind(AType));
  } 
  
private:
  TopoDS_Shape TransformShape  (TopoDS_Shape aShape, int theOrientation) const;
  TopoDS_Shape TransformShape  (TopoDS_Shape aShape, gp_Pnt P, gp_Dir V) const;
  TopoDS_Shape WPlaneTransform (TopoDS_Shape aShape, gp_Ax3 theWPlane) const;
  TopoDS_Shell MakeDiskHexagon (double R, double Ratio) const;
  TopoDS_Shape MakeDiskSquare  (double R, double Ratio) const;
};

#endif // _GEOMImpl_DividedDiskDriver_HXX
