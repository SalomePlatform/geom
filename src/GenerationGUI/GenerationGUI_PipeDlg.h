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

// GEOM GEOMGUI : GUI for Geometry component
// File   : GenerationGUI_PipeDlg.h
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#ifndef GENERATIONGUI_PIPEDLG_H
#define GENERATIONGUI_PIPEDLG_H

#include "GEOMBase_Skeleton.h"
#include "GEOM_GenericObjPtr.h"

typedef std::map<QString, QList<GEOM::GeomObjPtr> > ObjectMap;


class DlgRef_3Sel1Check;
class DlgRef_3Sel2Check3Spin;

//=================================================================================
// class    : GenerationGUI_PipeDlg
// purpose  :
//=================================================================================
class GenerationGUI_PipeDlg : public GEOMBase_Skeleton
{ 
  Q_OBJECT

public:
  GenerationGUI_PipeDlg( GeometryGUI*, QWidget* = 0, bool = false, Qt::WindowFlags = 0 );
  ~GenerationGUI_PipeDlg();
  
protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid( QString& );
  virtual bool                       execute( ObjectList& );    
  virtual void                       addSubshapesToStudy();
  virtual bool                       extractPrefix() const;
  virtual void                       restoreSubShapes
                                 (SALOMEDS::Study_ptr, SALOMEDS::SObject_ptr);
  virtual QList<GEOM::GeomObjPtr>    getSourceObjects();

private:
  void                               Init();
  void                               enterEvent( QEvent* );
  void                               updateGenGroup();
  void                               resetGenGroup
                                        (QCheckBox *theGenGroup,
                                         const bool isChecked,
                                         const bool isClearPrefix);
  void                               addGroups
                                      (GEOM::ListOfGO_var &theResult);
  
private:
  QList<GEOM::GeomObjPtr>            myBaseObjects;  /* Base shapes */
  GEOM::GeomObjPtr                   myPath; /* Shape, defining the path */
  GEOM::GeomObjPtr                   myVec;  /* Vector, defining the constant binormal direction */
  QList<GEOM::GeomObjPtr>            myLocations; /* Locations on the path corresponding specified list of the Base shapes */
  
  DlgRef_3Sel1Check*                 GroupPoints;
  DlgRef_3Sel2Check3Spin*            GroupMakePoints;
  QCheckBox                         *myGenGroupCheckGP;
  QLabel                            *myPrefixLblGP;
  QLineEdit                         *myPrefixEditGP;
  QCheckBox                         *myStepByStepCheckGMP;
  QCheckBox                         *myGenGroupCheckGMP;
  QLabel                            *myPrefixLblGMP;
  QLineEdit                         *myPrefixEditGMP;
  ObjectMap                          myGroupObjectsMap;

private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();
  void                               ActivateThisDialog();
  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();
  void                               ConstructorsClicked( int );
  void                               SelectionTypeButtonClicked();
  void                               GenGroupClicked(bool);
  void                               StepByStepClicked(bool);
};

#endif // GENERATIONGUI_PIPEDLG_H
