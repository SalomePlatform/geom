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
//  File   : GeometryGUI.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#ifndef GEOMETRYGUI_H
#define GEOMETRYGUI_H

#include "GEOMContext.h"

//=================================================================================
// class    : GeometryGUI
// purpose  :
//=================================================================================
class GeometryGUI : public QObject
{
  Q_OBJECT /* for QT compatibility */

public :
    GeometryGUI();
    ~GeometryGUI();

    static GEOMContext* GetOrCreateGeometryGUI(QAD_Desktop* desktop);

    /* Managed by IAPP */
    Standard_EXPORT static bool OnGUIEvent(int theCommandID, QAD_Desktop* parent);
    Standard_EXPORT static bool OnMousePress(QMouseEvent* pe, QAD_Desktop* parent, QAD_StudyFrame* studyFrame);
    Standard_EXPORT static bool OnMouseMove(QMouseEvent* pe, QAD_Desktop* parent, QAD_StudyFrame* studyFrame);
    Standard_EXPORT static bool OnKeyPress(QKeyEvent* pe, QAD_Desktop* parent, QAD_StudyFrame* studyFrame);
    Standard_EXPORT static bool SetSettings(QAD_Desktop* parent);
    Standard_EXPORT static void activeStudyChanged(QAD_Desktop* parent);
    Standard_EXPORT static void BuildPresentation(const Handle(SALOME_InteractiveObject)&);
    Standard_EXPORT static void DefinePopup(QString & theContext, QString & theParent, QString & theObject);
    Standard_EXPORT static bool CustomPopup(QAD_Desktop* parent, QPopupMenu* popup, const QString& theContext,
					    const QString& theParent, const QString& theObject);

};

#endif

