
#ifndef _GEOM_Engine_HXX_
#define _GEOM_Engine_HXX_

#include "GEOM_Application.hxx"
#include <Interface_DataMapOfIntegerTransient.hxx> 
#include <TDF_Label.hxx>
#include <TDocStd_Document.hxx>
#include "GEOM_Object.hxx"
#include <TColStd_HArray1OfInteger.hxx>
#include <TopAbs.hxx>

#include "GEOM_DataMapOfAsciiStringTransient.hxx"

class GEOM_Engine
{
 public:
  GEOM_Engine();
  ~GEOM_Engine() { _mapIDDocument.Clear(); _objects.Clear(); }

  //Retuns the engine    
  static GEOM_Engine* GetEngine();   

  //Returns the OCAF document by its ID, if document doesn't exists it will be created
  Handle(TDocStd_Document) GetDocument(int theDocID);

  //Returns the ID of the given OCAF document
  int GetDocID(Handle(TDocStd_Document) theDocument);
  
  //Returns the OCAF appliaction
  Handle(TDocStd_Application) GetApplication() { return _OCAFApp; }

  //Returns a pointer to GEOM_Object defined by a document and the entry
  Handle(GEOM_Object) GetObject(int theDocID, char* theEntry);
  
  //Adds a new object of the type theType in the OCAF document
  Handle(GEOM_Object) AddObject(int theDocID, int theType);

  //Removes the object from the OCAF document
  bool RemoveObject(Handle(GEOM_Object) theObject);  

  //Saves the OCAF document with ID = theDocID with file with name theFileName
  bool Save(int theDocID, char* theFileName);
  
  //Loads the OCAF document into the application and assigns to it an ID = theDocID
  bool Load(int theDocID, char* theFileName);

  //Closes the document with ID =  theDocID
  void Close(int theDocID);

  //Sets the number of Undos (default value = 10)
  void SetUndoLimit(int theLimit) { _UndoLimit = theLimit; }

  //Applies an Undo to document with ID = theDocID
  void Undo(int theDocID);

  //Applies an Redo to document with ID = theDocID
  void Redo(int theDocID);

  //Adds a new sub shape object of the MainShape object
  Handle(GEOM_Object) AddSubShape(Handle(GEOM_Object) theMainShape, Handle(TColStd_HArray1OfInteger) theIndices);

 protected:
  static void SetEngine(GEOM_Engine* theEngine);       

 private:

  Handle(GEOM_Application)  _OCAFApp;
  Interface_DataMapOfIntegerTransient _mapIDDocument;
  int _UndoLimit;
  GEOM_DataMapOfAsciiStringTransient _objects;
};

#endif
