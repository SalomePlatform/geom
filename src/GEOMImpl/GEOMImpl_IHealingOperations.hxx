
#ifndef _GEOMImpl_IHealingOperations_HXX_
#define _GEOMImpl_IHealingOperations_HXX_

#include "GEOM_IOperations.hxx"
#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"

#include <TColStd_HArray1OfExtendedString.hxx>
#include <TColStd_HArray1OfInteger.hxx>

#include <list>

class GEOMImpl_IHealingOperations : public GEOM_IOperations {
 public:
  GEOMImpl_IHealingOperations(GEOM_Engine* theEngine, int theDocID);
  ~GEOMImpl_IHealingOperations();

  // Apply Shape Processing to the selected Object
  Handle(GEOM_Object) ShapeProcess( Handle(GEOM_Object) theObject,
                                    const Handle(TColStd_HArray1OfExtendedString)& theOperations,
                                    const Handle(TColStd_HArray1OfExtendedString)& theParams,
                                    const Handle(TColStd_HArray1OfExtendedString)& theValues );

  // Retrieve default Shape Process parameters (from resource file)
	void GetShapeProcessParameters( list<string>& theOperations,
                                 	list<string>& theParams,
                                  list<string>& theValues );

  // returns all parameters that are valid for the given operation (Shape Process operator)
  static bool GetParameters( const string theOperation, list<string>& theParams );                                      

  Handle(GEOM_Object) SuppressFaces( Handle(GEOM_Object) theObject,
                                     const Handle(TColStd_HArray1OfInteger)& theFaces);

  Handle(GEOM_Object) CloseContour( Handle(GEOM_Object) theObject,
                                    const Handle(TColStd_HArray1OfInteger)& theWires,
                                    bool isCommonVertex );

  Handle(GEOM_Object) RemoveIntWires( Handle(GEOM_Object) theObject,
                                      const Handle(TColStd_HArray1OfInteger)& theWires);

  Handle(GEOM_Object) FillHoles( Handle(GEOM_Object) theObject,
                                 const Handle(TColStd_HArray1OfInteger)& theWires);
                                      
  Handle(GEOM_Object) Sew( Handle(GEOM_Object) theObject,
                           double theTolerance );

  Handle(GEOM_Object) DivideEdge( Handle(GEOM_Object) theObject,
                                  int theIndex,  
                                  double theValue,
                                  bool isByParameter );

  // this function does not use Function-Driver mechanism, it just computes the free
  // boundary edges and returns them in the sequence.  It is called just for information reasons
  // and it's not intended for history/undo/redo/etc..
  bool GetFreeBoundary ( Handle(GEOM_Object) theObject, 
                         Handle(TColStd_HSequenceOfTransient)& theOutClosedWires, 
                         Handle(TColStd_HSequenceOfTransient)& theOutOpenWires );
};

#endif
