//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

//  File   : GEOMImpl_IAdvancedOperations.hxx
//  Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#ifndef _GEOMImpl_IAdvancedOperations_HXX_
#define _GEOMImpl_IAdvancedOperations_HXX_

#include <Utils_SALOME_Exception.hxx>
#include "GEOM_IOperations.hxx"
#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"

#define Handle_GEOM_Object Handle(GEOM_Object)
#define Handle_TColStd_HSequenceOfTransient Handle(TColStd_HSequenceOfTransient)
#define Handle_TColStd_HSequenceOfInteger Handle(TColStd_HSequenceOfInteger)

class GEOMImpl_IAdvancedOperations: public GEOM_IOperations {
private:
	bool MakePipeTShapePartition(/*std::vector<GEOM_IOperations*> theOperations, */Handle_GEOM_Object theShape, double theR1, double theW1, double theL1, double theR2,
			double theW2, double theL2, double theH = 0, double theW = 0, double theRF = 0, bool isNormal = true);
    bool MakePipeTShapeMirrorAndGlue(/*std::vector<GEOM_IOperations*> theOperations, */Handle_GEOM_Object theShape, double theR1, double theW1, double theL1, double theR2,
            double theW2, double theL2);
	bool MakeGroups(/*std::vector<GEOM_IOperations*> theOperations, */Handle_GEOM_Object theShape, int shapType, double theR1, double theW1, double theL1, double theR2,
			double theW2, double theL2, Handle_TColStd_HSequenceOfTransient theSeq, gp_Trsf aTrsf);
	gp_Trsf GetPositionTrsf(double theL1, double theL2, Handle_GEOM_Object P1 = NULL, Handle_GEOM_Object P2 = NULL,
			Handle_GEOM_Object P3 = NULL);
    bool CheckCompatiblePosition(double& theL1, double& theL2, Handle_GEOM_Object theP1, Handle_GEOM_Object theP2,
            Handle_GEOM_Object theP3, double theTolerance);

public:
	Standard_EXPORT
	GEOMImpl_IAdvancedOperations(GEOM_Engine* theEngine, int theDocID);Standard_EXPORT
	~GEOMImpl_IAdvancedOperations();

	Standard_EXPORT Handle_TColStd_HSequenceOfTransient MakePipeTShape(double theR1, double theW1, double theL1,
			double theR2, double theW2, double theL2, bool theHexMesh = true);
	Standard_EXPORT Handle_TColStd_HSequenceOfTransient
	MakePipeTShapeWithPosition(double theR1, double theW1, double theL1, double theR2, double theW2, double theL2,
			bool theHexMesh = true, Handle_GEOM_Object P1 = NULL, Handle_GEOM_Object P2 = NULL, Handle_GEOM_Object P3 =
					NULL);
	Standard_EXPORT Handle_TColStd_HSequenceOfTransient MakePipeTShapeChamfer(double theR1, double theW1, double theL1,
			double theR2, double theW2, double theL2, double theH, double theW, bool theHexMesh = true);
	Standard_EXPORT Handle_TColStd_HSequenceOfTransient
	MakePipeTShapeChamferWithPosition(double theR1, double theW1, double theL1, double theR2, double theW2,
			double theL2, double theH, double theW, bool theHexMesh = true, Handle_GEOM_Object P1 = NULL,
			Handle_GEOM_Object P2 = NULL, Handle_GEOM_Object P3 = NULL);
	Standard_EXPORT Handle_TColStd_HSequenceOfTransient MakePipeTShapeFillet(double theR1, double theW1, double theL1,
			double theR2, double theW2, double theL2, double theRF, bool theHexMesh = true);
	Standard_EXPORT Handle_TColStd_HSequenceOfTransient
	MakePipeTShapeFilletWithPosition(double theR1, double theW1, double theL1, double theR2, double theW2,
			double theL2, double theRF, bool theHexMesh = true, Handle_GEOM_Object P1 = NULL, Handle_GEOM_Object P2 =
					NULL, Handle_GEOM_Object P3 = NULL);
	/*@@ insert new functions before this line @@*/
};

#undef Handle_GEOM_Object
#undef Handle_TColStd_HSequenceOfTransient
#undef Handle_TColStd_HSequenceOfInteger

#endif