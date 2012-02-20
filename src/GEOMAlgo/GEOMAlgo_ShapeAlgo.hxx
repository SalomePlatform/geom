// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef _GEOMAlgo_ShapeAlgo_HeaderFile
#define _GEOMAlgo_ShapeAlgo_HeaderFile

#include <GEOMAlgo_Algo.hxx>

#include <Basics_OCCTVersion.hxx>

#include <Standard.hxx>
#include <Standard_Macro.hxx>

#include <TopoDS_Shape.hxx>

#include <Standard_Real.hxx>

#if OCC_VERSION_LARGE > 0x06050200
#include <Handle_IntTools_Context.hxx>
#else
#include <IntTools_Context.hxx>
#endif

class IntTools_Context;
class TopoDS_Shape;

class GEOMAlgo_ShapeAlgo  : public GEOMAlgo_Algo {

public:

  void* operator new(size_t,void* anAddress)
  {
    return anAddress;
  }
  void* operator new(size_t size)
  {
    return Standard::Allocate(size);
  }
  void  operator delete(void *anAddress)
  {
    if (anAddress) Standard::Free((Standard_Address&)anAddress);
  }

#if OCC_VERSION_LARGE > 0x06050200
  //! Sets cashed geometrical tools <br>
  Standard_EXPORT     void SetContext(const Handle(IntTools_Context)& theContext) ;
  //! Returns cashed geometrical tools <br>
  Standard_EXPORT    const Handle_IntTools_Context& Context() const;

  Standard_EXPORT   virtual  void Perform() ;
#endif

  Standard_EXPORT   void SetShape(const TopoDS_Shape& aS) ;

  Standard_EXPORT   void SetTolerance(const Standard_Real aT) ;

  Standard_EXPORT  const TopoDS_Shape& Shape() const;

  Standard_EXPORT   Standard_Real Tolerance() const;

  Standard_EXPORT  const TopoDS_Shape& Result() const;

protected:


  Standard_EXPORT GEOMAlgo_ShapeAlgo();
  Standard_EXPORT virtual ~GEOMAlgo_ShapeAlgo();


  TopoDS_Shape myShape;
  Standard_Real myTolerance;
  TopoDS_Shape myResult;
#if OCC_VERSION_LARGE > 0x06050200
  Handle_IntTools_Context myContext;
#else
  IntTools_Context myContext;
#endif

private:

};

// other Inline functions and methods (like "C++: function call" methods)

#endif
