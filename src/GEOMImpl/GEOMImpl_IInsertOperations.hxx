
#ifndef _GEOMImpl_IInsertOperations_HXX_
#define _GEOMImpl_IInsertOperations_HXX_

#include "Utils_SALOME_Exception.hxx"
#include "GEOM_IOperations.hxx"
#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"
#include <TDocStd_Document.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Resource_Manager.hxx>

class GEOMImpl_IInsertOperations : public GEOM_IOperations {
 public:
  GEOMImpl_IInsertOperations(GEOM_Engine* theEngine, int theDocID);
  ~GEOMImpl_IInsertOperations();


  Handle(GEOM_Object) MakeCopy (Handle(GEOM_Object) theOriginal);
  
  Handle(GEOM_Object) Import (const char* theFileName, const char* theFormatType);
  
  void Export (const Handle(GEOM_Object) theOriginal,
	       const char*               theFileName,
	       const char*               theFormatType);

  Standard_Boolean ImportTranslators (Handle(TColStd_HSequenceOfAsciiString)& theFormats,
                                      Handle(TColStd_HSequenceOfAsciiString)& thePatterns);

  Standard_Boolean ExportTranslators (Handle(TColStd_HSequenceOfAsciiString)& theFormats,
                                      Handle(TColStd_HSequenceOfAsciiString)& thePatterns);

  Standard_Boolean IsSupported (const Standard_Boolean isImport,
                                const TCollection_AsciiString theFormat,
                                Handle(TCollection_HAsciiString)& theLibName);

 private:
  Standard_Boolean InitResMgr ();

 private:
  Handle(Resource_Manager) myResMgr;
};

#endif
