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
//  File   : GEOMBase_TransparencyDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM

using namespace std;
#include "GEOMToolsGUI_TransparencyDlg.h"

#include "QAD_RightFrame.h"
#include "SALOME_ListIteratorOfListIO.hxx"
#include <AIS_InteractiveContext.hxx>

#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qapplication.h>
#include <qgroupbox.h>

#include "VTKViewer_ViewFrame.h"
#include "VTKViewer_RenderWindowInteractor.h"

//=================================================================================
// class    : GEOMBase_TransparencyDlg()
// purpose  : Constructs a GEOMBase_SUBSHAPE which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//
//          : WARNING : this dialog is modal !
//
//=================================================================================
GEOMBase_TransparencyDlg::GEOMBase_TransparencyDlg(QWidget* parent, const char* name, SALOME_Selection* Sel, const Handle(AIS_InteractiveContext)& ic, bool modal, WFlags fl)
  :QDialog(parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  if(!name)
    setName("GEOMBase_TransparencyDlg");
  resize(152, 107); 
  setCaption(tr("GEOM_TRANSPARENCY_TITLE"));
  setSizeGripEnabled(TRUE);
  GEOMBase_TransparencyDlgLayout = new QGridLayout(this); 
  GEOMBase_TransparencyDlgLayout->setSpacing(6);
  GEOMBase_TransparencyDlgLayout->setMargin(11);

  /*************************************************************************/
  QGroupBox* GroupButtons = new QGroupBox( this, "GroupButtons" );
  GroupButtons->setColumnLayout(0, Qt::Vertical );
  GroupButtons->layout()->setSpacing( 0 );
  GroupButtons->layout()->setMargin( 0 );
  QGridLayout* GroupButtonsLayout = new QGridLayout( GroupButtons->layout() );
  GroupButtonsLayout->setAlignment( Qt::AlignTop );
  GroupButtonsLayout->setSpacing( 6 );
  GroupButtonsLayout->setMargin( 11 );
  
  buttonOk = new QPushButton( GroupButtons, "buttonOk" );
  buttonOk->setText( tr( "GEOM_BUT_OK" ) );
  buttonOk->setAutoDefault( TRUE );
  buttonOk->setDefault( TRUE );
  GroupButtonsLayout->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 0 );
  GroupButtonsLayout->addWidget( buttonOk, 0, 1 );
  GroupButtonsLayout->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 2 );

  /*************************************************************************/
  QGroupBox* GroupC1 = new QGroupBox( this, "GroupC1" );
  GroupC1->setColumnLayout(0, Qt::Vertical );
  GroupC1->layout()->setSpacing( 0 );
  GroupC1->layout()->setMargin( 0 );
  QGridLayout* GroupC1Layout = new QGridLayout( GroupC1->layout() );
  GroupC1Layout->setAlignment( Qt::AlignTop );
  GroupC1Layout->setSpacing( 6 );
  GroupC1Layout->setMargin( 11 );
  
  TextLabelOpaque = new QLabel( GroupC1, "TextLabelOpaque" );
  TextLabelOpaque->setText( tr( "GEOM_TRANSPARENCY_OPAQUE"  ) );
  TextLabelOpaque->setAlignment( int( QLabel::AlignLeft ) );
  GroupC1Layout->addWidget( TextLabelOpaque, 0, 0 );
  GroupC1Layout->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ), 0, 1 );
  
  TextLabelTransparent = new QLabel( GroupC1, "TextLabelTransparent" );
  TextLabelTransparent->setText( tr( "GEOM_TRANSPARENCY_TRANSPARENT"  ) );
  TextLabelTransparent->setAlignment( int( QLabel::AlignRight ) );
  GroupC1Layout->addWidget( TextLabelTransparent, 0, 2 );
  
  Slider1 = new QSlider( 0, 10, 1, 5, Horizontal, GroupC1, "Slider1" );
  Slider1->setMinimumSize( 300, 0 );
  Slider1->setTickmarks( QSlider::Left );
  GroupC1Layout->addMultiCellWidget( Slider1, 1, 1, 0, 2 );
  /*************************************************************************/
  
  GEOMBase_TransparencyDlgLayout->addWidget(GroupC1, 0,  0);
  GEOMBase_TransparencyDlgLayout->addWidget(GroupButtons, 1, 0);
  
  /* Initialisations */
  this->myGeomGUI = GEOMContext::GetGeomGUI();
  this->myGeomBase = new GEOMBase();
  this->myIc = ic;
  this->mySel = Sel;
  
  /* First call valueChanged() method for initialisation               */
  /* The default value of transparency will change with the selection  */
  this->myFirstInit = true;
//  Slider1->setMaxValue( 10 );
//  Slider1->setValue( 5 ) ;
  this->ValueHasChanged(Slider1->value());
  
  // signals and slots connections : after ValueHasChanged()
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(Slider1, SIGNAL(valueChanged(int)), this, SLOT(ValueHasChanged(int)));
  
  /* Move widget on the botton right corner of main widget */
  int x, y ;
  myGeomBase->DefineDlgPosition(this, x, y);
  this->move(x, y) ;
  this->show() ; /* Displays this Dialog */
}


//=================================================================================
// function : ~GEOMBase_TransparencyDlg()
// purpose  :
//=================================================================================
GEOMBase_TransparencyDlg::~GEOMBase_TransparencyDlg()
{
    // no need to delete child widgets, Qt does it all for us
}


//=======================================================================
// function : ClickOnOk()
// purpose  :
//=======================================================================
void GEOMBase_TransparencyDlg::ClickOnOk()
{
  accept();
  return;
}


//=======================================================================
// function : ClickOnClose()
// purpose  :
//=======================================================================
void GEOMBase_TransparencyDlg::ClickOnClose()
{
  accept();
  return;
}


//=================================================================================
// function : ValueHasChanged()
// purpose  : Called when value of slider change
//          : or the first time as initilisation
//=================================================================================
void GEOMBase_TransparencyDlg::ValueHasChanged(int newValue)
{
  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_VTK) {
    // VTK
    VTKViewer_RenderWindowInteractor* myRenderInter= ((VTKViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getRWInteractor();
    SALOME_ListIteratorOfListIO It(this->mySel->StoredIObjects());

    Handle(SALOME_InteractiveObject) FirstIOS =  mySel->firstIObject();
    if(!FirstIOS.IsNull()) {
      /* The first time as initialisation */
      if(this->myFirstInit) {	
	this->myFirstInit = false;
	float transp = (myRenderInter->GetTransparency(FirstIOS))*10.0;
        this->Slider1->setValue(int(transp));
      }
    }
    
    QApplication::setOverrideCursor(Qt::waitCursor);
    for(;It.More(); It.Next()) {
      Handle(SALOME_InteractiveObject) IOS = It.Value();
      myRenderInter->SetTransparency(IOS, newValue/10.0);
    }
    QApplication::restoreOverrideCursor();
  }

  else if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
    // OCC  
    SALOME_ListIteratorOfListIO It(this->mySel->StoredIObjects());
    Handle(SALOME_InteractiveObject) FirstIOS = mySel->firstIObject();
    if(!FirstIOS.IsNull()) {
      
      /* The first time as initialisation */
      if(this->myFirstInit) {
	this->myFirstInit = false;
	Standard_Boolean found;
	Handle(GEOM_AISShape) Shape = myGeomBase->ConvertIOinGEOMAISShape(FirstIOS, found);
	if(!found)
	  return;
	float transp = (int(Shape->Transparency() * 10.0 + 0.001));
        this->Slider1->setValue(int(transp));
	return;
      }
    }

    QApplication::setOverrideCursor(Qt::waitCursor);
    for(;It.More(); It.Next()) {
      Handle(SALOME_InteractiveObject) IObject = It.Value();
      Standard_Boolean found;
      Handle(GEOM_AISShape) Shape = myGeomBase->ConvertIOinGEOMAISShape(IObject, found);      
      if(!found) {
	QApplication::restoreOverrideCursor();
	return;
      }
      this->myIc->SetTransparency(Shape, newValue / 10.0, false);
      myIc->Redisplay(Shape, Standard_False, Standard_True);
    }
    myIc->UpdateCurrentViewer();
  }
  QApplication::restoreOverrideCursor();
  return;
}
