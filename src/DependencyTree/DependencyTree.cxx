// Copyright (C) 2014  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include <GraphicsView_Object.h>

#include "DependencyTree.h"
#include "DependencyTree_ViewModel.h"

#include <QGraphicsItem>
#include <QPolygonF>
#include <QPointF>
#include <QMenu>
#include <QPen>
#include <QFont>
#include <QString>
#include <QGraphicsPolygonItem>
#include <QGraphicsSimpleTextItem>
#include <QWidgetAction>
#include <QSpinBox>
#include <QLabel>
#include <QCheckBox>

#include <QtxActionToolMgr.h>

#include "DependencyTree_View.h"
#include <GraphicsView_Viewer.h>
#include <GraphicsView_ViewFrame.h>
#include <GraphicsView_Scene.h>

#include <SUIT_Session.h>
#include <SUIT_ViewManager.h>
#include <SalomeApp_Application.h>

#include <QList>
#include <QGraphicsView>


#include <iostream>
DependencyTree::DependencyTree()
{
	  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
	  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
	    if ( !app ) return;

	    SUIT_ViewManager *svm = app->getViewManager(GraphicsView_Viewer::Type(), false );

	    if(!svm) {
	      myView = new DependencyTree_View();
	      DependencyTree_ViewModel* ViewModel = new DependencyTree_ViewModel(GraphicsView_Viewer::Type(), myView);
	      SUIT_ViewManager *svm = app->createViewManager( ViewModel );
	      SUIT_ViewWindow* svw = svm->getActiveView();
	      GraphicsView_ViewFrame* aViewFrame = 0;
	      if (!svw) svw = svm->createViewWindow();
	      if (svw) aViewFrame = dynamic_cast<GraphicsView_ViewFrame*>(svw);

	      myView->init( aViewFrame );
	      svm->setTitle("DEPENDENCY_TREE");
	    }
	    else {
          if( DependencyTree_ViewModel* viewModel = dynamic_cast<DependencyTree_ViewModel*>( svm->getViewModel() ) )
	        if( DependencyTree_View* view = dynamic_cast<DependencyTree_View*>( viewModel->getActiveViewPort() ) ) {
	          svm->getActiveView()->setFocus();
	          view->updateModel();
	        }
	    }


}

DependencyTree::~DependencyTree()
{
}
