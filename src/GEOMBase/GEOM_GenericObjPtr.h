// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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
//  File   : GEOM_GenericObjPtr.h
//  Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#ifndef GEOM_GenericObjPtr_H
#define GEOM_GenericObjPtr_H

#include "GEOM_GEOMBase.hxx"

#include "SALOMEconfig.h"
#include CORBA_SERVER_HEADER(GEOM_Gen)

namespace GEOM
{
  /*!
    \class GenericObjPtr
    \brief A smart pointer for the SALOME GenericObj interface.

    This class can be used in conjunction with the references to the CORBA objects which
    interfaces are inherited from the SALOME::GenericObj CORBA interface.

    The smart pointer class automatically invokes Register() / UnRegister() functions of th
    interface in order to prevent memory leaks and other such problems caused by improper
    usage of the CORBA references.

    Smart pointers can be easily copied, stored within class as data members, passed to the
    functions requiring native CORBA reference as parameters, etc.

    Usage:
    - If you want to assign the smart pointer to the CORBA _var type variable, use copy()
    function to make a copy of the stored CORBA object. Otherwise you might cause Segmentation
    Fault error.
    - To pass the smart pointer to the function that requires CORBA _ptr type parameter,
    use get() function.
    - If you want to take an ownership on the CORBA object, use take() function.
    In case of SALOME Generic object this is useful when some function returns newly created
    object that should be removed by the caller as soon as the object is no more required.
    For example, function GetSubShape() of the GEOM_IShapesOperation interface always creates
    new servant object and returns new object reference to it. If the object is not published
    in the study, it has to be destroyed and the coresponding servant should be deleted.
    
    Examples:
    \code
    typedef GEOM::GenericObjPtr<MyInterface> MyIPtr;
    void MyInterface_ptr foo();
    void MyInterface_ptr bar( MyInterface_ptr p );

    MyIPtr v1;                       // create empty (nil) pointer
    MyIPtr v2 = foo();               // get some CORBA reference and store it within the smart pointer
    v1 = v2;                         // copy smart pointer (reference counter is incremented)
    v2 = bar( v1.get() );            // pass smart pointer to the function
    MyInterface_var var = v2.copy(); // initialize _var variable with the smart pointer contents
    v1.take( foo() );                // take ownership on the newly created object
    \endcode
   */
  
  template <typename TInterface> class GenericObjPtr
  {
    typedef typename TInterface::_var_type TInterfaceVar;
    typedef typename TInterface::_ptr_type TInterfacePtr;
    
  private:
    TInterfaceVar myObject;

  private:
    //! Increment counter for the object.
    void Register()
    {
      if ( !CORBA::is_nil( this->myObject ) )
        this->myObject->Register();
    }

    //! Decrement counter for the object.
    void UnRegister()
    {
      if ( !CORBA::is_nil( this->myObject ) ) {
        this->myObject->UnRegister();
        this->myObject = TInterface::_nil();
      }
    }
      
  public:
    //! Initialize pointer to nil generic object reference.
    GenericObjPtr()
    {}
    
    //! Initialize pointer to the given generic object reference.
    GenericObjPtr( TInterfacePtr theObject )
    {
      this->myObject = TInterface::_duplicate( theObject );
      this->Register();
    }
    
    //! Initialize pointer with a new reference to the same object referenced by given pointer.
    GenericObjPtr( const GenericObjPtr& thePointer )
    {
      this->myObject = thePointer.myObject;
      this->Register();
    }
    
    //! Destroy pointer and remove the reference to the object.
    ~GenericObjPtr()
    {
      this->UnRegister();
    }
    
    //! Assign object to reference and remove reference to an old object.
    GenericObjPtr& operator=( TInterfacePtr theObject )
    {
      this->UnRegister();
      this->myObject = TInterface::_duplicate( theObject );
      this->Register();
      return *this;
    }

    //! Assign object to reference and remove reference to an old object.
    GenericObjPtr& operator=( const GenericObjPtr& thePointer )
    {
      this->UnRegister();
      this->myObject = thePointer.myObject;
      this->Register();
      return *this;
    }

    static bool isSame( TInterfacePtr theLeft, TInterfacePtr theRight )
    {
      return theLeft->_is_equivalent( theRight );
    }

    //! Check equivalence
    bool operator==( TInterfacePtr theObject )
    {
      return isSame( this->myObject, theObject );
    }

    //! Check equivalence
    bool operator==( const GenericObjPtr& thePointer )
    {
      return isSame( this->myObject, thePointer.get() );;
    }

    //! Check difference
    bool operator!=( TInterfacePtr theObject )
    {
      return !isSame( this->myObject, theObject );
    }

    //! Check difference
    bool operator!=( const GenericObjPtr& thePointer )
    {
      return !isSame( this->myObject, thePointer.get() );;
    }

    //! Provides normal pointer target member access using operator ->.
    TInterfacePtr operator->() const
    {
      return this->get();
    }

    //! Check validity of the pointer.
    operator bool() const
    {
       return !this->isNull();
    }

    //! Initialize pointer to the given generic object reference and take ownership on it.
    void take( TInterfacePtr theObject )
    {
      this->UnRegister();
      this->myObject = TInterface::_duplicate( theObject );
    }

    //! Get the contained object.
    TInterfacePtr get() const
    {
      return this->myObject;
    }

    //! Make the copy of the contained object and return it (caller becomes owner of the CORBA reference).
    TInterfacePtr copy() const
    {
      return TInterface::_duplicate( this->myObject );
    }
    
    //! Check if pointer is null.
    bool isNull() const
    {
      return CORBA::is_nil( this->myObject );
    }

    //! Nullify pointer.
    void nullify()
    {
      this->UnRegister();
    }
  };
  
  typedef GenericObjPtr<GEOM::GEOM_Object>               GeomObjPtr;
  typedef GenericObjPtr<GEOM::GEOM_Field>                GeomFieldPtr;
  typedef GenericObjPtr<GEOM::GEOM_IBasicOperations>     BasicOpPtr;
  typedef GenericObjPtr<GEOM::GEOM_ITransformOperations> TransformOpPtr;
  typedef GenericObjPtr<GEOM::GEOM_I3DPrimOperations>    I3DPrimOpPtr;
  typedef GenericObjPtr<GEOM::GEOM_IShapesOperations>    ShapesOpPtr;
  typedef GenericObjPtr<GEOM::GEOM_IBlocksOperations>    BlocksOpPtr;
  typedef GenericObjPtr<GEOM::GEOM_IBooleanOperations>   BooleanOpPtr;
  typedef GenericObjPtr<GEOM::GEOM_ICurvesOperations>    CurvesOpPtr;
  typedef GenericObjPtr<GEOM::GEOM_ILocalOperations>     LocalOpPtr;
  typedef GenericObjPtr<GEOM::GEOM_IHealingOperations>   HealingOpPtr;
  typedef GenericObjPtr<GEOM::GEOM_IInsertOperations>    InsertOpPtr;
  typedef GenericObjPtr<GEOM::GEOM_IMeasureOperations>   MeasureOpPtr;
  typedef GenericObjPtr<GEOM::GEOM_IGroupOperations>     GroupOpPtr;

  template<> bool GEOMBASE_EXPORT GenericObjPtr<GEOM::GEOM_Object>::isSame( GEOM::GEOM_Object_ptr theLeft, GEOM::GEOM_Object_ptr theRight );
}

#endif // GEOM_GenericObjPtr_H
