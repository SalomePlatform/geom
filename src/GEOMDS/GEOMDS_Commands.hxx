//  File      : GeomDS_Commands.hxx
//  Created   :
//  Author    : Yves FRICAUD/Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GEOM
//  Copyright : OPEN CASCADE
//  $Header$


#ifndef _GEOMDS_Commands_HeaderFile
#define _GEOMDS_Commands_HeaderFile

#ifndef _TDF_Label_HeaderFile
#include <TDF_Label.hxx>
#endif
class TDF_Label;
class TopoDS_Shape;
class TCollection_ExtendedString;


#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif



//============================================================================
// class    : GEOMDS_Commands
// purpose  :
//============================================================================
class GEOMDS_Commands  {
  
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
  //    inline void  operator delete(void *anAddress, size_t size) 
  //      { 
  //        if (anAddress) Standard::Free((Standard_Address&)anAddress,size); 
  //      }
  // Methods PUBLIC
  // 

  // used for specific entities
  enum GEOMDS_ConstructiveType { VECTOR, LINE, PLANE } ;
  
  // Methods to add or create items in the data structure
  Standard_EXPORT GEOMDS_Commands(const TDF_Label& Main);
  Standard_EXPORT TDF_Label AddShape(const TopoDS_Shape& S,
				     const TCollection_ExtendedString& Name) ;

  Standard_EXPORT TDF_Label Generated(const TopoDS_Shape& S,
				      const TCollection_ExtendedString& Name) ;
  Standard_EXPORT TDF_Label Generated(const TopoDS_Shape& S1,
				      const TopoDS_Shape& S2,
				      const TCollection_ExtendedString& Name) ;

  /* Shapes construction */
  Standard_EXPORT TDF_Label AddIndependentShape(const TopoDS_Shape& S,
						const TCollection_AsciiString& nameIOR) ;
  Standard_EXPORT TDF_Label AddDependentShape(const TopoDS_Shape& S,
					      const TCollection_AsciiString& nameIOR,
					      const TDF_Label& mainLab) ;
  Standard_EXPORT TDF_Label AddConstructiveElement(const TopoDS_Shape& S,
						   const TCollection_ExtendedString& nameIOR,
						   const GEOMDS_ConstructiveType& aType);

  Standard_EXPORT Standard_Boolean AddIORNameAttribute(const TDF_Label& aLabel,
						       const TCollection_ExtendedString& nameIOR) ;

  /* Shapes query */

  Standard_EXPORT Standard_Boolean IsConstructiveElement(const TDF_Label& aLabel) ;

  Standard_EXPORT Standard_Boolean IsConstructiveElement(const TDF_Label& aLabel,
							 TopoDS_Shape& returnTopo,
							 GEOMDS_ConstructiveType& returnType) ;

  Standard_EXPORT Standard_Boolean GetShape(const TDF_Label& aLabel,
					    TopoDS_Shape& returnTopo) ;

  Standard_EXPORT Standard_Boolean IsDependentShape(const TDF_Label& aLabel) ;

  Standard_EXPORT Standard_Boolean GetMainShapeLabel(const TDF_Label& aLabel,
						     TDF_Label& returnMainLabel) ;

  Standard_EXPORT Standard_Boolean ClearAllIOR(const TDF_Label& aLabel) ;

  Standard_EXPORT Standard_Boolean HasIOR(const TDF_Label& aLabel) ;

  Standard_EXPORT Standard_Boolean ReturnNameIOR(const TDF_Label& aLabel,
						 TCollection_ExtendedString& returnNameIOR) ;
  
protected:
  
  // Methods PROTECTED
  // 

  // Fields PROTECTED
  //
  
private: 
  
  // Methods PRIVATE
  // 
  
  // Fields PRIVATE
  //
  
  TDF_Label myLab;
  
};

// other inline functions and methods (like "C++: function call" methods)
//


#endif
