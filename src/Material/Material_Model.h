// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

class Graphic3d_MaterialAspect;

class GEOM_VTKPropertyMaterial;

class QtxResourceMgr;

class MATERIAL_SALOME_EXPORT Material_Model
{

public:

  //! Enumeration of reflection types of material
  typedef enum {
    Ambient,  //!< Ambient 
    Diffuse,  //!< Diffuse
    Specular, //!< Specular
    Emission  //!< Emission
  } ReflectionType;


  Material_Model();
  virtual ~Material_Model();

  static Material_Model* getMaterialModel( QStringList );

  QString                getMaterialProperty();

  Graphic3d_MaterialAspect getMaterialOCCAspect();
  GEOM_VTKPropertyMaterial*             getMaterialVTKProperty();

  void                initDefaults();
  void                fromResources( QtxResourceMgr*, const QString& = QString());
  void                save( QtxResourceMgr* = 0, const QString& = QString() );

  QtxResourceMgr*     resourceMgr() const;
  QString             resourceSection() const;

  bool                hasAmbientReflection();
  bool                hasDiffuseReflection();
  bool                hasSpecularReflection();
  bool                hasEmissionReflection();

  QColor              color( ReflectionType ) const;
  void                setColor( ReflectionType, const QColor& );
  void                setColor( QString,
				                        QString,
				                        ReflectionType );
  void                removeColor( ReflectionType );

  double              coefficient( ReflectionType ) const;
  void                setCoefficient( ReflectionType, double );  
  void                setCoefficient( QString,
				                              QString,
				                              ReflectionType );
  void                removeCoefficient( ReflectionType );

  double              shininess() const;
  void                setShininess( double );  

  bool                isPhysical() const;
  void                setPhysical( bool );

private:
  void                clearModel();

private:
  typedef QMap<ReflectionType, QColor> ColorMap;
  typedef QMap<ReflectionType, double> CoefficientMap;

  QtxResourceMgr*     myResourceMgr;
  QString             myResourceSection;

  ColorMap            myColors;
  CoefficientMap      myCoefficients;

  double              myShininess;

  bool                myIsPhysical;

};

#endif // MATERIAL_MODEL_H
