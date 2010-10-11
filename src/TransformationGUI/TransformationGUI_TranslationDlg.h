//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

// GEOM GEOMGUI : GUI for Geometry component
// File   : TransformationGUI_TranslationDlg.h
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#ifndef TRANSFORMATIONGUI_TRANSLATIONDLG_H
#define TRANSFORMATIONGUI_TRANSLATIONDLG_H

#include <GEOMBase_Skeleton.h>

class DlgRef_3Sel3Spin2Check;

//=================================================================================
// class    : TransformationGUI_TranslationDlg
// purpose  :
//=================================================================================
class TransformationGUI_TranslationDlg : public GEOMBase_Skeleton
{ 
  Q_OBJECT

public:
  TransformationGUI_TranslationDlg( GeometryGUI*, QWidget* = 0,
                                    bool = false, Qt::WindowFlags = 0 );
  ~TransformationGUI_TranslationDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid( QString& );
  virtual bool                       execute( ObjectList& );
  virtual void                       addSubshapesToStudy();
  virtual void                       restoreSubShapes( SALOMEDS::Study_ptr, SALOMEDS::SObject_ptr );

private:
  void                               Init();
  void                               enterEvent( QEvent* );
  void                               createPathPreview( GEOM::GEOM_Object_var );

private:
  GEOM::ListOfGO                     myObjects;
  GEOM::GEOM_Object_var              myCurrObject;
  GEOM::GEOM_Object_var              myVector, myPoint1, myPoint2;

  // to initialize the first selection field with a selected object on the dialog creation
  bool                               myInitial;
  
  DlgRef_3Sel3Spin2Check*            GroupPoints;
    
private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();
  void                               ActivateThisDialog();
  void                               LineEditReturnPressed();
  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();
  void                               ConstructorsClicked( int );
  void                               ValueChangedInSpinBox();
  void                               CreateCopyModeChanged( bool );
  void                               SetDoubleSpinBoxStep( double );
  void                               ActivateDistanceChanged( bool );
};

#endif // TRANSFORMATIONGUI_TRANSLATIONDLG_H
