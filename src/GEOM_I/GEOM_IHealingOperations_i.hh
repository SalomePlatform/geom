
#ifndef _GEOM_IHealingOperations_i_HeaderFile
#define _GEOM_IHealingOperations_i_HeaderFile


#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include "GEOMImpl_IHealingOperations.hxx"

#include <TColStd_HArray1OfExtendedString.hxx>
#include <TColStd_HArray1OfInteger.hxx>

class GEOM_IHealingOperations_i : 
    public virtual POA_GEOM::GEOM_IHealingOperations,
    public virtual GEOM_IOperations_i
{
 public:
   GEOM_IHealingOperations_i(PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine, ::GEOMImpl_IHealingOperations* theImpl);
   ~GEOM_IHealingOperations_i();
  
   GEOM::GEOM_Object_ptr ProcessShape(GEOM::GEOM_Object_ptr theObject, const GEOM::string_array& theOperations, const GEOM::string_array& theParams, const GEOM::string_array& theValues);

   void GetShapeProcessParameters(GEOM::string_array_out theOperations, GEOM::string_array_out theParams, GEOM::string_array_out theValues);

   GEOM::GEOM_Object_ptr SuppressFaces(GEOM::GEOM_Object_ptr theObject, const GEOM::short_array& theFaces);

   GEOM::GEOM_Object_ptr CloseContour (GEOM::GEOM_Object_ptr theObject, const GEOM::short_array& theWires, CORBA::Boolean isCommonVertex);

   GEOM::GEOM_Object_ptr RemoveIntWires (GEOM::GEOM_Object_ptr theObject, const GEOM::short_array& theWires);
   
   GEOM::GEOM_Object_ptr FillHoles (GEOM::GEOM_Object_ptr theObject, const GEOM::short_array& theWires);

   GEOM::GEOM_Object_ptr Sew (GEOM::GEOM_Object_ptr theObject, CORBA::Double theTolerance);

   GEOM::GEOM_Object_ptr DivideEdge (GEOM::GEOM_Object_ptr theObject, CORBA::Short theIndex, CORBA::Double theValue, CORBA::Boolean isByParameter);        

   CORBA::Boolean GetFreeBoundary(GEOM::GEOM_Object_ptr theObject, GEOM::ListOfGO_out theClosedWires, GEOM::ListOfGO_out theOpenWires );
   
   ::GEOMImpl_IHealingOperations* GetOperations() { return (::GEOMImpl_IHealingOperations*)GetImpl(); }

private:
	 Handle(TColStd_HArray1OfExtendedString) Convert( const GEOM::string_array& );
	 Handle(TColStd_HArray1OfInteger)        Convert( const GEOM::short_array& );

};

#endif
