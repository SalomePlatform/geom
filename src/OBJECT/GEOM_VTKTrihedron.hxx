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

//  SALOME GEOM : 
//  File   : GEOM_VTKTrihedron.h
//  Author : Sergey LITONIN
//  Module : SALOME
//
#ifndef GEOM_VTKTrihedron_H
#define GEOM_VTKTrihedron_H

#include "GEOM_OBJECT_defs.hxx"

#include <Geom_Axis2Placement.hxx>
#include "SALOME_Actor.h"
#include "VTKViewer_Trihedron.h"

class vtkRenderer;
class vtkPolyDataMapper;

/*
  Class       : GEOM_VTKTrihedron
  Description : Class for displaying trihedron of local CS in VTK viewer.
                Placement of trihedron is chahged with SetPlacement() method
*/

class GEOM_OBJECT_EXPORT GEOM_VTKTrihedron : public SALOME_Actor
{

protected:
                                    GEOM_VTKTrihedron();
                                    GEOM_VTKTrihedron( const GEOM_VTKTrihedron& );
  virtual                           ~GEOM_VTKTrihedron();

public:
                                    vtkTypeMacro( GEOM_VTKTrihedron, SALOME_Actor );
  static GEOM_VTKTrihedron*         New();

  virtual double      GetSize() { return mySize;}

  virtual void                      SetVisibility( int theVisibility );
  virtual void                      SetVisibility( VTKViewer_Trihedron::TVisibility theVis );
  virtual void                      VisibilityOff() { SetVisibility( VTKViewer_Trihedron::eOff ); }
  virtual void                      VisibilityOn() { SetVisibility( VTKViewer_Trihedron::eOn ); }

  virtual void                      AddToRender( vtkRenderer* theRenderer );
  virtual void                      RemoveFromRender( vtkRenderer* theRenderer );

  virtual int                       GetVisibleActorCount( vtkRenderer* theRenderer );

  void                              SetPlacement( const Handle(Geom_Axis2Placement)& );

  virtual vtkMapper*                GetMapper();

  virtual void                      Render(vtkRenderer *, vtkMapper *);
  virtual bool                      IsSetCamera() const;
  virtual bool                      IsResizable() const;
  virtual void                      SetSize( double );
  virtual void                      SetCamera( vtkCamera* );

  void                              SetColor( double r, double g, double b );
  void                              GetColor( double& r, double& g, double& b );

  virtual bool                      hasHighlight() { return true; } 

  virtual void                      Highlight( bool theHighlight );
  virtual bool                      PreHighlight( vtkInteractorStyle* theInteractorStyle,
                                                  SVTK_SelectionEvent* theSelectionEvent,
                                                  bool theIsHighlight );

protected:
  virtual void                      ResetAxesColors();
  virtual void                      SetAxesColors( double theColor[3], bool theIsDiffuse = false );

protected:
  VTKViewer_Axis*                   myAxis[3];
  vtkPolyDataMapper*                myMapper;
  double              mySize;
  gp_Pnt                            myLocation;
  gp_Dir                            myDirX, myDirY, myDirZ;
  double              myColor[ 3 ];
  double              myDefaultColor[ 3 ];
  double              myPreHighlightColor[ 3 ];
  double              myHighlightColor[ 3 ];
};

#endif
