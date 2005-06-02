//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : GeometryGUI_Swig.cxx
//  Author : Nicolas REJNERI, Paul RASCLE
//  Module : GEOM
//  $Header$

#include "GeometryGUI_Swig.hxx"
#include "SALOMEGUI_Swig.hxx"


#include "GEOM_Actor.h"
#include "GEOM_Client.hxx"
#include "GEOM_AISShape.hxx"
#include "GEOM_AssemblyBuilder.h"
#include "GEOM_InteractiveObject.hxx"

#include "utilities.h"

using namespace std;

static GEOM_Client ShapeReader;


GEOM_Swig::GEOM_Swig()
{
  // MESSAGE("Constructeur");
}

GEOM_Swig::~GEOM_Swig()
{
  // MESSAGE("Destructeur");
}

void GEOM_Swig::createAndDisplayGO(const char* Entry)
{
  // MESSAGE("createAndDisplayGO");

}


int  GEOM_Swig::getIndexTopology(const char* SubIOR, const char* IOR)
{

  return -1;
}

const char* GEOM_Swig::getShapeTypeString(const char* IOR)
{

  return 0;
}


const char* GEOM_Swig::getShapeTypeIcon(const char* IOR)
{
  return "None";
}

void GEOM_Swig::setDisplayMode(const char* theEntry, int theMode)
{
}

void GEOM_Swig::setColor(const char* theEntry, int red, int green, int blue)
{
}

void GEOM_Swig::setTransparency(const char* theEntry, float transp)
{
}
