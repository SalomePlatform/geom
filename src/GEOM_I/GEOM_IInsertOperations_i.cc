using namespace std; 

#include "GEOM_IInsertOperations_i.hh"

#include "utilities.h"
#include "OpUtil.hxx"
#include "Utils_ExceptHandlers.hxx"

#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"

#include <TColStd_HSequenceOfAsciiString.hxx>

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
GEOM_IInsertOperations_i::GEOM_IInsertOperations_i (PortableServer::POA_ptr thePOA,
						    GEOM::GEOM_Gen_ptr theEngine,
						    ::GEOMImpl_IInsertOperations* theImpl)
     :GEOM_IOperations_i(thePOA, theEngine, theImpl)
{
  MESSAGE("GEOM_IInsertOperations_i::GEOM_IInsertOperations_i");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOM_IInsertOperations_i::~GEOM_IInsertOperations_i()
{
  MESSAGE("GEOM_IInsertOperations_i::~GEOM_IInsertOperations_i");
}


//=============================================================================
/*!
 *  MakeCopy
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IInsertOperations_i::MakeCopy(GEOM::GEOM_Object_ptr theOriginal)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theOriginal == NULL) return aGEOMObject._retn();

  //Get the reference shape
  Handle(GEOM_Object) anOriginal =
    GetOperations()->GetEngine()->GetObject(theOriginal->GetStudyID(),
					    theOriginal->GetEntry());

  if (anOriginal.IsNull()) return aGEOMObject._retn();

  //Create the copy
  Handle(GEOM_Object) anObject = GetOperations()->MakeCopy(anOriginal);

  if (!GetOperations()->IsDone() || anObject.IsNull()) return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  Export
 */
//=============================================================================
void GEOM_IInsertOperations_i::Export
                   (GEOM::GEOM_Object_ptr theOriginal,
		    const char*           theFileName,
		    const char*           theFormatName)
{
  GEOM::GEOM_Object_var aGEOMObject = GEOM::GEOM_Object::_duplicate(theOriginal);

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theOriginal == NULL) return;

  //Get the reference shape
  Handle(GEOM_Object) anOriginal =
    GetOperations()->GetEngine()->GetObject(theOriginal->GetStudyID(),
					    theOriginal->GetEntry());

  if (anOriginal.IsNull()) return;

  //Export the shape to the file
  char* aFileName   = strdup(theFileName);
  char* aFormatName = strdup(theFormatName);
  GetOperations()->Export(anOriginal, aFileName, aFormatName);
  free(aFileName);
  free(aFormatName);

  return;
}

//=============================================================================
/*!
 *  Import
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IInsertOperations_i::Import
                   (const char* theFileName,
		    const char* theFormatName)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Import the shape from the file
  char* aFileName   = strdup(theFileName);
  char* aFormatName = strdup(theFormatName);
  Handle(GEOM_Object) anObject = GetOperations()->Import(aFileName, aFormatName);
  free(aFileName);
  free(aFormatName);

  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  ImportTranslators
 */
//=============================================================================
void GEOM_IInsertOperations_i::ImportTranslators
  (GEOM::string_array_out theFormats, GEOM::string_array_out thePatterns)
{
  // Get sequences of available formats
  Handle(TColStd_HSequenceOfAsciiString) aFormats  = new TColStd_HSequenceOfAsciiString;
  Handle(TColStd_HSequenceOfAsciiString) aPatterns = new TColStd_HSequenceOfAsciiString;
  if (!GetOperations()->ImportTranslators(aFormats, aPatterns)) return;

  const int formSize = aFormats->Length(), pattSize = aPatterns->Length();
  if (formSize != pattSize) return;

  // allocate the CORBA arrays
  GEOM::string_array_var aFormatsArray  = new GEOM::string_array();
  GEOM::string_array_var aPatternsArray = new GEOM::string_array();
  aFormatsArray->length(formSize);
  aPatternsArray->length(formSize);

  // fill the local CORBA arrays with values from sequences
  CORBA::Long i = 1;
  for (; i <= formSize; i++) {
    aFormatsArray[i-1]  = CORBA::string_dup(aFormats->Value(i).ToCString());
    aPatternsArray[i-1] = CORBA::string_dup(aPatterns->Value(i).ToCString());
  }

  // initialize out-parameters with local arrays
  theFormats = aFormatsArray._retn();
  thePatterns = aPatternsArray._retn();
}

//=============================================================================
/*!
 *  ExportTranslators
 */
//=============================================================================
void GEOM_IInsertOperations_i::ExportTranslators
  (GEOM::string_array_out theFormats, GEOM::string_array_out thePatterns)
{
  // Get sequences of available formats
  Handle(TColStd_HSequenceOfAsciiString) aFormats  = new TColStd_HSequenceOfAsciiString;
  Handle(TColStd_HSequenceOfAsciiString) aPatterns = new TColStd_HSequenceOfAsciiString;
  if (!GetOperations()->ExportTranslators(aFormats, aPatterns)) return;

  const int formSize = aFormats->Length(), pattSize = aPatterns->Length();
  if (formSize != pattSize) return;

  // allocate the CORBA arrays
  GEOM::string_array_var aFormatsArray  = new GEOM::string_array();
  GEOM::string_array_var aPatternsArray = new GEOM::string_array();
  aFormatsArray->length(formSize);
  aPatternsArray->length(formSize);

  // fill the local CORBA arrays with values from sequences
  CORBA::Long i = 1;
  for (; i <= formSize; i++) {
    aFormatsArray[i-1]  = CORBA::string_dup(aFormats->Value(i).ToCString());
    aPatternsArray[i-1] = CORBA::string_dup(aPatterns->Value(i).ToCString());
  }

  // initialize out-parameters with local arrays
  theFormats = aFormatsArray._retn();
  thePatterns = aPatternsArray._retn();
}
