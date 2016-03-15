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

// File   : Material_Model.cxx
// Author : Margarita KARPUNINA, Open CASCADE S.A.S. (margarita.karpunina@opencascade.com)
//

#include "Material_Model.h"
#include "GEOM_VTKPropertyMaterial.hxx"
#include "Material_ResourceMgr.h"

#include <QMutexLocker>

/*!
  \brief Constructor

  Create new SALOME material model with default properties.
*/
Material_Model::Material_Model()
{
  myReflection  = ReflectionList(4);
  init(); // set default properties
}

/*!
  \brief Destructor
*/
Material_Model::~Material_Model()
{
}

/*!
  \brief Initialize material data from the given properties list
  \param props material properties
  \sa toProperties()
*/
void Material_Model::fromProperties( const QString& props )
{
  // reset to default values
  init();

  // parse material properties
  QStringList propList = props.split( ":", QString::SkipEmptyParts );
  foreach ( QString prop, propList ) 
  {
    QStringList pdata = prop.split( "=" );
    if ( pdata.count() < 2 ) continue;
    QString key   = pdata[0].trimmed().toLower();
    QString data  = pdata[1].trimmed().toLower();
    bool dblOk, boolOk;
    double dblValue  = data.toDouble( &dblOk );
    bool   boolValue = (bool)( data.toInt( &boolOk ) );
    QColor colorValue;
    
    if      ( key == "ambientcolor" && Qtx::stringToColor( data, colorValue ) ) {
      setColor( Ambient, colorValue );
    }
    else if ( key == "diffusecolor" && Qtx::stringToColor( data, colorValue ) ) {
      setColor( Diffuse, colorValue );
    }
    else if ( key == "specularcolor" && Qtx::stringToColor( data, colorValue ) ) {
      setColor( Specular, colorValue );
    }
    else if ( key == "emissivecolor" && Qtx::stringToColor( data, colorValue ) ) {
      setColor( Emissive, colorValue );
    }
    else if ( key == "frontambientcoefficient" && dblOk ) {
      setReflection( Ambient, dblValue );
    }
    else if ( key == "backambientcoefficient" && dblOk ) {
      setReflection( Ambient, dblValue, false );
    }
    else if ( key == "frontdiffusecoefficient" && dblOk ) {
      setReflection( Diffuse, dblValue );
    }
    else if ( key == "backdiffusecoefficient" && dblOk ) {
      setReflection( Diffuse, dblValue, false );
    }
    else if ( key == "frontspecularcoefficient" && dblOk ) {
      setReflection( Specular, dblValue );
    }
    else if ( key == "backspecularcoefficient" && dblOk ) {
      setReflection( Specular, dblValue, false );
    }
    else if ( key == "frontemissivecoefficient" && dblOk ) {
      setReflection( Emissive, dblValue );
    }
    else if ( key == "backemissivecoefficient" && dblOk ) {
      setReflection( Emissive, dblValue, false );
    }
    else if ( key == "frontshininess" && dblOk ) {
      setShininess( dblValue );
    }
    else if ( key == "backshininess" && dblOk ) {
      setShininess( dblValue, false );
    }
    else if ( key == "transparency" && dblOk ) {
      setTransparency( dblValue );
    }
    else if ( key == "physical" && boolOk ) {
      setPhysical( boolValue );
    }
    else if ( key == "ambient" && boolOk ) {
      setReflection( Ambient, boolValue );
    }
    else if ( key == "diffuse" && boolOk ) {
      setReflection( Diffuse, boolValue );
    }
    else if ( key == "specular" && boolOk ) {
      setReflection( Specular, boolValue );
    }
    else if ( key == "emissive" && boolOk ) {
      setReflection( Emissive, boolValue );
    }
  }
}

/*!
  \brief Get material properties string representation
  \return string representing of material properties
  \sa fromProperties()
*/
QString Material_Model::toProperties()
{
  QStringList props;
  QString fmt = "%1=%2";

  // physical
  props << fmt.arg( "Physical" ).arg( isPhysical() );

  // shininess
  props << fmt.arg( "FrontShininess" ).arg( QString::number ( shininess( true ), 'g', 4 ) ) << fmt.arg( "BackShininess" ).arg( QString::number ( shininess( false ), 'g', 4 ) );

  //transparency
  props << fmt.arg( "Transparency" ).arg( transparency() );

  // ambient reflection
  props << fmt.arg( "Ambient" ).arg( hasReflection( Ambient ) );
  if ( color( Ambient ).isValid() )
    props << fmt.arg( "AmbientColor" ).arg( Qtx::colorToString( color( Ambient ) ) );
  props << fmt.arg( "FrontAmbientCoefficient" ).arg( QString::number ( reflection( Ambient, true ), 'g', 4 ) ) << fmt.arg( "BackAmbientCoefficient" ).arg( QString::number ( reflection( Ambient, false ), 'g', 4 ) );

  // diffuse reflection
  props << fmt.arg( "Diffuse" ).arg( hasReflection( Diffuse ) );
  if ( color( Diffuse ).isValid() )
    props << fmt.arg( "DiffuseColor" ).arg( Qtx::colorToString( color( Diffuse ) ) );
  props << fmt.arg( "FrontDiffuseCoefficient" ).arg( QString::number ( reflection( Diffuse, true ), 'g', 4 ) ) << fmt.arg( "BackDiffuseCoefficient" ).arg( QString::number ( reflection( Diffuse, false ), 'g', 4 ) );

  // specular reflection
  props << fmt.arg( "Specular" ).arg( hasReflection( Specular ) );
  if ( color( Specular ).isValid() )
    props << fmt.arg( "SpecularColor" ).arg( Qtx::colorToString( color( Specular ) ) );
  props << fmt.arg( "FrontSpecularCoefficient" ).arg( QString::number ( reflection( Specular, true ), 'g', 4 ) ) << fmt.arg( "BackSpecularCoefficient" ).arg( QString::number ( reflection( Specular, false ), 'g', 4 ) );

  // emissive reflection
  props << fmt.arg( "Emissive" ).arg( hasReflection( Emissive ) );
  if ( color( Emissive ).isValid() )
    props << fmt.arg( "EmissiveColor" ).arg( Qtx::colorToString( color( Emissive ) ) );
  props << fmt.arg( "FrontEmissiveCoefficient" ).arg( QString::number ( reflection( Emissive, true ), 'g', 4 ) ) << fmt.arg( "BackEmissiveCoefficient" ).arg( QString::number ( reflection( Emissive, false ), 'g', 4 ) );

  return props.join( ":" );
}

/*!
  \brief Initialize material model from the resources

  This function can be used to retrieve material properties from the resource file.

  \param material material name (if not specified, model is initialized by default material)
  \param resMgr resource manager (if not specified, new resources manager is created)
  \sa toResources()
*/
void Material_Model::fromResources( const QString& material, Material_ResourceMgr* resMgr )
{
  static QString common = "[common]";
  
  // reset to default values
  init();

  // material name is not specified: use default values
  if ( material.isEmpty() ) return;

  if ( !resMgr )
    resMgr = Material_ResourceMgr::resourceMgr();

  // lock resources manager
  QMutexLocker lock( &resMgr->myMutex );

  // read common section
  if ( material != common && resMgr->hasSection( common ) )
    read( common, resMgr );

  // read material section
  read( material, resMgr );
}

void Material_Model::read( const QString& material, Material_ResourceMgr* resMgr )
{
  // physical
  if ( resMgr->hasValue( material, "physical" ) ) {
    setPhysical( resMgr->booleanValue( material, "physical" ) );
  }

  // shininess
  if ( resMgr->hasValue( material, "front_shininess" ) ) {
    setShininess( resMgr->doubleValue( material, "front_shininess" ) );
  }
  if ( resMgr->hasValue( material, "back_shininess" ) ) {
    setShininess( resMgr->doubleValue( material, "back_shininess" ), false );
  }

  // transparency
  if ( resMgr->hasValue( material, "transparency" ) ) {
    setTransparency( resMgr->doubleValue( material, "transparency" ) );
  }

  // ambient reflection
  if ( resMgr->hasValue( material, "ambient-color" ) ) {
    setColor( Ambient, resMgr->colorValue( material, "ambient-color" ) );
  }
  if ( resMgr->hasValue( material, "front_ambient-coefficient" ) ) {
    setReflection( Ambient, resMgr->doubleValue( material, "front_ambient-coefficient" ) );
  }
  if ( resMgr->hasValue( material, "back_ambient-coefficient" ) ) {
    setReflection( Ambient, resMgr->doubleValue( material, "back_ambient-coefficient" ), false );
  }
  if ( resMgr->hasValue( material, "ambient" ) ) {
    setReflection( Ambient, resMgr->booleanValue( material, "ambient" ) );
  }

  // diffuse reflection
  if ( resMgr->hasValue( material, "diffuse-color" ) ) {
    setColor( Diffuse, resMgr->colorValue( material, "diffuse-color" ) );
  }
  if ( resMgr->hasValue( material, "front_diffuse-coefficient" ) ) {
    setReflection( Diffuse, resMgr->doubleValue( material, "front_diffuse-coefficient" ) );
  }
  if ( resMgr->hasValue( material, "back_diffuse-coefficient" ) ) {
    setReflection( Diffuse, resMgr->doubleValue( material, "back_diffuse-coefficient" ), false );
  }
  if ( resMgr->hasValue( material, "diffuse" ) ) {
    setReflection( Diffuse, resMgr->booleanValue( material, "diffuse" ) );
  }

  // specular reflection
  if ( resMgr->hasValue( material, "specular-color" ) ) {
    setColor( Specular, resMgr->colorValue( material, "specular-color" ) );
  }
  if ( resMgr->hasValue( material, "front_specular-coefficient" ) ) {
    setReflection( Specular, resMgr->doubleValue( material, "front_specular-coefficient" ) );
  }
  if ( resMgr->hasValue( material, "back_specular-coefficient" ) ) {
    setReflection( Specular, resMgr->doubleValue( material, "back_specular-coefficient" ), false );
  }
  if ( resMgr->hasValue( material, "specular" ) ) {
    setReflection( Specular, resMgr->booleanValue( material, "specular" ) );
  }

  // emissive reflection
  if ( resMgr->hasValue( material, "emissive-color" ) ) {
    setColor( Emissive, resMgr->colorValue( material, "emissive-color" ) );
  }
  if ( resMgr->hasValue( material, "front_emissive-coefficient" ) ) {
    setReflection( Emissive, resMgr->doubleValue( material, "front_emissive-coefficient" ) );
  }
  if ( resMgr->hasValue( material, "back_emissive-coefficient" ) ) {
    setReflection( Emissive, resMgr->doubleValue( material, "back_emissive-coefficient" ), false );
  }
  if ( resMgr->hasValue( material, "emissive" ) ) {
    setReflection( Emissive, resMgr->booleanValue( material, "emissive" ) );
  }
}

/*!
  \brief Save material properties to the resource file.
  \param material material name
  \param resMgr resource manager
  \sa fromResources()
*/
void Material_Model::toResources( const QString& material, Material_ResourceMgr* resMgr )
{
  if ( resMgr && !material.isEmpty() ) {
    // lock resources manager
    QMutexLocker lock( &resMgr->myMutex );
  
    // remove resources section (to clean-up all previous properties)
    resMgr->remove( material );

    // physical
    resMgr->setValue( material, "physical", isPhysical() );

    // shininess
    resMgr->setValue( material, "front_shininess", shininess( true) );
    resMgr->setValue( material, "back_shininess", shininess( false ) );

    // transparency
    resMgr->setValue( material, "transparency", transparency() );

    // ambient reflection
    if ( color( Ambient ).isValid() )
      resMgr->setValue( material, "ambient-color", color( Ambient ) );
    resMgr->setValue( material, "front_ambient-coefficient", reflection( Ambient ) );
    resMgr->setValue( material, "back_ambient-coefficient", reflection( Ambient, false ) );
    resMgr->setValue( material, "ambient", hasReflection( Ambient ) );

    // diffuse reflection
    if ( color( Diffuse ).isValid() )
      resMgr->setValue( material, "diffuse-color", color( Diffuse ) );
    resMgr->setValue( material, "front_diffuse-coefficient", reflection( Diffuse ) );
    resMgr->setValue( material, "back_diffuse-coefficient", reflection( Diffuse, false ) );
    resMgr->setValue( material, "diffuse", hasReflection( Diffuse ) );

    // Specular reflection
    if ( color( Specular ).isValid() )
      resMgr->setValue( material, "specular-color", color( Specular ) );
    resMgr->setValue( material, "front_specular-coefficient", reflection( Specular ) );
    resMgr->setValue( material, "back_specular-coefficient", reflection( Specular, false ) );
    resMgr->setValue( material, "specular", hasReflection( Specular ) );

    // Emissive reflection
    if ( color( Emissive ).isValid() )
      resMgr->setValue( material, "emissive-color", color( Emissive ) );
    resMgr->setValue( material, "front_emissive-coefficient", reflection( Emissive ) );
    resMgr->setValue( material, "back_emissive-coefficient", reflection( Emissive, false ) );
    resMgr->setValue( material, "emissive", hasReflection( Emissive ) );
  }
}

/*!
  \brief Get material type
  \return \c true if material is physical or \c false otherwise
  \sa setPhysical()
*/
bool Material_Model::isPhysical() const
{
  return myIsPhysical;
}

/*!
  \brief Set material type
  \param value \c true if material is physical or \c false otherwise
  \sa isPhysical()
*/
void Material_Model::setPhysical( bool value )
{
  myIsPhysical = value;
}

/*!
  \brief Check if given reflection is enabled
  \param type reflection type
  \return \c true if specified reflection type is enabled or \c false otherwise
  \sa setReflection(ReflectionType, bool)
*/
bool Material_Model::hasReflection( ReflectionType type ) const
{
  bool value = false;
  if ( type >= 0 && type < 4 )
    value = myReflection[ type ].enabled;
  return value;
}

/*!
  \brief Enable/disable given reflection type
  \param type reflection type
  \param value boolean flag
  \sa hasReflection()
*/
void Material_Model::setReflection( ReflectionType type, bool value )
{
  if ( type >= 0 && type < 4 )
    myReflection[ type ].enabled = value;
}

/*!
  \brief Get color value for the given reflection type
  \param type reflection type
  \return color associated with the specified reflection type
  \sa setColor()
*/
QColor Material_Model::color( ReflectionType type ) const
{
  QColor value;
  if ( type >= 0 && type < 4 )
    value = myReflection[ type ].color;
  return value;
}

/*!
  \brief Set color value for the given reflection type
  \param type reflection type
  \param value color to be used for specified reflection type
  \sa color()
*/
void Material_Model::setColor( ReflectionType type, const QColor& value )
{
  if ( type >= 0 && type < 4 )
    myReflection[ type ].color = value;
}

/*!
  \brief Get coefficient value for the given reflection type
  \param type reflection type
  \param theIsFront boolean flag for choosing side
  \return coefficient value for the specified reflection type
  \sa setReflection(ReflectionType, double, bool = true)
*/
double Material_Model::reflection( ReflectionType type, bool theIsFront ) const
{
  double value = 0.0;
  if ( type >= 0 && type < 4 )
  {
    if ( theIsFront )
      value = myReflection[ type ].front_coef;
    else
      value = myReflection[ type ].back_coef;
  }
  return value;
}

/*!
  \brief Set coefficient value for the given reflection type
  \param type reflection type
  \param value coefficient to be used by the given reflection type
  \param theIsFront boolean flag for choosing side
  \sa reflection( bool = true)
*/
void Material_Model::setReflection( ReflectionType type, double value, bool theIsFront )
{
  if ( type >= 0 && type < 4 )
  {
    if ( theIsFront )
      myReflection[ type ].front_coef = value;
    else
      myReflection[ type ].back_coef = value;
  }
}

/*!
  \brief Get shininess value
  \param theIsFront boolean flag for choosing side
  \return shininess value of the material
  \sa setShininess( double, bool = true )
*/
double Material_Model::shininess( bool theIsFront ) const
{
  if ( theIsFront )
    return myFrontShininess;
  else
    return myBackShininess;
}

/*!
  \brief Set shininess value
  \param value new shininess value
  \param theIsFront boolean flag for choosing side
  \sa shininess( bool = true )
*/
void Material_Model::setShininess( double value, bool theIsFront )
{
  if ( theIsFront )
    myFrontShininess = value;
  else
    myBackShininess = value;
}

/*!
  \brief Get transparency value
  \return transparency value of the material
  \sa setTransparency()
*/
double Material_Model::transparency() const
{
  return myTransparency;
}

/*!
  \brief Set transparency value
  \param value new transparency value
  \sa transparency()
*/
void Material_Model::setTransparency( double value )
{
  myTransparency = value;
}

/*!
  \brief Initialize material model with default values
*/
void Material_Model::init()
{  
  QColor c;

  // non-physical by default
  setPhysical( false );
  // shininess
  setShininess( 0.039 );
  setShininess( 0.039, false );
  // transparency
  setTransparency( 0.0 );

  // ambient reflection (enabled by default)
  Qtx::stringToColor( "#333333", c );
  setColor( Ambient, c );
  setReflection( Ambient, 0.3, true );
  setReflection( Ambient, 0.3, false );
  setReflection( Ambient, true );

  // diffuse reflection (enabled by default)
  Qtx::stringToColor( "#000000", c );
  setColor( Diffuse, c );
  setReflection( Diffuse, 0.65 );
  setReflection( Diffuse, 0.65, false );
  setReflection( Diffuse, true );

  // specular reflection (enabled by default)
  Qtx::stringToColor( "#ffffff", c );
  setColor( Specular, c );
  setReflection( Specular, 0.0 );
  setReflection( Specular, 0.0, false );
  setReflection( Specular, true );

  // emissive reflection (disabled by default)
  Qtx::stringToColor( "#000000", c );
  setColor( Emissive, c );
  setReflection( Emissive, 0.0 );
  setReflection( Emissive, 0.0, false );
  setReflection( Emissive, false );
}

/*!
  \brief Construct OCCT material aspect from material model
  \param theIsFront boolean flag for choosing side
  \return material aspect object with corresponding properties
*/
Graphic3d_MaterialAspect Material_Model::getMaterialOCCAspect( bool theIsFront )
{
  // Get material aspect from the current model
  Graphic3d_MaterialAspect aspect;

  QColor c;
  
  // ambient reflection
  if ( color( Ambient ).isValid() ) {
    c = color( Ambient );
    aspect.SetAmbientColor( Quantity_Color( c.redF(), c.greenF(), c.blueF(), Quantity_TOC_RGB ) );
  }
  aspect.SetAmbient( reflection( Ambient, theIsFront ));
  if ( hasReflection( Ambient ) )
    aspect.SetReflectionModeOn( Graphic3d_TOR_AMBIENT );
  else
    aspect.SetReflectionModeOff( Graphic3d_TOR_AMBIENT );
  
  // diffuse reflection
  if ( color( Diffuse ).isValid() ) {
    c = color( Diffuse );
    aspect.SetDiffuseColor( Quantity_Color( c.redF(), c.greenF(), c.blueF(), Quantity_TOC_RGB ) );
  }
  aspect.SetDiffuse( reflection( Diffuse, theIsFront ));
  if ( hasReflection( Diffuse ) )
    aspect.SetReflectionModeOn( Graphic3d_TOR_DIFFUSE );
  else
    aspect.SetReflectionModeOff( Graphic3d_TOR_DIFFUSE );

  // specular reflection
  if ( color( Specular ).isValid() ) {
    c = color( Specular );
    aspect.SetSpecularColor( Quantity_Color( c.redF(), c.greenF(), c.blueF(), Quantity_TOC_RGB ) );
  }
  aspect.SetSpecular( reflection( Specular, theIsFront ));
  if ( hasReflection( Specular ) )
    aspect.SetReflectionModeOn( Graphic3d_TOR_SPECULAR );
  else
    aspect.SetReflectionModeOff( Graphic3d_TOR_SPECULAR );

  // emissive reflection
  if ( color( Emissive ).isValid() ) {
    c = color( Emissive );
    aspect.SetEmissiveColor( Quantity_Color( c.redF(), c.greenF(), c.blueF(), Quantity_TOC_RGB ) );
  }
  aspect.SetEmissive( reflection( Emissive, theIsFront ));
  if ( hasReflection( Emissive ) )
    aspect.SetReflectionModeOn( Graphic3d_TOR_EMISSION );
  else
    aspect.SetReflectionModeOff( Graphic3d_TOR_EMISSION );
  
  // shininess
  aspect.SetShininess( shininess( theIsFront ) );

  // transparency
  aspect.SetTransparency( transparency() );

  // material type
  aspect.SetMaterialType( isPhysical() ? Graphic3d_MATERIAL_PHYSIC : Graphic3d_MATERIAL_ASPECT );

  return aspect;
}

/*!
  \brief Construct VTK property from material model
  \param theIsFront boolean flag for choosing side
  \return VTK property with correspondent material properties
*/
GEOM_VTKPropertyMaterial* Material_Model::getMaterialVTKProperty( bool theIsFront )
{
  // NOTE: In VTK it's impossible to switch on/off specific reflection type
  // NOTE: In VTK emissive reflection type is not supported
  // NOTE: In VTK shininess is specified via SpecularPower attribute
  // NOTE: In VTK transparency is specified via Opacity attribute

  // Get material properties from the current model
  GEOM_VTKPropertyMaterial* prop = GEOM_VTKPropertyMaterial::New();

  QColor c;

  // ambient reflection
  if ( color( Ambient ).isValid() && hasReflection( Ambient ) ) {
    c = color( Ambient );
    prop->SetAmbientColor( c.redF(), c.greenF(), c.blueF() );
    prop->SetAmbient( reflection( Ambient, theIsFront ) );
  }

  // diffuse reflection
  if ( color( Diffuse ).isValid() && hasReflection( Diffuse ) ) {
    c = color( Diffuse );
    prop->SetDiffuseColor( c.redF(), c.greenF(), c.blueF() );
    prop->SetDiffuse( reflection( Diffuse, theIsFront ) );
  }

  // specular reflection
  if ( color( Specular ).isValid() && hasReflection( Specular ) ) {
    c = color( Specular );
    prop->SetSpecularColor( c.redF(), c.greenF(), c.blueF() );
    prop->SetSpecular( reflection( Specular, theIsFront ) );
  }

  // shininess
  prop->SetSpecularPower( shininess( theIsFront )*100.0 );

  // transparency
  prop->SetOpacity( 1 - transparency() );

  // material type
  prop->SetPhysical( isPhysical() );

  return prop;
}

