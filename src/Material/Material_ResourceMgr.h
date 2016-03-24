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

// File   : Material_ResourceMgr.h
// Author : Margarita KARPUNINA, Open CASCADE S.A.S. (margarita.karpunina@opencascade.com)
//
#ifndef MATERIAL_RESOURCEMGR_H
#define MATERIAL_RESOURCEMGR_H

#include "Material.h"

#include <QObject>
#include <QMutex>
#include <QtxResourceMgr.h>

class QFileSystemWatcher;

class MATERIAL_SALOME_EXPORT Material_ResourceMgr : public QObject, public QtxResourceMgr
{
  Q_OBJECT;

  class Updater;

public:
  //! Material type
  typedef enum {
    Global,            //!< Global materials
    User,              //!< User materials
    All                //!< All materials
  } MaterialType;

  Material_ResourceMgr();
  ~Material_ResourceMgr();

  static Material_ResourceMgr* resourceMgr();

  QStringList materials( MaterialType = All, bool = true );

signals:
  void changed();

private:
  void watchUserFile( bool );
  void saved();

private slots:
  void update();

private:
  QFileSystemWatcher* myWatcher;
  QMutex              myMutex;

  friend class Material_Model;
};

#endif // MATERIAL_RESOURCEMGR_H
