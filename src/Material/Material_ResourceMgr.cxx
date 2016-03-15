// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File   : Material_ResourceMgr.cxx
// Author : Margarita KARPUNINA, Open CASCADE S.A.S. (margarita.karpunina@opencascade.com)

#include "Material_ResourceMgr.h"

#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QThread>

/*!
  \class Material_ResourceMgr::Updater
  \brief Updates the contents of the resource manager as soon as
  user materials database file is changed
  \internal
*/
class Material_ResourceMgr::Updater : public QThread
{
public:
  Material_ResourceMgr* myResourceMgr;
  Updater( Material_ResourceMgr* resMgr ) : myResourceMgr( resMgr )
  {
    start();
  }
  void run()
  {
    QMutexLocker lock( &myResourceMgr->myMutex );
    myResourceMgr->clear();
    myResourceMgr->load();
  }
};

/*!
  \class Material_ResourceMgr
  \brief Material properties resources manager.

  This class is used to manage the material properties throughout the application
  in the similar way as QtxResourceMgr does it with application preferences.

  Standard material types are stored in the global application settings files
  (named as SalomeMaterial.xml). User-defined materials are stored in user's home
  directory - in the file .SalomeMaterialrc.

  The Material_ResourceMgr class is used by material properties dialog box
  (GEOMToolsGUI_MaterialPropertiesDlg class).
*/

/*!
  \brief Constructor
*/
Material_ResourceMgr::Material_ResourceMgr()
  : QtxResourceMgr( "SalomeMaterial", "%1Config" ),
    myWatcher( 0 )
{
  if ( dirList().isEmpty() && ::getenv( "GEOM_ROOT_DIR" ) )
    setDirList( QStringList() << Qtx::addSlash( ::getenv( "GEOM_ROOT_DIR" ) ) + "share/salome/resources/geom" );
  setCurrentFormat( "xml" );
}

/*!
  \brief Destructor
*/
Material_ResourceMgr::~Material_ResourceMgr()
{
  watchUserFile( false );
}

/*!
  \brief Get shared instance of resources manager
  
  This instance of resource manager is global for the application;
  it watches for changes in the user materials database file to
  maintain the fresh version of the materials data.
*/
Material_ResourceMgr* Material_ResourceMgr::resourceMgr()
{
  static Material_ResourceMgr* resMgr = 0;
  if ( !resMgr ) {
    resMgr = new Material_ResourceMgr();
    resMgr->watchUserFile( true );
  }
  return resMgr;
}

/*!
  \brief Get list of avaiable materials
  \param theType material type
  \param theSort if \c true (default), returns a list of materials sorted by name
  \return list of avaiable materials names
*/
QStringList Material_ResourceMgr::materials( MaterialType theType, bool theSort )
{
  QMutexLocker lock( &myMutex );

  // store original working mode
  WorkingMode m = workingMode();

  QStringList slglobal, sluser;

  // retrieve all materials : global + user
  setWorkingMode( AllowUserValues );
  sluser = sections();

  // retrieve only global materials
  setWorkingMode( IgnoreUserValues );
  slglobal = sections();

  // remove global materials from user list to obtain only user materials
  QMutableListIterator<QString> it( sluser );
  while ( it.hasNext() ) {
    QString s = it.next();
    if ( slglobal.contains( s ) ) it.remove();
  }

  // remove 'common' material 
  slglobal.removeAll("[common]");

  // restore original working mode
  setWorkingMode( m );

  // sort if necessary (separately global and user materials)
  if ( theSort ) {
    qSort( slglobal );
    qSort( sluser );
  }
  // special processing for default material (to make it first in the list)
  if ( slglobal.contains( "[ Default ]" ) ) {
    slglobal.removeAll( "[ Default ]" );
    slglobal.prepend( "[ Default ]" );
  }

  // combine the materials to obtain result list
  QStringList result;
  
  switch ( theType ) {
  case Global:
    result = slglobal;
    break;
  case User:
    result = sluser;
    break;
  case All:
    result = slglobal + sluser;
    break;
  default:
    break;
  }

  return result;
}

/*!
  \brief Start/stop this resource manager watching the user materials database file.
  \internal
*/
void Material_ResourceMgr::watchUserFile( bool on )
{
  if ( on ) {
    if ( !myWatcher ) {
      myWatcher = new QFileSystemWatcher( this );
      QFileInfo ufile = userFileName( appName() );
      if ( ufile.exists() ) {
        myWatcher->addPath( ufile.filePath() );
      }
      connect( myWatcher, SIGNAL( fileChanged( QString ) ), this, SLOT( update() ) );
    }
  }
  else {
    if ( myWatcher ) {
      delete myWatcher;
      myWatcher = 0;
    }
  }
}

/*!
  \brief This function is called after user configuration file is saved.
  \internal
*/
void Material_ResourceMgr::saved()
{
  if ( resourceMgr() != this ) {
    resourceMgr()->saved();
  }
  else if ( myWatcher ) {
    QStringList files = myWatcher->files();
    QFileInfo ufile = userFileName( appName() );
    if ( ufile.exists() && !files.contains( ufile.filePath() ) ) {
      myWatcher->addPath( ufile.filePath() );
      update();
    }
  }
}

/*!
  \brief Update user database slot
  \internal
*/
void Material_ResourceMgr::update()
{
  Updater( this ).wait();
  emit changed();
}
