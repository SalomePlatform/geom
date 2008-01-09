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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : GEOMBase_aWarningDlg.h
//  Author : Dmitry Matveitchev
//  Module : GEOM
//  $Header: /home/server/cvs/GEOM/GEOM_SRC/src/GEOMBase/GEOMBase_aWarningDlg.h

#ifndef GEOMBase_aWarningDLG_H
#define GEOMBase_aWarningDLG_H

#include <qdialog.h>
#include <qwidget.h>

class QString;
class QPushButton;

//=================================================================================
// class    : GEOMBase_aWarningDlg
// purpose  :
//=================================================================================
class GEOMBase_aWarningDlg : public QDialog
{ 
    Q_OBJECT

public:
    GEOMBase_aWarningDlg( QWidget* parent, const char* name, QString theText, int nb );
    ~GEOMBase_aWarningDlg();

private:
    QPushButton* myButtonOk;
    QPushButton* myButtonCancel;

};

#endif
