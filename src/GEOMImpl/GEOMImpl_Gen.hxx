
#ifndef _GEOMImpl_GEN_HXX_
#define _GEOMImpl_GEN_HXX_

#include <map>

#include "GEOMImpl_IBasicOperations.hxx"
#include "GEOMImpl_ITransformOperations.hxx"
#include "GEOMImpl_I3DPrimOperations.hxx"
#include "GEOMImpl_IShapesOperations.hxx"
#include "GEOMImpl_IBlocksOperations.hxx"
#include "GEOMImpl_IBooleanOperations.hxx"
#include "GEOMImpl_IHealingOperations.hxx"
#include "GEOMImpl_ICurvesOperations.hxx"
#include "GEOMImpl_ILocalOperations.hxx"
#include "GEOMImpl_IInsertOperations.hxx"
#include "GEOMImpl_IMeasureOperations.hxx"
#include "GEOMImpl_IGroupOperations.hxx"
#include "GEOM_Engine.hxx"

class GEOMImpl_Gen : public GEOM_Engine
{
 public:
  GEOMImpl_Gen();
  ~GEOMImpl_Gen();

  GEOMImpl_IBasicOperations* GetIBasicOperations(int theDocID);

  GEOMImpl_ITransformOperations* GetITransformOperations(int theDocID);

  GEOMImpl_I3DPrimOperations* GetI3DPrimOperations(int theDocID);

  GEOMImpl_IShapesOperations* GetIShapesOperations(int theDocID);

  GEOMImpl_IBlocksOperations* GetIBlocksOperations(int theDocID);

  GEOMImpl_IMeasureOperations* GetIMeasureOperations(int theDocID);

  GEOMImpl_IBooleanOperations* GetIBooleanOperations(int theDocID);

  GEOMImpl_ICurvesOperations* GetICurvesOperations(int theDocID);

  GEOMImpl_ILocalOperations* GetILocalOperations(int theDocID);

  GEOMImpl_IInsertOperations* GetIInsertOperations(int theDocID);

  GEOMImpl_IHealingOperations* GetIHealingOperations(int theDocID);

  GEOMImpl_IGroupOperations* GetIGroupOperations(int theDocID);

 private:

  std::map <int, GEOMImpl_IBasicOperations*>     _mapOfBasicOperations;
  std::map <int, GEOMImpl_ITransformOperations*> _mapOfTransformOperations;
  std::map <int, GEOMImpl_I3DPrimOperations*>    _mapOf3DPrimOperations;
  std::map <int, GEOMImpl_IShapesOperations*>    _mapOfShapesOperations;
  std::map <int, GEOMImpl_IBlocksOperations*>    _mapOfBlocksOperations;
  std::map <int, GEOMImpl_IBooleanOperations*>   _mapOfBooleanOperations;
  std::map <int, GEOMImpl_IHealingOperations*>   _mapOfHealingOperations;
  std::map <int, GEOMImpl_ICurvesOperations*>    _mapOfCurvesOperations;
  std::map <int, GEOMImpl_ILocalOperations*>     _mapOfLocalOperations;
  std::map <int, GEOMImpl_IInsertOperations*>    _mapOfInsertOperations;
  std::map <int, GEOMImpl_IMeasureOperations*>   _mapOfMeasureOperations;
  std::map <int, GEOMImpl_IGroupOperations*>   _mapOfGroupOperations;
};

#endif
