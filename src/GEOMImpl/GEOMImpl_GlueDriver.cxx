
using namespace std;
#include "GEOMImpl_GlueDriver.hxx"
#include "GEOMImpl_IGlue.hxx"
#include "GEOMImpl_Types.hxx"
#include "GEOM_Function.hxx"

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepLib.hxx>
#include <BRepBndLib.hxx>
#include <BRepTools_Quilt.hxx>
#include <BRepClass3d_SolidClassifier.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <gp_Pnt.hxx>
#include <Bnd_Box.hxx>
#include <Precision.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_ConstructionError.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_GlueDriver::GetID()
{
  static Standard_GUID aGlueDriver("FF1BBB63-5D14-4df2-980B-3A668264EA16");
  return aGlueDriver;
}


//=======================================================================
//function : GEOMImpl_GlueDriver
//purpose  :
//=======================================================================
GEOMImpl_GlueDriver::GEOMImpl_GlueDriver()
{
}

//=======================================================================
//function : FindSameFace
//purpose  : for GLUE_FACES
//=======================================================================
static TopoDS_Face FindSameFace (const TopoDS_Shape& aShape,
                                 const TopoDS_Face&  F,
                                 const double        tol3d)
{
  TopoDS_Face aFace;
  bool isSame = false;
  for (TopExp_Explorer exf (aShape, TopAbs_FACE); exf.More(); exf.Next()) {
    // test a face
    int nbFound = 0;
    aFace = TopoDS::Face(exf.Current());
    TopTools_ListOfShape liste1;
    TopTools_ListOfShape liste2;
    for (TopExp_Explorer exp (aFace, TopAbs_VERTEX); exp.More(); exp.Next()) {
      const TopoDS_Vertex& V = TopoDS::Vertex(exp.Current());
      liste1.Append(V);
    }
    for (TopExp_Explorer exp (F, TopAbs_VERTEX); exp.More(); exp.Next()) {
      const TopoDS_Vertex& V = TopoDS::Vertex(exp.Current());
      liste2.Append(V);
    }
    isSame = false;
    if (liste1.Extent() == liste2.Extent())
    {
      TopTools_ListIteratorOfListOfShape it1 (liste1);
      isSame = true;
      for (; it1.More(); it1.Next())
      {
        bool foundSamePoint = false;
        gp_Pnt P1 = BRep_Tool::Pnt(TopoDS::Vertex(it1.Value()));
        TopTools_ListIteratorOfListOfShape it2 (liste2);
        for (; it2.More(); it2.Next()) {
          gp_Pnt P2 = BRep_Tool::Pnt(TopoDS::Vertex(it2.Value()));
          double d = P1.Distance(P2);
          if (d < tol3d) {
            nbFound++;
            // found Same Point : P1
            foundSamePoint = true;
            break;
          }
        }
        isSame = isSame && foundSamePoint;
        if (! isSame) break; // a vertex does not correspond : not same face
      }
    }
    if (isSame) {
      // Found Same Face
      break; // a face corresponding to F is found
    }
  }
  if (!isSame) aFace.Nullify(); // return null face
  return aFace;
}

//=======================================================================
//function : FindSameEdge
//purpose  : for GLUE_FACES
//=======================================================================
static TopoDS_Edge FindSameEdge (const TopoDS_Face& nf,
                                 TopoDS_Edge&       Eold,
                                 const double       tol3d)
{
  TopoDS_Face newFace = TopoDS::Face(nf.Oriented(TopAbs_REVERSED));
  TopoDS_Vertex VFirst, VLast;
  TopExp::Vertices(Eold, VFirst, VLast);
  gp_Pnt Pf = BRep_Tool::Pnt(VFirst);
  gp_Pnt Pl = BRep_Tool::Pnt(VLast);
  TopoDS_Edge Enew;
  for (TopExp_Explorer ee (newFace, TopAbs_EDGE); ee.More(); ee.Next()) {
    const TopoDS_Edge& E = TopoDS::Edge(ee.Current());
    TopoDS_Vertex VFn, VLn;
    TopExp::Vertices(E, VFn, VLn);
    gp_Pnt Pfn = BRep_Tool::Pnt(VFn);
    gp_Pnt Pln = BRep_Tool::Pnt(VLn);
    double dff = Pf.Distance(Pfn);
    double dfl = Pf.Distance(Pln);
    double dlf = Pl.Distance(Pfn);
    double dll = Pl.Distance(Pln);
    if ((dff < tol3d) && (dll <tol3d)) {
      // edge forward : Pf - Pl
      Enew = TopoDS::Edge(E.Oriented(TopAbs_FORWARD));
      Eold = TopoDS::Edge(Eold.Oriented(TopAbs_FORWARD));
      break;
    }
    if ((dfl < tol3d) && (dlf <tol3d)) {
      // edge reversed : Pf - Pl
      Enew = TopoDS::Edge(E.Oriented(TopAbs_REVERSED));
      Eold = TopoDS::Edge(Eold.Oriented(TopAbs_FORWARD));
      break;
    }
  }
  return Enew;
}

//=======================================================================
//function : GlueFaces
//purpose  :
//=======================================================================
TopoDS_Shape GEOMImpl_GlueDriver::GlueFaces (const TopoDS_Shape& theShape,
                                             const Standard_Real theTolerance)
{
  // prendre un premier shell dans la liste des shells
  // initialiser un compshell avec ce shell
  // tant qu'il reste des shells dans la liste
  //   chercher un shell qui a des faces en  commun avec le compshell
  //   creer un BRepTools_Quilt
  //   recenser les faces communes issues du compshell, les ajouter au quilt
  //   recenser les faces restantes du shell a inclure, les ajouter au quilt
  //   recenser les edges en double, a remplacer
  //   pour chaque paire d'edge
  //     tester l'orientation relative des aretes
  //     bind dans le quilt de Eold.Forward et Enew.Forward (ou reverse)
  //   recuperer le nouveau shell
  // l'incorporer dans le compshell
  // appliquer BRepTools_SameParameter au compshell
  // (rendre parametres 2D des edges identiques aux parametres 3D)

  TopoDS_Shape aShape;

  TopoDS_Compound C;
  BRep_Builder bu;
  bu.MakeCompound(C); // empty compound;
  TopTools_ListOfShape shellList;
  for (TopExp_Explorer exp (theShape, TopAbs_SHELL); exp.More(); exp.Next()) {
    const TopoDS_Shell& S = TopoDS::Shell(exp.Current());
    shellList.Append(S);
  }
  TopTools_ListIteratorOfListOfShape its (shellList);
  if (!its.More()) {
    Standard_ConstructionError::Raise("Glue aborted : no shell in shape");
  }

  TopoDS_Shell S = TopoDS::Shell(its.Value());
  bu.Add(C, S); // add first shell to compound
  shellList.Remove(its);
  bool shellAdded = true;
  bool bigTolerance = false;

  while ((shellList.Extent() > 0) && shellAdded) {
    shellAdded = false;
    its.Initialize(shellList);
    for (; its.More(); its.Next()) {
      S = TopoDS::Shell(its.Value());

      // compare tolerance with shape's size
      Bnd_Box aBox;
      BRepBndLib::Add(S, aBox);
      Standard_Real Xmin, Ymin, Zmin, Xmax, Ymax, Zmax;
      aBox.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
      Standard_Real aTolerance = theTolerance;
      if (aBox.IsXThin(100. * aTolerance))
        aTolerance = 0.01 * (Xmax - Xmin);
      if (aBox.IsYThin(100. * aTolerance))
        aTolerance = 0.01 * (Ymax - Ymin);
      if (aBox.IsZThin(100. * aTolerance))
        aTolerance = 0.01 * (Zmax - Zmin);
      if (theTolerance > aTolerance)
        bigTolerance = true;

      bool isConnected = false;
      TopTools_ListOfShape newEdges; // common edges from new compound
      TopTools_ListOfShape oldEdges; // common edges from face to add
      TopoDS_Compound CFN;
      TopoDS_Compound CFO;
      bu.MakeCompound(CFN);       // empty compound for new faces
      bu.MakeCompound(CFO);       // empty compound for old faces

      for (TopExp_Explorer exp (S, TopAbs_FACE); exp.More(); exp.Next()) {
        // try to find corresponding face in new compound
        TopoDS_Face F = TopoDS::Face(exp.Current());
        TopoDS_Face newFace = FindSameFace(C,F,aTolerance);
        if (! newFace.IsNull())
        {
          // face found
          isConnected = true;
          bu.Add(CFN, newFace); // common faces from new compound
          for (TopExp_Explorer ee (F, TopAbs_EDGE); ee.More(); ee.Next()) {
            // find edge pair
            TopoDS_Edge Eold = TopoDS::Edge(ee.Current());
            TopoDS_Edge Enew = FindSameEdge(newFace, Eold, aTolerance);
            if (Enew.IsNull()) {
              Standard_ConstructionError::Raise("Glue aborted : no same edge in same face");
            }
            oldEdges.Append(Eold);
            newEdges.Append(Enew);
          }
        } else {
          bu.Add(CFO, F); // not common faces from shell to add
        }
      }
      if (isConnected) {
        // some faces found
        shellAdded = true;
        BRepTools_Quilt glue;
        glue.Add(CFN);
        TopTools_ListIteratorOfListOfShape ito (oldEdges);
        TopTools_ListIteratorOfListOfShape itn (newEdges);
        for (; ito.More(); ito.Next()) {
          // bind
          glue.Bind(TopoDS::Edge(ito.Value()), TopoDS::Edge(itn.Value()));
          itn.Next();
        }
        glue.Add(CFO);
        TopoDS_Compound newc = TopoDS::Compound(glue.Shells());
        for (TopExp_Explorer exs (newc, TopAbs_SHELL); exs.More(); exs.Next()) {
          TopoDS_Shell NS = TopoDS::Shell(exs.Current());
          bu.Add(C, NS);
        }
        shellList.Remove(its);
        // remove shell from list
        break;
      }
    }
  }

  if (shellList.Extent() > 0) {
    TCollection_AsciiString aMsg
      ("Some shapes can not be glued with others, because they are too far from them.");
    if (bigTolerance) {
      aMsg += "\n\nWarning: The tolerance is too big for some sub-shapes, 1% of sub-shape size is given instead.";
    }
    Standard_ConstructionError::Raise(aMsg.ToCString());
  }

  TopExp_Explorer exp (C, TopAbs_SHELL);
  Standard_Integer ish = 0;
  TopoDS_Compound  Res;
  TopoDS_Solid     Sol;
  BRep_Builder     B;
  B.MakeCompound(Res);

  for (; exp.More(); exp.Next()) {
    TopoDS_Shape Sh = exp.Current();
    B.MakeSolid(Sol);
    B.Add(Sol,Sh);
    BRepClass3d_SolidClassifier SC(Sol);
    SC.PerformInfinitePoint(1.E-6); // cf. BRepFill_Confusion() - BRepFill_Evolved.cxx
    if (SC.State() == TopAbs_IN) {
      B.MakeSolid(Sol);
      B.Add(Sol,Sh.Reversed());
    }
    B.Add(Res,Sol);
    ish++;
  }
  if (ish == 1) {
    aShape = Sol;
  } else {
    aShape = Res;
  }

  BRepLib::SameParameter(aShape, 1.E-5, Standard_True);
  return aShape;
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_GlueDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IGlue aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  if (aType == GLUE_FACES) {
    Handle(GEOM_Function) aRefBase = aCI.GetBase();
    TopoDS_Shape aShapeBase = aRefBase->GetValue();
    if (aShapeBase.IsNull()) {
      Standard_NullObject::Raise("Shape for gluing is null");
    }

    Standard_Real tol3d = aCI.GetTolerance();
    aShape = GlueFaces(aShapeBase, tol3d);

  } else {
  }

  if (aShape.IsNull()) return 0;

  aFunction->SetValue(aShape);

  log.SetTouched(Label());

  return 1;
}


//=======================================================================
//function :  GEOMImpl_GlueDriver_Type_
//purpose  :
//=======================================================================
Standard_EXPORT Handle_Standard_Type& GEOMImpl_GlueDriver_Type_()
{

  static Handle_Standard_Type aType1 = STANDARD_TYPE(TFunction_Driver);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TFunction_Driver);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);


  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOMImpl_GlueDriver",
			                                 sizeof(GEOMImpl_GlueDriver),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}

//=======================================================================
//function : DownCast
//purpose  :
//=======================================================================
const Handle(GEOMImpl_GlueDriver) Handle(GEOMImpl_GlueDriver)::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle(GEOMImpl_GlueDriver) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOMImpl_GlueDriver))) {
       _anOtherObject = Handle(GEOMImpl_GlueDriver)((Handle(GEOMImpl_GlueDriver)&)AnObject);
     }
  }

  return _anOtherObject ;
}
