
#ifndef _GEOM_Function_HeaderFile
#define _GEOM_Function_HeaderFile

#ifndef _Standard_TypeMismatch_HeaderFile
#include <Standard_TypeMismatch.hxx>
#endif
#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif
#ifndef _Handle_MMgt_TShared_HeaderFile
#include <Handle_MMgt_TShared.hxx>
#endif  
#ifndef _MMgt_TShared_HeaderFile
#include <MMgt_TShared.hxx>
#endif  
#ifndef _Standard_GUID_HeaderFile
#include <Standard_GUID.hxx>
#endif
#ifndef _TopoDS_Shape_HeaderFile
#include <TopoDS_Shape.hxx>
#endif
#ifndef _TDF_Label_HeaderFile
#include <TDF_Label.hxx>
#endif
#ifndef _TDF_LabelSequence_HeaderFile
#include <TDF_LabelSequence.hxx>
#endif
#ifndef _TColStd_HArray1OfExtendedString_HeaderFile
#include <TColStd_HArray1OfExtendedString.hxx>
#endif

class Handle_TColStd_HArray1OfReal;
class Handle_TColStd_HArray1OfInteger;
class Handle_TColStd_HSequenceOfTransient;
class Handle_TColStd_HArray1OfExtendedString;

class Standard_Transient;
class Handle_Standard_Type;
class Handle(MMgt_TShared);
class GEOM_Function;

#include <TCollection_AsciiString.hxx>


Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(GEOM_Function);

class Handle(GEOM_Function) : public Handle(MMgt_TShared) {
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

    Handle(GEOM_Function)():Handle(MMgt_TShared)() {} 
    Handle(GEOM_Function)(const Handle(GEOM_Function)& aHandle) : Handle(MMgt_TShared)(aHandle) 
     {
     }

    Handle(GEOM_Function)(const GEOM_Function* anItem) : Handle(MMgt_TShared)((MMgt_TShared *)anItem) 
     {
     }

    Handle(GEOM_Function)& operator=(const Handle(GEOM_Function)& aHandle)
     {
      Assign(aHandle.Access());
      return *this;
     }

    Handle(GEOM_Function)& operator=(const GEOM_Function* anItem)
     {
      Assign((Standard_Transient *)anItem);
      return *this;
     }

    GEOM_Function* operator->() 
     {
      return (GEOM_Function *)ControlAccess();
     }

    GEOM_Function* operator->() const 
     {
      return (GEOM_Function *)ControlAccess();
     }

   Standard_EXPORT ~Handle(GEOM_Function)() {};
 
   Standard_EXPORT static const Handle(GEOM_Function) DownCast(const Handle(Standard_Transient)& AnObject);
};

class GEOM_Function : public MMgt_TShared
{

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

    // Type management
    //
    Standard_EXPORT friend Handle_Standard_Type& GEOM_Function_Type_();
    Standard_EXPORT const Handle(Standard_Type)& DynamicType() const  { return STANDARD_TYPE(GEOM_Function) ; }
    Standard_EXPORT Standard_Boolean IsKind(const Handle(Standard_Type)& AType) const { return (STANDARD_TYPE(GEOM_Function) == AType || MMgt_TShared::IsKind(AType)); } 


private:
  GEOM_Function(const TDF_Label& theEntry) { _label = theEntry; }

public:

  //Returns a GUID for a function tree  
  static const Standard_GUID& GetFunctionTreeID();

  //Returns the ID which is associated with a reference to another function  
  static const Standard_GUID& GetDependencyID();

  //Finds and returns a function located on a label theEntry
  static Handle(GEOM_Function) GetFunction(const TDF_Label& theEntry);


  GEOM_Function(const TDF_Label& theEntry, const Standard_GUID& theGUID, int theType); 
  ~GEOM_Function() {;}

  TDF_Label GetOwnerEntry();

  //Access to properties 

  //Returns a result of the function built by the function Driver
  TopoDS_Shape GetValue();

  //Sets the function result
  void SetValue(TopoDS_Shape& theShape);

  //Returns a function entry in the OCAF document
  TDF_Label& GetEntry() { return _label; }

  //Returns the type of the function
  int GetType();    

  //Returns a function Driver GUID
  Standard_GUID GetDriverGUID();

  //Returns aPython description of the function
  TCollection_AsciiString GetDescription();

  //Sets aPython description of the function  
  void SetDescription(TCollection_AsciiString& theDescription);  

  //Access to arguments

  //Sets a real argument at position thePosition
  void SetReal(int thePosition, double theValue);

  //Returns a real argument at position thePosition
  double GetReal(int thePosition);

  //Sets a real array argument at position thePosition
  void SetRealArray(int thePosition, const Handle(TColStd_HArray1OfReal)& theArray);

  //Returns a real array argument at position thePosition
  Handle(TColStd_HArray1OfReal) GetRealArray(int thePosition);

  //Sets an integer argument at position thePosition
  void SetInteger(int thePosition, int theValue);

  //Returns an integer argument at position thePosition
  int GetInteger(int thePosition);  
  
  //Sets an integer array argument at position thePosition
  void SetIntegerArray(int thePosition, const Handle(TColStd_HArray1OfInteger)& theArray);

  //Returns an integer array argument at position thePosition
  Handle(TColStd_HArray1OfInteger) GetIntegerArray(int thePosition);

  //Sets a reference to other function argument at position thePosition
  void SetReference(int thePosition, Handle(GEOM_Function) theReference);

  //Sets a string argument at position thePosition
  void SetString(int thePosition, const TCollection_AsciiString& theValue);

  //Returns a string argument at position thePosition
  TCollection_AsciiString GetString(int thePosition);  
  
  //Returns a reference to other function argument at position thePosition
  Handle(GEOM_Function) GetReference(int thePosition); 

  //Set an array of ExtendedString
  void SetStringArray(int thePosition, const Handle(TColStd_HArray1OfExtendedString)& theArray);
    
  //Returns the array of ExtendedString
  Handle(TColStd_HArray1OfExtendedString) GetStringArray(int thePosition);

  //Returns a GUID for a references tree  
  static const Standard_GUID& GetReferencesTreeID();

  //Sets a list of references to other function arguments at position thePosition
  void SetReferenceList (int thePosition,
                         const Handle(TColStd_HSequenceOfTransient)& theRefList);

  //Returns a list of references to other function arguments at position thePosition
  Handle(TColStd_HSequenceOfTransient) GetReferenceList (int thePosition); 

  //Sets a TopoDS_Shape argument at position thePosition
  void SetShape(int thePosition, const TopoDS_Shape& theShape);
    
  //Returns a TopoDS_Shape argument at position thePosition
  TopoDS_Shape GetShape(int thePosition);  
  
  //Returns true if the last method succided 
  bool IsDone() { return _isDone; }

  //Returns a sequence of the external dependencies of this function
 void GetDependency(TDF_LabelSequence& theSeq);

private:

  
  TDF_Label    _label;
  bool _isDone;
};

#endif
