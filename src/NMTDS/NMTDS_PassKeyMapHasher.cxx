// Copyright (C) 2006 SAMTECH
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
// File:	NMTDS_PassKeyMapHasher.cxx
// Created:	
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <NMTDS_PassKeyMapHasher.ixx>

//=======================================================================
//function : HashCode
//purpose  : 
//=======================================================================
  Standard_Integer NMTDS_PassKeyMapHasher::HashCode(const NMTDS_PassKey& aPKey,
						    const Standard_Integer Upper)
{
  return aPKey.HashCode(Upper);
}
//=======================================================================
//function :IsEqual
//purpose  : 
//=======================================================================
  Standard_Boolean NMTDS_PassKeyMapHasher::IsEqual(const NMTDS_PassKey& aPKey1,
						   const NMTDS_PassKey& aPKey2)
{
  return aPKey1.IsEqual(aPKey2);
}
