// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "GEOM_WireframeFace.h"

#include <GEOMUtils_Hatcher.hxx>
 
#include <vtkObjectFactory.h> 
 
#include <vtkPoints.h> 
#include <vtkCellArray.h> 

#include <vtkPolyDataMapper.h>  
#include <vtkPolyData.h>  
#include <vtkInformation.h>
#include <vtkInformationVector.h>
 
#include <Adaptor3d_HCurve.hxx>
#include <BRep_Tool.hxx>
#include <TColStd_Array1OfReal.hxx>

vtkStandardNewMacro(GEOM_WireframeFace);
 
GEOM_WireframeFace::GEOM_WireframeFace(): 
  Discret(15)
{ 
  NbIso[0] = 1;
  NbIso[1] = 1;

  this->SetNumberOfInputPorts(0);
} 
 
GEOM_WireframeFace::~GEOM_WireframeFace() 
{ 
} 
 
int GEOM_WireframeFace::RequestData(vtkInformation *vtkNotUsed(request),
                                    vtkInformationVector **vtkNotUsed(inputVector),
                                    vtkInformationVector *outputVector)
{
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkPolyData *aPolyData = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  aPolyData->Allocate();
  vtkPoints* aPts = vtkPoints::New();
  aPolyData->SetPoints(aPts);
  aPts->Delete();

  TFaceSet::Iterator anIter(myFaceSet);
  for(; anIter.More(); anIter.Next()){
    const TopoDS_Face& aFace = anIter.Value();
    OCC2VTK(aFace,aPolyData,aPts,NbIso,Discret);
  }
  return 1;
}

void GEOM_WireframeFace::SetNbIso(const int theNb[2])
{
  if ( theNb[0] == NbIso[0] && theNb[1] == NbIso[1])
    return;

  NbIso[0] = theNb[0];
  NbIso[1] = theNb[1];

  Modified();
}

void GEOM_WireframeFace::GetNbIso(int &theNbU,int &theNbV)
{
  theNbU = NbIso[0];
  theNbV = NbIso[1];
}

void  
GEOM_WireframeFace:: 
OCC2VTK(const TopoDS_Face& theFace,
        vtkPolyData* thePolyData,
                    vtkPoints* thePts,  
        const int theNbIso[2], 
        const int theDiscret) 
{ 
  TopoDS_Face aFace = theFace; 
  aFace.Orientation(TopAbs_FORWARD);
  CreateIso(aFace,theNbIso,theDiscret,thePolyData,thePts);
}

void
GEOM_WireframeFace::
CreateIso(const TopoDS_Face& theFace,
          const int theNbIso[2],
          const int theDiscret,
          vtkPolyData* thePolyData,
          vtkPoints* thePts)
{
  GEOMUtils::Hatcher aHatcher(theFace);

  aHatcher.Init(theNbIso[0], theNbIso[1]);
  aHatcher.Perform();

  if (aHatcher.IsDone()) {
    // Push iso lines in vtk kernel
    CreateIso(aHatcher, Standard_True, theDiscret, thePolyData, thePts);
    CreateIso(aHatcher, Standard_False, theDiscret, thePolyData, thePts);
  }
}



void
GEOM_WireframeFace::
CreateIso(const GEOMUtils::Hatcher &theHatcher,
          const Standard_Boolean   IsUIso,
          const int                theDiscret,
          vtkPolyData              *thePolyData,
          vtkPoints                *thePts)
{
  Handle(TColStd_HArray1OfInteger) anIndices;
  Handle(TColStd_HArray1OfReal)    aParams;

  if (IsUIso) {
    // U-isolines
    anIndices = theHatcher.GetUIndices();
    aParams   = theHatcher.GetUParams();
  } else {
    // V-isolines
    anIndices = theHatcher.GetVIndices();
    aParams   = theHatcher.GetVParams();
  }

  if (anIndices.IsNull() == Standard_False &&
      aParams.IsNull()   == Standard_False) {
    const GeomAbs_IsoType aType    = (IsUIso ? GeomAbs_IsoU : GeomAbs_IsoV);
    Standard_Integer      anIsoInd = anIndices->Lower();

    for (; anIsoInd <= anIndices->Upper(); anIsoInd++) {
      const Standard_Integer aHatchingIndex = anIndices->Value(anIsoInd);

      if (aHatchingIndex != 0) {
        const Standard_Real    aParam     = aParams->Value(anIsoInd);
        const Standard_Integer aNbDomains =
          theHatcher.GetNbDomains(aHatchingIndex);

        if (aNbDomains >= 0) {
          Standard_Integer anIDom = 1;
          Standard_Real    aV1;
          Standard_Real    aV2;

          for (; anIDom <= aNbDomains; anIDom++) {
            if (theHatcher.GetDomain(aHatchingIndex, anIDom, aV1, aV2)) {
              CreateIso_(theHatcher.GetFace(), aType, aParam, aV1, aV2,
                         theDiscret, thePolyData, thePts);
            }
          }
        }
      }
    }
  }
}



void 
GEOM_WireframeFace:: 
CreateIso_(const TopoDS_Face& theFace,
           GeomAbs_IsoType theIsoType, 
           Standard_Real Par, 
           Standard_Real T1,
           Standard_Real T2,
           const int theDiscret, 
           vtkPolyData* thePolyData,
           vtkPoints* thePts)
{
  Standard_Real U1, U2, V1, V2, stepU=0., stepV=0.;
  Standard_Integer j;
  gp_Pnt P;

  TopLoc_Location aLoc;
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(theFace,aLoc);

  if(!S.IsNull()){
    BRepAdaptor_Surface S(theFace,Standard_False);
      
    GeomAbs_SurfaceType SurfType = S.GetType();

    GeomAbs_CurveType CurvType = GeomAbs_OtherCurve;

    Standard_Integer Intrv, nbIntv;
    Standard_Integer nbUIntv = S.NbUIntervals(GeomAbs_CN);
    Standard_Integer nbVIntv = S.NbVIntervals(GeomAbs_CN);
    TColStd_Array1OfReal TI(1,Max(nbUIntv, nbVIntv)+1);

    if(theIsoType == GeomAbs_IsoU){
      S.VIntervals(TI, GeomAbs_CN);
      V1 = Max(T1, TI(1));
      V2 = Min(T2, TI(2));
      U1 = Par;
      U2 = Par;
      stepU = 0;
      nbIntv = nbVIntv;
    }else{
      S.UIntervals(TI, GeomAbs_CN);
      U1 = Max(T1, TI(1));
      U2 = Min(T2, TI(2));
      V1 = Par;
      V2 = Par;
      stepV = 0;
      nbIntv = nbUIntv;
    }   
        
    S.D0(U1,V1,P);
    MoveTo(P,thePts);

    for(Intrv = 1; Intrv <= nbIntv; Intrv++){
      if(TI(Intrv) <= T1 && TI(Intrv + 1) <= T1)
        continue;
      if(TI(Intrv) >= T2 && TI(Intrv + 1) >= T2)
              continue;
      if(theIsoType == GeomAbs_IsoU){
              V1 = Max(T1, TI(Intrv));
              V2 = Min(T2, TI(Intrv + 1));
              stepV = (V2 - V1) / theDiscret;
      }else{
              U1 = Max(T1, TI(Intrv));
              U2 = Min(T2, TI(Intrv + 1));
              stepU = (U2 - U1) / theDiscret;
      }

      switch (SurfType) {
      case GeomAbs_Plane :
              break;
      case GeomAbs_Cylinder :
      case GeomAbs_Cone :
        if(theIsoType == GeomAbs_IsoV){
                for(j = 1; j < theDiscret; j++){
                  U1 += stepU;
                  V1 += stepV;
                  S.D0(U1,V1,P);
                  DrawTo(P,thePolyData,thePts);
                }
              }
              break;
      case GeomAbs_Sphere :
      case GeomAbs_Torus :
      case GeomAbs_OffsetSurface :
      case GeomAbs_OtherSurface :
        for(j = 1; j < theDiscret; j++){
                U1 += stepU;
                V1 += stepV;
                S.D0(U1,V1,P);
                DrawTo(P,thePolyData,thePts);
              }
              break;
      case GeomAbs_BezierSurface :
      case GeomAbs_BSplineSurface :
        for(j = 1; j <= theDiscret/2; j++){
          Standard_Real aStep = (theIsoType == GeomAbs_IsoV) ? stepU*2. : stepV*2.;
                CreateIso__(S, theIsoType, U1, V1, aStep, thePolyData, thePts);
                U1 += stepU*2.;
                V1 += stepV*2.;
              }
              break;
      case GeomAbs_SurfaceOfExtrusion :
      case GeomAbs_SurfaceOfRevolution :
        if((theIsoType == GeomAbs_IsoV && SurfType == GeomAbs_SurfaceOfRevolution) ||
                 (theIsoType == GeomAbs_IsoU && SurfType == GeomAbs_SurfaceOfExtrusion)) 
        {
                if(SurfType == GeomAbs_SurfaceOfExtrusion) 
            break;
                for(j = 1; j < theDiscret; j++){
                  U1 += stepU;
                  V1 += stepV;
                  S.D0(U1,V1,P);
                  DrawTo(P,thePolyData,thePts);
                }
              }else{
                CurvType = (S.BasisCurve())->GetType();
                switch(CurvType){
                case GeomAbs_Line :
                  break;
                case GeomAbs_Circle :
                case GeomAbs_Ellipse :
                  for (j = 1; j < theDiscret; j++) {
                    U1 += stepU;
                    V1 += stepV;
                    S.D0(U1,V1,P);
                    DrawTo(P,thePolyData,thePts);
                  }
                  break;
                case GeomAbs_Parabola :
                case GeomAbs_Hyperbola :
                case GeomAbs_BezierCurve :
                case GeomAbs_BSplineCurve :
                case GeomAbs_OtherCurve :
                  for(j = 1; j <= theDiscret/2; j++){
              Standard_Real aStep = (theIsoType == GeomAbs_IsoV) ? stepU*2. : stepV*2.;
                  CreateIso__(S, theIsoType, U1, V1, aStep, thePolyData, thePts);
                    U1 += stepU*2.;
                    V1 += stepV*2.;
                  }
                  break;
                }
              }
      }
    }
    S.D0(U2,V2,P);
    DrawTo(P,thePolyData,thePts);
  }  
}
 
 
 
 
void 
GEOM_WireframeFace:: 
CreateIso__(const BRepAdaptor_Surface& theSurface, 
            GeomAbs_IsoType theIsoType,
                                    Standard_Real& theU, 
                                    Standard_Real& theV, 
                                    Standard_Real theStep, 
            vtkPolyData* thePolyData,
            vtkPoints* thePts)
{
  gp_Pnt Pl, Pr, Pm;
  if (theIsoType == GeomAbs_IsoU) {
    theSurface.D0(theU, theV, Pl);
    theSurface.D0(theU, theV + theStep/2., Pm);
    theSurface.D0(theU, theV + theStep, Pr);
  } else {
    theSurface.D0(theU, theV, Pl);
    theSurface.D0(theU + theStep/2., theV, Pm);
    theSurface.D0(theU + theStep, theV, Pr);
  }

  static Standard_Real ISO_RATIO = 1.001;
  if (Pm.Distance(Pl) + Pm.Distance(Pr) <= ISO_RATIO*Pl.Distance(Pr)) {
    DrawTo(Pr,thePolyData,thePts);
  } else {
    if (theIsoType == GeomAbs_IsoU) {
      CreateIso__(theSurface, theIsoType, theU, theV, theStep/2, thePolyData, thePts);
      Standard_Real aLocalV = theV + theStep/2 ;
      CreateIso__(theSurface, theIsoType, theU, aLocalV , theStep/2, thePolyData, thePts);
    } else {
      CreateIso__(theSurface, theIsoType, theU, theV, theStep/2, thePolyData, thePts);
      Standard_Real aLocalU = theU + theStep/2 ;
      CreateIso__(theSurface, theIsoType, aLocalU , theV, theStep/2, thePolyData, thePts);
    }
  }
}
