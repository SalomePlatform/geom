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
//  File   : MeasureGUI_CheckCompoundOfBlocksDlg.h
//  Author : VKN
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_CHECKCOMPOUNDOFBLOCKSDLG_H
#define DIALOGBOX_CHECKCOMPOUNDOFBLOCKSDLG_H

#include "MeasureGUI_Skeleton.h"

class MeasureGUI_1Sel1TextView_QTD;
class QListBox;
class QLabel;

//=================================================================================
// class    : MeasureGUI_CheckCompoundOfBlocksDlg
// purpose  :
//=================================================================================
class MeasureGUI_CheckCompoundOfBlocksDlg : public MeasureGUI_Skeleton
{
    Q_OBJECT

public:
                                        MeasureGUI_CheckCompoundOfBlocksDlg( QWidget* theParent,
                                                                             SALOME_Selection* theSel );
                                        ~MeasureGUI_CheckCompoundOfBlocksDlg();
protected:

    // redefined from GEOMBase_Helper and MeasureGUI_Skeleton
    virtual GEOM::GEOM_IOperations_ptr  createOperation();
    virtual void                        processObject();

private slots:

    void onErrorsListSelectionChanged();
    void onSubShapesListSelectionChanged();

private:

    void                                Init( SALOME_Selection* theSel );
    bool                                getBCErrors( bool& theIsCompoundOfBlocks,
						     GEOM::GEOM_IBlocksOperations::BCErrors& theErrors);
    void                                activateSelection();

private:

    MeasureGUI_1Sel1TextView_QTD*        myGrp;
    QLabel*                              myErrorsLbl;
    QListBox*                            myErrorsLBox;
    QLabel*                              mySubShapesLbl;
    QListBox*                            mySubShapesLBox;

};

#endif // DIALOGBOX_CHECKCOMPOUNDOFBLOCKSDLG_H
