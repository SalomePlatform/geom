// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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

#ifndef _GEOM_IHealingOperations_i_HeaderFile
#define _GEOM_IHealingOperations_i_HeaderFile

#include "GEOMImpl_Gen.hxx"

#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include "GEOMImpl_IHealingOperations.hxx"

#include <TColStd_HArray1OfExtendedString.hxx>
#include <TColStd_HArray1OfInteger.hxx>

class GEOM_I_EXPORT GEOM_IHealingOperations_i : 
    public virtual POA_GEOM::GEOM_IHealingOperations,
    public virtual GEOM_IOperations_i
{
 public:
   GEOM_IHealingOperations_i(PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine, ::GEOMImpl_IHealingOperations* theImpl);
   ~GEOM_IHealingOperations_i();
  
   GEOM::GEOM_Object_ptr ProcessShape(GEOM::GEOM_Object_ptr theObject, const GEOM::string_array& theOperations, const GEOM::string_array& theParams, const GEOM::string_array& theValues);

   void GetShapeProcessParameters(GEOM::string_array_out theOperations, GEOM::string_array_out theParams, GEOM::string_array_out theValues);

   void GetOperatorParameters (const char* theOperator,  GEOM::string_array_out theParams, GEOM::string_array_out theValues);

   GEOM::GEOM_Object_ptr SuppressFaces(GEOM::GEOM_Object_ptr theObject, const GEOM::short_array& theFaces);

   GEOM::GEOM_Object_ptr CloseContour (GEOM::GEOM_Object_ptr theObject, const GEOM::short_array& theWires, CORBA::Boolean isCommonVertex);

   GEOM::GEOM_Object_ptr RemoveIntWires (GEOM::GEOM_Object_ptr theObject, const GEOM::short_array& theWires);
   
   GEOM::GEOM_Object_ptr FillHoles (GEOM::GEOM_Object_ptr theObject, const GEOM::short_array& theWires);

   GEOM::GEOM_Object_ptr Sew (GEOM::GEOM_Object_ptr theObject, CORBA::Double theTolerance);

   GEOM::GEOM_Object_ptr DivideEdge (GEOM::GEOM_Object_ptr theObject, CORBA::Short theIndex, CORBA::Double theValue, CORBA::Boolean isByParameter);        

   CORBA::Boolean GetFreeBoundary(GEOM::GEOM_Object_ptr theObject, GEOM::ListOfGO_out theClosedWires, GEOM::ListOfGO_out theOpenWires );
   
   GEOM::GEOM_Object_ptr ChangeOrientation (GEOM::GEOM_Object_ptr theObject);
   GEOM::GEOM_Object_ptr ChangeOrientationCopy (GEOM::GEOM_Object_ptr theObject);

   ::GEOMImpl_IHealingOperations* GetOperations() { return (::GEOMImpl_IHealingOperations*)GetImpl(); }

private:
	 Handle(TColStd_HArray1OfExtendedString) Convert( const GEOM::string_array& );
	 Handle(TColStd_HArray1OfInteger)        Convert( const GEOM::short_array& );

};

#endif
