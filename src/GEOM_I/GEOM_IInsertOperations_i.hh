
#ifndef _GEOM_IInsertOperations_i_HeaderFile
#define _GEOM_IInsertOperations_i_HeaderFile


#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include "GEOMImpl_IInsertOperations.hxx"

class GEOM_IInsertOperations_i : 
    public virtual POA_GEOM::GEOM_IInsertOperations,
    public virtual GEOM_IOperations_i
{
 public:
  GEOM_IInsertOperations_i (PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine,
			   ::GEOMImpl_IInsertOperations* theImpl);
  ~GEOM_IInsertOperations_i();

  GEOM::GEOM_Object_ptr MakeCopy (GEOM::GEOM_Object_ptr theOriginal);

  void                  Export (GEOM::GEOM_Object_ptr theOriginal,
				const char*           theFileName,
				const char*           theFormatName);

  GEOM::GEOM_Object_ptr Import (const char* theFileName,
				const char* theFormatName);

  void ImportTranslators (GEOM::string_array_out theFormats,
			  GEOM::string_array_out thePatterns);

  void ExportTranslators (GEOM::string_array_out theFormats,
			  GEOM::string_array_out thePatterns);

  ::GEOMImpl_IInsertOperations* GetOperations()
  { return (::GEOMImpl_IInsertOperations*)GetImpl(); }
};

#endif
