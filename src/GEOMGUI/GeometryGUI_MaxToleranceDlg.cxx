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
//  File   : GeometryGUI_MaxToleranceDlg.cxx
//  Author : Nicolas REJNERI
//  Module : GEOM
//  $Header$

using namespace std;
#include "GeometryGUI_MaxToleranceDlg.h"

#include "GeometryGUI.h"
#include "QAD_Application.h"
#include "QAD_Desktop.h"
#include "utilities.h"

// Open CASCADE Includes
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>

// QT Includes
#include <qbuttongroup.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>



//=================================================================================
// class    : GeometryGUI_MaxToleranceDlg()
// purpose  : Constructs a GeometryGUI_MaxToleranceDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GeometryGUI_MaxToleranceDlg::GeometryGUI_MaxToleranceDlg( QWidget* parent, const char* name, SALOME_Selection* Sel, bool modal, WFlags fl )
    : QDialog( parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{
    QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_DLG_TOLERANCE")));
    QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_SELECT")));

    if ( !name )
	setName( "GeometryGUI_MaxToleranceDlg" );
    resize( 303, 275 ); 
    setCaption( tr( "GEOM_TOLERANCE_TITLE"  ) );
    setSizeGripEnabled( TRUE );
    GeometryGUI_MaxToleranceDlgLayout = new QGridLayout( this ); 
    GeometryGUI_MaxToleranceDlgLayout->setSpacing( 6 );
    GeometryGUI_MaxToleranceDlgLayout->setMargin( 11 );
    
    /***************************************************************/
    GroupConstructors = new QButtonGroup( this, "GroupConstructors" );
    GroupConstructors->setTitle( tr( "GEOM_TOLERANCE"  ) );
    GroupConstructors->setExclusive( TRUE );
    GroupConstructors->setColumnLayout(0, Qt::Vertical );
    GroupConstructors->layout()->setSpacing( 0 );
    GroupConstructors->layout()->setMargin( 0 );
    GroupConstructorsLayout = new QGridLayout( GroupConstructors->layout() );
    GroupConstructorsLayout->setAlignment( Qt::AlignTop );
    GroupConstructorsLayout->setSpacing( 6 );
    GroupConstructorsLayout->setMargin( 11 );
    Constructor1 = new QRadioButton( GroupConstructors, "Constructor1" );
    Constructor1->setText( tr( ""  ) );
    Constructor1->setPixmap( image0 );
    Constructor1->setChecked( TRUE );
    Constructor1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, 
					      Constructor1->sizePolicy().hasHeightForWidth() ) );
    Constructor1->setMinimumSize( QSize( 60, 0 ) );
    GroupConstructorsLayout->addWidget( Constructor1, 0, 0 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupConstructorsLayout->addItem( spacer, 0, 1 );
    GeometryGUI_MaxToleranceDlgLayout->addWidget( GroupConstructors, 0, 0 );

    /***************************************************************/
    GroupConstructor1 = new QGroupBox( this, "GroupConstructor1" );
    GroupConstructor1->setTitle( tr( "GEOM_TOLERANCE_CONSTR"  ) );
    GroupConstructor1->setColumnLayout(0, Qt::Vertical );
    GroupConstructor1->layout()->setSpacing( 0 );
    GroupConstructor1->layout()->setMargin( 0 );
    GroupConstructor1Layout = new QGridLayout( GroupConstructor1->layout() );
    GroupConstructor1Layout->setAlignment( Qt::AlignTop );
    GroupConstructor1Layout->setSpacing( 6 );
    GroupConstructor1Layout->setMargin( 11 );

    LineEditC1A1 = new QLineEdit( GroupConstructor1, "LineEditC1A1" );
    LineEditC1A1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditC1A1->sizePolicy().hasHeightForWidth() ) );
//    GroupConstructor1Layout->addWidget( LineEditC1A1, 0, 2 );
    SelectButtonC1A1 = new QPushButton( GroupConstructor1, "SelectButtonC1A1" );
    SelectButtonC1A1->setText( tr( ""  ) );
    SelectButtonC1A1->setPixmap( image1 );
//    GroupConstructor1Layout->addWidget( SelectButtonC1A1, 0, 1 );
    TextLabelC1A1 = new QLabel( GroupConstructor1, "TextLabelC1A1" );
    TextLabelC1A1->setText( tr( "GEOM_OBJECT"  ) );
    TextLabelC1A1->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC1A1->setFrameShape( QLabel::NoFrame );
    TextLabelC1A1->setFrameShadow( QLabel::Plain );
//    GroupConstructor1Layout->addWidget( TextLabelC1A1, 0, 0 );

    QHBoxLayout* bl = new QHBoxLayout;
    bl->setMargin(0); bl->setSpacing(6);
    bl->addWidget(TextLabelC1A1); bl->addWidget(SelectButtonC1A1); bl->addWidget(LineEditC1A1);
    GroupConstructor1Layout->addMultiCellLayout(bl, 0, 0, 0, 2);

    TextLabel_Min = new QLabel( GroupConstructor1, "TextLabel_Min" );
    TextLabel_Min->setText( tr( "GEOM_MIN"  ) );
    TextLabel_Min->setMinimumSize( QSize( 50, 0 ) );
    TextLabel_Min->setFrameShape( QLabel::NoFrame );
    TextLabel_Min->setFrameShadow( QLabel::Plain );
    GroupConstructor1Layout->addWidget( TextLabel_Min, 1, 1 );

    TextLabel_Max = new QLabel( GroupConstructor1, "TextLabel_Max" );
    TextLabel_Max->setText( tr( "GEOM_MAX"  ) );
    TextLabel_Max->setMinimumSize( QSize( 50, 0 ) );
    TextLabel_Max->setFrameShape( QLabel::NoFrame );
    TextLabel_Max->setFrameShadow( QLabel::Plain );
    GroupConstructor1Layout->addWidget( TextLabel_Max, 1, 2 );

    TextLabel_Face = new QLabel( GroupConstructor1, "TextLabel_Face" );
    TextLabel_Face->setText( tr( "GEOM_TOLERANCE_FACE"  ) );
    TextLabel_Face->setMinimumSize( QSize( 50, 0 ) );
    TextLabel_Face->setFrameShape( QLabel::NoFrame );
    TextLabel_Face->setFrameShadow( QLabel::Plain );
    GroupConstructor1Layout->addWidget( TextLabel_Face, 2, 0 );
    LineEdit_MinFace = new QLineEdit( GroupConstructor1, "LineEdit_MinFace" );
    LineEdit_MinFace->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0,
						 LineEdit_MinFace->sizePolicy().hasHeightForWidth() ) );
    //LineEdit_MinFace->setEnabled( FALSE );
    LineEdit_MinFace->setReadOnly( TRUE );
    GroupConstructor1Layout->addWidget( LineEdit_MinFace, 2, 1 );
    LineEdit_MaxFace = new QLineEdit( GroupConstructor1, "LineEdit_MaxFace" );
    LineEdit_MaxFace->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0,
						 LineEdit_MaxFace->sizePolicy().hasHeightForWidth() ) );
    //LineEdit_MaxFace->setEnabled( FALSE );
    LineEdit_MaxFace->setReadOnly( TRUE );
    GroupConstructor1Layout->addWidget( LineEdit_MaxFace, 2, 2 );

    TextLabel_Edge = new QLabel( GroupConstructor1, "TextLabel_Edge" );
    TextLabel_Edge->setText( tr( "GEOM_TOLERANCE_EDGE"  ) );
    TextLabel_Edge->setMinimumSize( QSize( 50, 0 ) );
    TextLabel_Edge->setFrameShape( QLabel::NoFrame );
    TextLabel_Edge->setFrameShadow( QLabel::Plain );
    GroupConstructor1Layout->addWidget( TextLabel_Edge, 3, 0 );
    LineEdit_MinEdge = new QLineEdit( GroupConstructor1, "LineEdit_MinEdge" );
    LineEdit_MinEdge->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0,
						 LineEdit_MinEdge->sizePolicy().hasHeightForWidth() ) );
    //LineEdit_MinEdge->setEnabled( FALSE );
    LineEdit_MinEdge->setReadOnly( TRUE );
    GroupConstructor1Layout->addWidget( LineEdit_MinEdge, 3, 1 );
    LineEdit_MaxEdge = new QLineEdit( GroupConstructor1, "LineEdit_MaxEdge" );
    LineEdit_MaxEdge->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0,
						 LineEdit_MaxEdge->sizePolicy().hasHeightForWidth() ) );
    //LineEdit_MaxEdge->setEnabled( FALSE );
    LineEdit_MaxEdge->setReadOnly( TRUE );
    GroupConstructor1Layout->addWidget( LineEdit_MaxEdge, 3, 2 );

    TextLabel_Vertex = new QLabel( GroupConstructor1, "TextLabel_Vertex" );
    TextLabel_Vertex->setText( tr( "GEOM_TOLERANCE_VERTEX"  ) );
    TextLabel_Vertex->setMinimumSize( QSize( 50, 0 ) );
    TextLabel_Vertex->setFrameShape( QLabel::NoFrame );
    TextLabel_Vertex->setFrameShadow( QLabel::Plain );
    GroupConstructor1Layout->addWidget( TextLabel_Vertex, 4, 0 );
    LineEdit_MinVertex = new QLineEdit( GroupConstructor1, "LineEdit_MinVertex" );
    LineEdit_MinVertex->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0,
						 LineEdit_MinVertex->sizePolicy().hasHeightForWidth() ) );
    //LineEdit_MinVertex->setEnabled( FALSE );
    LineEdit_MinVertex->setReadOnly( TRUE );
    GroupConstructor1Layout->addWidget( LineEdit_MinVertex, 4, 1 );
    LineEdit_MaxVertex = new QLineEdit( GroupConstructor1, "LineEdit_MaxVertex" );
    LineEdit_MaxVertex->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0,
						 LineEdit_MaxVertex->sizePolicy().hasHeightForWidth() ) );
    //LineEdit_MaxVertex->setEnabled( FALSE );
    LineEdit_MaxVertex->setReadOnly( TRUE );
    GroupConstructor1Layout->addWidget( LineEdit_MaxVertex, 4, 2 );

    GeometryGUI_MaxToleranceDlgLayout->addWidget( GroupConstructor1, 1, 0 );

    /***************************************************************/
    GroupButtons = new QGroupBox( this, "GroupButtons" );
    GroupButtons->setGeometry( QRect( 10, 10, 281, 48 ) ); 
    GroupButtons->setTitle( tr( ""  ) );
    GroupButtons->setColumnLayout(0, Qt::Vertical );
    GroupButtons->layout()->setSpacing( 0 );
    GroupButtons->layout()->setMargin( 0 );
    GroupButtonsLayout = new QGridLayout( GroupButtons->layout() );
    GroupButtonsLayout->setAlignment( Qt::AlignTop );
    GroupButtonsLayout->setSpacing( 6 );
    GroupButtonsLayout->setMargin( 11 );
    buttonCancel = new QPushButton( GroupButtons, "buttonCancel" );
    buttonCancel->setText( tr( "GEOM_BUT_CLOSE"  ) );
    buttonCancel->setAutoDefault( TRUE );
    GroupButtonsLayout->addWidget( buttonCancel, 0, 1 );
    // buttonApply = new QPushButton( GroupButtons, "buttonApply" );
//     buttonApply->setText( tr( "GEOM_BUT_APPLY"  ) );
//     buttonApply->setAutoDefault( TRUE );
//     GroupButtonsLayout->addWidget( buttonApply, 0, 1 );
    QSpacerItem* spacer_8 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupButtonsLayout->addItem( spacer_8, 0, 0 );
    QSpacerItem* spacer_9 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupButtonsLayout->addItem( spacer_9, 0, 2 );
//     buttonOk = new QPushButton( GroupButtons, "buttonOk" );
//     buttonOk->setText( tr( "GEOM_BUT_OK"  ) );
//     buttonOk->setAutoDefault( TRUE );
//     buttonOk->setDefault( TRUE );
//     GroupButtonsLayout->addWidget( buttonOk, 0, 0 );
    GeometryGUI_MaxToleranceDlgLayout->addWidget( GroupButtons, 2, 0 );
    /***************************************************************/

    Init(Sel) ; /* Initialisations */
}


//=================================================================================
// function : ~GeometryGUI_MaxToleranceDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GeometryGUI_MaxToleranceDlg::~GeometryGUI_MaxToleranceDlg()
{
    // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GeometryGUI_MaxToleranceDlg::Init( SALOME_Selection* Sel )
{
  myConstructorId = 0 ;
  Constructor1->setChecked( TRUE );
  myEditCurrentArgument = LineEditC1A1 ;	
  mySelection = Sel;
  myGeomGUI = GeometryGUI::GetGeometryGUI() ;
  myGeomGUI->SetActiveDialogBox( (QDialog*)this ) ;
  
  // TODO : previous selection into argument ?

  /* Filter definitions */
  Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer", "GEOM");
  myGeom = GEOM::GEOM_Gen::_narrow(comp);

  /* signals and slots connections */
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( ClickOnCancel() ) ) ;
  connect( GroupConstructors, SIGNAL(clicked(int) ), SLOT( ConstructorsClicked(int) ) );
  connect( SelectButtonC1A1, SIGNAL (clicked() ),   this, SLOT( SetEditCurrentArgument() ) ) ;
  connect( mySelection, SIGNAL( currentSelectionChanged() ),     this, SLOT( SelectionIntoArgument() ) );

  connect( LineEditC1A1, SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;

  connect( myGeomGUI, SIGNAL ( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) ) ;
  /* to close dialog if study change */
  connect( myGeomGUI, SIGNAL ( SignalCloseAllDialogs() ), this, SLOT( ClickOnCancel() ) ) ;

  /* Move widget on the botton right corner of main widget */
  int x, y ;
  myGeomGUI->DefineDlgPosition( this, x, y ) ;
  this->move( x, y ) ;
  this->show() ; /* displays Dialog */
  return ;
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void GeometryGUI_MaxToleranceDlg::ConstructorsClicked(int constructorId)
{
  return ;
}



//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void GeometryGUI_MaxToleranceDlg::ClickOnCancel()
{
  disconnect( mySelection, 0, this, 0 );
  myGeomGUI->ResetState() ;
  reject() ;
  return ;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void GeometryGUI_MaxToleranceDlg::SelectionIntoArgument()
{
  LineEdit_MinFace->setText("") ;
  LineEdit_MinEdge->setText("") ;
  LineEdit_MinVertex->setText("") ;
  LineEdit_MaxFace->setText("") ;
  LineEdit_MaxEdge->setText("") ;
  LineEdit_MaxVertex->setText("") ;  
  myEditCurrentArgument->setText("") ;

  QString aString = ""; /* future the name of selection */

  int nbSel = myGeomGUI->GetNameOfSelectedIObjects(mySelection, aString) ;
  if ( nbSel != 1 ) {
    return ;
  }

  /*  nbSel == 1  */ 
  TopoDS_Shape S;
  if( !myGeomGUI->GetTopoFromSelection(mySelection, S) )
    return ;
  
  if( S.IsNull() ) {
    myEditCurrentArgument->setText( "" );
    return ;
  }
 
  LineEditC1A1->setText(aString) ;
  this->CalculateMaxTolerance(S) ;

  return ;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GeometryGUI_MaxToleranceDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  switch (myConstructorId)
    {
    case 0: /* default constructor */
      {	
	if(send == SelectButtonC1A1) {
	  LineEditC1A1->setFocus() ;
	  myEditCurrentArgument = LineEditC1A1;
	}
	SelectionIntoArgument() ;
	break;
      }
    }
  return ;
}



//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void GeometryGUI_MaxToleranceDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();  
  if( send == LineEditC1A1 )
    myEditCurrentArgument = LineEditC1A1 ;
  else
    return ;
  
  /* User name of object input management                          */
  /* If successfull the selection is changed and signal emitted... */
  /* so SelectionIntoArgument() is automatically called.           */
  const QString objectUserName = myEditCurrentArgument->text() ;
  QWidget* thisWidget = (QWidget*)this ;
  if( myGeomGUI->SelectionByNameInDialogs( thisWidget, objectUserName, mySelection ) ) {
    myEditCurrentArgument->setText( objectUserName ) ;
  }
  return ;
}



//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void GeometryGUI_MaxToleranceDlg::DeactivateActiveDialog()
{
  if ( GroupConstructors->isEnabled() ) {    
    disconnect( mySelection, 0, this, 0 );
    GroupConstructors->setEnabled(false) ;
    GroupConstructor1->setEnabled(false) ;
    GroupButtons->setEnabled(false) ;
  }
  return ;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GeometryGUI_MaxToleranceDlg::ActivateThisDialog()
{
  /* Emit a signal to deactivate the active dialog */
  myGeomGUI->EmitSignalDeactivateDialog() ;   
  GroupConstructors->setEnabled(true) ;
  GroupConstructor1->setEnabled(true) ;
  GroupButtons->setEnabled(true) ;
  connect( mySelection, SIGNAL( currentSelectionChanged() ),     this, SLOT( SelectionIntoArgument() ) );
  return ;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void GeometryGUI_MaxToleranceDlg::enterEvent(QEvent* e)
{
  if ( GroupConstructors->isEnabled() )
    return ;  
  ActivateThisDialog() ;
  return ;
}


//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void GeometryGUI_MaxToleranceDlg::closeEvent( QCloseEvent* e )
{
  /* same than click on cancel button */
  this->ClickOnCancel() ;
  return ;
}


//=================================================================================
// function : CalculateMaxTolerance()
// purpose  :
//=================================================================================
void GeometryGUI_MaxToleranceDlg::CalculateMaxTolerance(const TopoDS_Shape& S)
{
  LineEdit_MinFace->setText("") ;
  LineEdit_MinEdge->setText("") ;
  LineEdit_MinVertex->setText("") ;
  LineEdit_MaxFace->setText("") ;
  LineEdit_MaxEdge->setText("") ;
  LineEdit_MaxVertex->setText("") ;
  if( S.IsNull() ) 
    return ;

  Standard_Real T,TMF,TME,TMV,TmF,TmE,TmV;
  Standard_Integer nbF,nbE,nbV;
  TMF=TME=TMV=-RealLast();
  TmF=TmE=TmV=RealLast();
  nbF=nbE=nbV=0;

  bool m_isFace = false;
  bool m_isEdge = false;
  bool m_isVertex = false;
  try 
    {
    for( TopExp_Explorer ExF(S,TopAbs_FACE); ExF.More(); ExF.Next() ) 
      { 
	m_isFace = true;
	TopoDS_Face Face=TopoDS::Face(ExF.Current());
	T=BRep_Tool::Tolerance(Face);
	if(T>TMF) TMF=T;
	if(T<TmF) TmF=T;
	nbF++;
      }
    for( TopExp_Explorer ExE(S,TopAbs_EDGE); ExE.More(); ExE.Next() )
      { 
	m_isEdge = true;
	TopoDS_Edge Edge=TopoDS::Edge(ExE.Current());
	T=BRep_Tool::Tolerance(Edge);
	if(T>TME) TME=T;
	if(T<TmE) TmE=T;
	nbE++;
      }
    for( TopExp_Explorer ExV(S,TopAbs_VERTEX); ExV.More(); ExV.Next() )
      { 
	m_isVertex = true;
	TopoDS_Vertex Vertex=TopoDS::Vertex(ExV.Current());
	T=BRep_Tool::Tolerance(Vertex);
	if(T>TMV) TMV=T;
	if(T<TmV) TmV=T;
	nbV++;
      }
    if (m_isFace)
      {
	LineEdit_MinFace->setText( tr("%1").arg( TmF, 5, 'e', 8 ) ) ;
	LineEdit_MaxFace->setText( tr("%1").arg( TMF, 5, 'e', 8 ) ) ;
      }
    else
      {
	LineEdit_MinFace->setText( "" ) ;
	LineEdit_MaxFace->setText( "" ) ;
      }
    if (m_isEdge)
      {
	LineEdit_MinEdge->setText( tr("%1").arg( TmE, 5, 'e', 8 ) ) ;
	LineEdit_MaxEdge->setText( tr("%1").arg( TME, 5, 'e', 8 ) ) ;
      }
    else
      {
	LineEdit_MinEdge->setText( "" ) ;
	LineEdit_MaxEdge->setText( "" ) ;
      }
    if (m_isVertex)
      {
	LineEdit_MinVertex->setText( tr("%1").arg( TmV, 5, 'e', 8 ) ) ;
	LineEdit_MaxVertex->setText( tr("%1").arg( TMV, 5, 'e', 8 ) ) ;
      }
    else
      {
	LineEdit_MinVertex->setText( "" ) ;
	LineEdit_MaxVertex->setText( "" ) ;
      }
  }
  catch(Standard_Failure) 
    {
      MESSAGE("Catch intercepted in CalculateMaxTolerance()" << endl ) ;
    }
  return ;
}
