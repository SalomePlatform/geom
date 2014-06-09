#ifndef OCC2VTK_INTERNAL_H
#define OCC2VTK_INTERNAL_H

#include <NCollection_BaseCollection.hxx>
#include <NCollection_BaseList.hxx>
#include <NCollection_TListNode.hxx>
#include <NCollection_TListIterator.hxx>

#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx> 
#include <TopoDS_Face.hxx> 

template <class TheItemType> class GEOM_Set
  : public NCollection_BaseCollection<TheItemType>,
    public NCollection_BaseList
{
public:
  typedef NCollection_TListNode<TheItemType> SetNode;
  typedef NCollection_TListIterator<TheItemType> Iterator;

public:
  //! Constructor
  GEOM_Set(const Handle(NCollection_BaseAllocator)& theAllocator=0L) :
    NCollection_BaseCollection<TheItemType>(theAllocator),
    NCollection_BaseList() {}

  //! Copy constructor
  GEOM_Set (const GEOM_Set& theOther) :
    NCollection_BaseCollection<TheItemType>(theOther.myAllocator),
    NCollection_BaseList()
  { *this = theOther; }

  //! Size - Number of items
  virtual Standard_Integer Size (void) const
  { return Extent(); }

  //! Replace this list by the items of theOther collection
  virtual void Assign (const NCollection_BaseCollection<TheItemType>& theOther)
  {
    if (this == &theOther) 
      return;
    Clear();
    TYPENAME NCollection_BaseCollection<TheItemType>::Iterator& anIter = 
      theOther.CreateIterator();
    for (; anIter.More(); anIter.Next())
    {
      SetNode* pNew = new (this->myAllocator) SetNode(anIter.Value());
      PAppend (pNew);
    }
  }

  //! Replace this list by the items of theOther Set
  GEOM_Set& operator= (const GEOM_Set& theOther)
  { 
    if (this == &theOther) 
      return *this;
    Clear ();
    SetNode * pCur = (SetNode *) theOther.PFirst();
    while (pCur)
    {
      SetNode* pNew = new (this->myAllocator) SetNode(pCur->Value());
      PAppend (pNew);
      pCur = (SetNode *) pCur->Next();
    }
    return *this;
  }

  //! Clear this set
  void Clear (void)
  { PClear (SetNode::delNode, this->myAllocator); }

  //! Add item
  Standard_Boolean Add (const TheItemType& theItem)
  { 
    Iterator anIter(*this);
    while (anIter.More())
    {
      if (anIter.Value() == theItem)
        return Standard_False;
      anIter.Next();
    }
    SetNode * pNew = new (this->myAllocator) SetNode(theItem);
    PPrepend (pNew);
    return Standard_True;
  }

  //! Remove item
  Standard_Boolean Remove (const TheItemType& theItem)
  {
    Iterator anIter(*this);
    while (anIter.More())
    {
      if (anIter.Value() == theItem)
      {
        PRemove (anIter, SetNode::delNode, this->myAllocator);
        return Standard_True;
      }
      anIter.Next();
    }
    return Standard_False;
  }

  //! Remove - wrapper against 'hiding' warnings
  void Remove (Iterator& theIter) 
  { NCollection_BaseList::PRemove (theIter,
                                   SetNode::delNode,
                                   this->myAllocator); }

  //! Contains - item inclusion query
  Standard_Boolean Contains (const TheItemType& theItem) const
  {
    Iterator anIter(*this);
    for (; anIter.More(); anIter.Next())
      if (anIter.Value() == theItem)
        return Standard_True;
    return Standard_False;
  }

  //! IsASubset
  Standard_Boolean IsASubset (const GEOM_Set& theOther)
  { 
    if (this == &theOther) 
      return Standard_True;
    Iterator anIter(theOther);
    for (; anIter.More(); anIter.Next())
      if (!Contains(anIter.Value()))
        return Standard_False;
    return Standard_True;
  }

  //! IsAProperSubset
  Standard_Boolean IsAProperSubset (const GEOM_Set& theOther)
  {
    if (myLength <= theOther.Extent())
      return Standard_False;
    Iterator anIter(theOther);
    for (; anIter.More(); anIter.Next())
      if (!Contains(anIter.Value()))
        return Standard_False;
    return Standard_True;
  }

  //! Union
  void Union (const GEOM_Set& theOther)
  { 
    if (this == &theOther) 
      return;
    Iterator anIter(theOther);
    Iterator aMyIter;
    Standard_Integer i, iLength=myLength;
    for (; anIter.More(); anIter.Next())
    {
      Standard_Boolean isIn=Standard_False;
      const TheItemType& theItem = anIter.Value();
      for (aMyIter.Init(*this), i=1; 
           i<=iLength;
           aMyIter.Next(), i++)
        if (theItem == aMyIter.Value())
          isIn = Standard_True;
      if (!isIn)
      {
        SetNode * pNew = new (this->myAllocator) SetNode(theItem);
        PAppend (pNew);
      }
    }
  }

  //! Intersection
  void Intersection (const GEOM_Set& theOther)
  { 
    if (this == &theOther) 
      return;
    Iterator anIter(*this);
    while (anIter.More())
      if (theOther.Contains(anIter.Value()))
        anIter.Next();
      else
        NCollection_BaseList::PRemove (anIter, SetNode::delNode, this->myAllocator);
  }

  //! Difference (Subtraction)
  void Difference (const GEOM_Set& theOther)
  { 
    if (this == &theOther) 
      return;
    Iterator anIter(*this);
    while (anIter.More())
      if (theOther.Contains(anIter.Value()))
        NCollection_BaseList::PRemove (anIter, SetNode::delNode, this->myAllocator);
      else
        anIter.Next();
  }

  //! Destructor - clears the List
  ~GEOM_Set (void)
  { Clear(); }

private:
  //! Creates Iterator for use on BaseCollection
  virtual TYPENAME NCollection_BaseCollection<TheItemType>::Iterator& 
    CreateIterator(void) const
  { return *(new (this->IterAllocator()) Iterator(*this)); }

};

typedef GEOM_Set<TopoDS_Vertex> TVertexSet; 
typedef GEOM_Set<TopoDS_Edge> TEdgeSet; 
typedef GEOM_Set<TopoDS_Face> TFaceSet; 

class VertexSourceInternal
{
public:
  TVertexSet myVertexSet;
};

class EdgeSourceInternal
{
public:
  TEdgeSet myEdgeSet;
};

class FaceSourceInternal
{
public:
  TFaceSet myFaceSet;
};

#endif // OCC2VTK_INTERNAL_H
