// File:	GEOMAlgo_PassKeyMapHasher.cxx
// Created:	
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <GEOMAlgo_PassKeyMapHasher.ixx>

//=======================================================================
//function : HashCode
//purpose  : 
//=======================================================================
  Standard_Integer GEOMAlgo_PassKeyMapHasher::HashCode(const GEOMAlgo_PassKey& aPKey,
						       const Standard_Integer Upper)
{
  return aPKey.HashCode(Upper);
}
//=======================================================================
//function :IsEqual
//purpose  : 
//=======================================================================
  Standard_Boolean GEOMAlgo_PassKeyMapHasher::IsEqual(const GEOMAlgo_PassKey& aPKey1,
						      const GEOMAlgo_PassKey& aPKey2)
{
  return aPKey1.IsEqual(aPKey2);
}
