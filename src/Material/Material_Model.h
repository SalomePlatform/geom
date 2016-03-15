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

// File   : Material_Model.h
// Author : Margarita KARPUNINA, Open CASCADE S.A.S. (margarita.karpunina@opencascade.com)
//
#ifndef MATERIAL_MODEL_H
#define MATERIAL_MODEL_H

#include "Material.h"

#include <QColor>
#include <QMap>
#include <QString>
#include <QVector>

#include <Graphic3d_MaterialAspect.hxx>

class Material_ResourceMgr;
class GEOM_VTKPropertyMaterial;

class MATERIAL_SALOME_EXPORT Material_Model
{
public:
  //! Reflection type
  typedef enum {
    Ambient,  //!< Ambient 
    Diffuse,  //!< Diffuse
    Specular, //!< Specular
    Emissive  //!< Emissive
  } ReflectionType;

  Material_Model();
  virtual ~Material_Model();

  void                fromProperties( const QString& );
  QString             toProperties();
  void                fromResources( const QString& = QString(), Material_ResourceMgr* = 0 );
  void                toResources( const QString&, Material_ResourceMgr* );

  bool                isPhysical() const;
  void                setPhysical( bool );

  bool                hasReflection( ReflectionType ) const;
  void                setReflection( ReflectionType, bool );

  QColor              color( ReflectionType ) const;
  void                setColor( ReflectionType, const QColor& );

  double              reflection( ReflectionType, bool = true ) const;
  void                setReflection( ReflectionType, double, bool = true );

  double              shininess( bool = true) const;
  void                setShininess( double, bool = true );

  double              transparency() const;
  void                setTransparency( double );

  Graphic3d_MaterialAspect  getMaterialOCCAspect( bool = true );
  GEOM_VTKPropertyMaterial* getMaterialVTKProperty( bool = true );

private:
  void                init();
  void                read( const QString&, Material_ResourceMgr* );

private:
  typedef struct {
    QColor color;
    double front_coef;
    double back_coef;
    bool   enabled;
  } ReflectionData;

  typedef QVector<ReflectionData> ReflectionList;

  bool                myIsPhysical;
  double              myFrontShininess;
  double              myBackShininess;
  double              myTransparency;
  ReflectionList      myReflection;
};

#endif // MATERIAL_MODEL_H
