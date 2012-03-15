// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File   : Material_Model.cxx
// Author : Margarita KARPUNINA, Open CASCADE S.A.S. (margarita.karpunina@opencascade.com)
//

#include "Material_Model.h"
#include "Material_ResourceMgr.h"

#include <GEOM_Constants.h>

#include <QtxResourceMgr.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>

// OCCT Includes
#include <Graphic3d_AspectFillArea3d.hxx>

// VTK includes
#include <vtkProperty.h> 

/*!
  \brief Constructor

  Create new SALOME material model with default properties.
*/
Material_Model::Material_Model()
  : myResourceMgr( 0 )
{
  myShininess = 0.0;
}

/*!
  \brief Destructor
*/
Material_Model::~Material_Model()
{
}

/*!
  \brief Construct material model according to the given list of
  material properties
  
  \param theProps the list of material properties
  \return material model object with correspondent properties
  \sa getMaterialProperty()
*/
Material_Model* Material_Model::getMaterialModel( QStringList theProps )
{
  Material_Model* aModel = new Material_Model();

  foreach ( QString aProp, theProps ) {
    if ( aProp.isNull() ) continue;
    
    // Set current ambient color
    aModel->setColor( aProp, "AmbientColor=", Material_Model::Ambient );
    // Set current ambient coefficient
    aModel->setCoefficient( aProp, "AmbientCoefficient=", Material_Model::Ambient );
    
    // Set current diffuse color
    aModel->setColor( aProp, "DiffuseColor=", Material_Model::Diffuse );
    // Set current diffuse coefficient
    aModel->setCoefficient( aProp, "DiffuseCoefficient=", Material_Model::Diffuse );
    
    // Set current specular color
    aModel->setColor( aProp, "SpecularColor=", Material_Model::Specular );
    // Set current specular coefficient
    aModel->setCoefficient( aProp, "SpecularCoefficient=", Material_Model::Specular );
    
    // Set current emission color
    aModel->setColor( aProp, "EmissionColor=", Material_Model::Emission );
    // Set current emission coefficient
    aModel->setCoefficient( aProp, "EmissionCoefficient=", Material_Model::Emission );
    
    // Set current shininess
    QString aPropName = "Shininess=";
    int anId = aProp.indexOf(aPropName);
    if ( anId != -1 ) {
      bool ok;
      double aCoef = aProp.right( aProp.length() - (anId+aPropName.length()) ).toDouble(&ok);
      if ( ok )
	aModel->setShininess( aCoef );
    }    
  }

  return aModel;
}

/*!
  \brief Construct string of material properties for this model object
  
  \return a string representing a set of material properties
  \sa getMaterialModel()
*/
QString Material_Model::getMaterialProperty()
{
  // Parse material properties of the current model and form a string for persistent purpose
  QString aMaterial;

  bool isReflectionTypeActive;  
  QColor c;
  double coef;

  // Ambient reflection
  isReflectionTypeActive = hasAmbientReflection();
  if ( isReflectionTypeActive ) {
    c = color( Material_Model::Ambient );
    coef = coefficient(Material_Model::Ambient);
    // Insert properties into persistent string
    aMaterial = "AmbientColor=%1%2AmbientCoefficient=%3";
    aMaterial = aMaterial.arg( Qtx::colorToString(c) );
    aMaterial = aMaterial.arg( DIGIT_SEPARATOR );
    aMaterial = aMaterial.arg( coef );
  }
  // Diffuse reflection
  isReflectionTypeActive = hasDiffuseReflection();
  if ( isReflectionTypeActive ) {
    c = color( Material_Model::Diffuse );
    coef = coefficient(Material_Model::Diffuse);
    // Insert properties into persistent string
    aMaterial += "%1DiffuseColor=%2%3DiffuseCoefficient=%4";
    aMaterial = aMaterial.arg( DIGIT_SEPARATOR );
    aMaterial = aMaterial.arg( Qtx::colorToString(c) );
    aMaterial = aMaterial.arg( DIGIT_SEPARATOR );
    aMaterial = aMaterial.arg( coef );
  }
  // Specular reflection
  isReflectionTypeActive = hasSpecularReflection();
  if ( isReflectionTypeActive ) {
    c = color( Material_Model::Specular );
    coef = coefficient(Material_Model::Specular);
    // Insert properties into persistent string
    aMaterial += "%1SpecularColor=%2%3SpecularCoefficient=%4";
    aMaterial = aMaterial.arg( DIGIT_SEPARATOR );
    aMaterial = aMaterial.arg( Qtx::colorToString(c) );
    aMaterial = aMaterial.arg( DIGIT_SEPARATOR );
    aMaterial = aMaterial.arg( coef );
  }
  // Emission reflection
  isReflectionTypeActive = hasEmissionReflection();
  if ( isReflectionTypeActive ) {
    c = color( Material_Model::Emission );
    coef = coefficient(Material_Model::Emission);
    // Insert properties into persistent string
    aMaterial += "%1EmissionColor=%2%3EmissionCoefficient=%4";
    aMaterial = aMaterial.arg( DIGIT_SEPARATOR );
    aMaterial = aMaterial.arg( Qtx::colorToString(c) );
    aMaterial = aMaterial.arg( DIGIT_SEPARATOR );
    aMaterial = aMaterial.arg( coef );
  }
  if ( !aMaterial.isEmpty() ) {
    // Shininess
    // Insert properties into persistent string
    aMaterial += "%1Shininess=%2";
    aMaterial = aMaterial.arg( DIGIT_SEPARATOR );
    aMaterial = aMaterial.arg( shininess() );
  }

  return aMaterial;
}

/*!
  \brief Construct OCCT material aspect object based on the current model
  
  \return material aspect object with correspondent properties
*/
Graphic3d_MaterialAspect Material_Model::getMaterialOCCAspect()
{
  // Get material aspect from the current model
  Graphic3d_MaterialAspect aMat;

  bool isReflectionTypeActive;
  QColor c;
  double coef;

  // Ambient reflection
  isReflectionTypeActive = hasAmbientReflection();
  if ( isReflectionTypeActive ) {
    aMat.SetReflectionModeOn( Graphic3d_TOR_AMBIENT );
    c = color( Material_Model::Ambient );
    aMat.SetAmbientColor( Quantity_Color( c.redF(), c.greenF(), c.blueF(), Quantity_TOC_RGB ) );
    coef = coefficient( Material_Model::Ambient );
    aMat.SetAmbient( coef );
  }
  // Diffuse reflection
  isReflectionTypeActive = hasDiffuseReflection();
  if ( isReflectionTypeActive ) {
    aMat.SetReflectionModeOn( Graphic3d_TOR_DIFFUSE );
    c = color( Material_Model::Diffuse );
    aMat.SetDiffuseColor( Quantity_Color( c.redF(), c.greenF(), c.blueF(), Quantity_TOC_RGB ) );
    coef = coefficient( Material_Model::Diffuse );
    aMat.SetDiffuse( coef );	  
  }
  // Specular reflection
  isReflectionTypeActive = hasSpecularReflection();
  if ( isReflectionTypeActive ) {
    aMat.SetReflectionModeOn( Graphic3d_TOR_SPECULAR );
    c = color( Material_Model::Specular );
    aMat.SetSpecularColor( Quantity_Color( c.redF(), c.greenF(), c.blueF(), Quantity_TOC_RGB ) );
    coef = coefficient( Material_Model::Specular );
    aMat.SetSpecular( coef );
  }
  // Emission reflection
  isReflectionTypeActive = hasEmissionReflection();
  if ( isReflectionTypeActive ) {
    aMat.SetReflectionModeOn( Graphic3d_TOR_EMISSION );
    c = color( Material_Model::Emission );
    aMat.SetEmissiveColor( Quantity_Color( c.redF(), c.greenF(), c.blueF(), Quantity_TOC_RGB ) );
    coef = coefficient( Material_Model::Emission );
    aMat.SetEmissive( coef );
  }
  // Shininess
  aMat.SetShininess( shininess() );

  return aMat;
}

/*!
  \brief Construct VTK property with properties of material based on the current model
  
  \return VTK property with correspondent material properties
*/
vtkProperty* Material_Model::getMaterialVTKProperty()
{
  // Get material properties from the current model
  vtkProperty* aProperty = vtkProperty::New();
  
  bool isReflectionTypeActive;
  QColor c;
  double coef;

  // Ambient reflection
  isReflectionTypeActive = hasAmbientReflection();
  if ( isReflectionTypeActive ) {
    c = color( Material_Model::Ambient );
    aProperty->SetAmbientColor( c.redF(), c.greenF(), c.blueF() ); //SalomeApp_Tools::color( c )
    coef = coefficient( Material_Model::Ambient );
    aProperty->SetAmbient( coef );
  }
  // Diffuse reflection
  isReflectionTypeActive = hasDiffuseReflection();
  if ( isReflectionTypeActive ) {
    c = color( Material_Model::Diffuse );
    aProperty->SetDiffuseColor( c.redF(), c.greenF(), c.blueF() );
    coef = coefficient( Material_Model::Diffuse );
    aProperty->SetDiffuse( coef );	 
  }
  // Specular reflection
  isReflectionTypeActive = hasSpecularReflection();
  if ( isReflectionTypeActive ) {
    c = color( Material_Model::Specular );
    aProperty->SetSpecularColor( c.redF(), c.greenF(), c.blueF() );
    coef = coefficient( Material_Model::Specular );
    aProperty->SetSpecular( coef );
  }
  // Shininess
  aProperty->SetSpecularPower( shininess()*100.0 );

  return aProperty;
}

/*!
  \brief Initialize material model with default values
*/
void Material_Model::initDefaults()
{  
  // Set default ambient color
  setColor( Ambient, "#333333" );
  // Set default ambient coefficient
  setCoefficient( Ambient, 0.3 );

  // Set default diffuse color
  setColor( Diffuse, "#000000" );
  // Set default diffuse coefficient
  setCoefficient( Diffuse, 0.65 );

  // Set default specular color
  setColor( Specular, "#ffffff" );
  // Set default specular coefficient
  setCoefficient( Specular, 0.0 );
  
  // Set default shininess
  setShininess( 0.039 );
}

/*!
  \brief Clear current content of this material model
*/
void Material_Model::clearModel()
{
  myColors.clear();
  myCoefficients.clear();
  myShininess = 0.0;
}

/*!
  \brief Initialize material model from the resources

  This function can be used to retrieve material properties from the resource file(s).
  Note, that paremeters \a theResMgr and \a theResSection are stored by the model to be used
  later with save() method.

  \param theResMgr resources manager
  \param theResSection resources section name
  \param theIsFront if True, it is front material, else it is back material
  \sa save()
*/
void Material_Model::fromResources( QtxResourceMgr* theResMgr,
				    const QString& theResSection,
				    bool theIsFront )
{
  // Clear current content of the model
  // before setting properties from resources
  clearModel();

  myResourceMgr     = theResMgr;
  myResourceSection = theResSection;

  // init from resource manager
  if ( !resourceMgr() )
    return;

  if ( theResSection.compare( "Geometry" ) == 0 ) {
    if ( theIsFront ) {
      myResourceSection = theResMgr->stringValue("Geometry", "front_material", "Gold");
    }
    else {
      myResourceSection = theResMgr->stringValue("Geometry", "back_material", "");
      if ( myResourceSection.isEmpty() )
	myResourceSection = theResMgr->stringValue("Geometry", "front_material", "Gold");
    }
      
    myResourceMgr = new Material_ResourceMgr();
  }
  
  QString section = resourceSection( theIsFront );

  // If there is no material preference in XML files,
  // use the default material hardcoded in material model
  if ( section.isEmpty() ) {
    initDefaults();
    return;
  }

  // Set ambient color
  if ( resourceMgr()->hasValue( section, "ambient-color" ) ) {
    setColor( Ambient, resourceMgr()->colorValue( section, "ambient-color" ) );
  }
  // Set ambient coefficient
  if ( resourceMgr()->hasValue( section, "ambient-coefficient" ) ) {
    setCoefficient( Ambient, resourceMgr()->doubleValue( section, "ambient-coefficient" ) );
  }

  // Set diffuse color
  if ( resourceMgr()->hasValue( section, "diffuse-color" ) ) {
    setColor( Diffuse, resourceMgr()->colorValue( section, "diffuse-color" ) );
  }
  // Set diffuse coefficient
  if ( resourceMgr()->hasValue( section, "diffuse-coefficient" ) ) {
    setCoefficient( Diffuse, resourceMgr()->doubleValue( section, "diffuse-coefficient" ) );
  }

  // Set specular color
  if ( resourceMgr()->hasValue( section, "specular-color" ) ) {
    setColor( Specular, resourceMgr()->colorValue( section, "specular-color" ) );
  }
  // Set specular coefficient
  if ( resourceMgr()->hasValue( section, "specular-coefficient" ) ) {
    setCoefficient( Specular, resourceMgr()->doubleValue( section, "specular-coefficient" ) );
  }

  // Set emission color
  if ( resourceMgr()->hasValue( section, "emission-color" ) ) {
    setColor( Emission, resourceMgr()->colorValue( section, "emission-color" ) );
  }
  // Set emission coefficient
  if ( resourceMgr()->hasValue( section, "emission-coefficient" ) ) {
    setCoefficient( Emission, resourceMgr()->doubleValue( section, "emission-coefficient" ) );
  }

  // Set shininess
  if ( resourceMgr()->hasValue( section, "shininess" ) ) {
    setShininess( resourceMgr()->doubleValue( section, "shininess" ) );
  }
}

/*!
  \brief Save material properties to the resource file.
  
  If paremeters \a theResMgr and \a theResSection are not specified, default ones
  (those passed to the fromResources() function) are used instead.

  \param theResMgr resources manager
  \param theResSection resources section name
  \param theIsFront if True, it is front material, else it is back material
  \sa fromResources()
*/
void Material_Model::save( QtxResourceMgr* theResMgr,
			   const QString& theResSection,
			   bool theIsFront )
{
  if ( !theResMgr )
    theResMgr = resourceMgr();
  if ( !theResMgr )
    return;

  QString section = theResSection.isEmpty() ? resourceSection( theIsFront ) : theResSection;
  myResourceSection = section;

  if ( hasAmbientReflection() ) {
    // Save ambient color
    theResMgr->setValue( section, "ambient-color", color( Ambient ) );
    // Save ambient coefficient
    theResMgr->setValue( section, "ambient-coefficient", coefficient( Ambient ) );
  }
  else {
    // Remove ambient color
    theResMgr->remove( section, "ambient-color" );
    // Remove ambient coefficient
    theResMgr->remove( section, "ambient-coefficient" );
  }

  if ( hasDiffuseReflection() ) {
    // Save diffuse color
    theResMgr->setValue( section, "diffuse-color", color( Diffuse ) );
    // Save diffuse coefficient
    theResMgr->setValue( section, "diffuse-coefficient", coefficient( Diffuse ) );
  }
  else {
    // Remove diffuse color
    theResMgr->remove( section, "diffuse-color" );
    // Remove diffuse coefficient
    theResMgr->remove( section, "diffuse-coefficient" );
  }

  if ( hasSpecularReflection() ) {
    // Save specular color
    theResMgr->setValue( section, "specular-color", color( Specular ) );
    // Save specular coefficient
    theResMgr->setValue( section, "specular-coefficient", coefficient( Specular ) );
  }
  else {
    // Remove specular color
    theResMgr->remove( section, "specular-color" );
    // Remove specular coefficient
    theResMgr->remove( section, "specular-coefficient" );
  }

  if ( hasEmissionReflection() ) {
    // Save emission color
    theResMgr->setValue( section, "emission-color", color( Emission ) );
    // Save emission coefficient
    theResMgr->setValue( section, "emission-coefficient", coefficient( Emission ) );
  }
  else {
    // Remove emission color
    theResMgr->remove( section, "emission-color" );
    // Remove emission coefficient
    theResMgr->remove( section, "emission-coefficient" );
  }

  // Save shininess
  theResMgr->setValue( section, "shininess", shininess() );
}

/*!
  \brief Get resource manager used by this material model.

  \return pointer to the resource manager passed previously to the fromResources() method
  \sa fromResources(), resourceSection()
*/
QtxResourceMgr* Material_Model::resourceMgr() const
{
  return myResourceMgr;
}

/*!
  \brief Get resources section name

  If section name is empty, default material name from "Geometry" section
  is returned ("front_material" or "back_material" is used depending on
  the parameter value)

  \param theIsFront the flag indicating that section of front or back material
  is required
  \return resource section name passed previously to the fromResources() method
  \sa fromResources(), resourceMgr()
*/
QString Material_Model::resourceSection( bool theIsFront ) const
{
  return !myResourceSection.isEmpty() ? myResourceSection : 
    SUIT_Session::session()->resourceMgr()->stringValue("Geometry", 
							( theIsFront ? "front_material" : "back_material" ),
							"Gold");
}

/*!
  \brief Check if ambient reflection type is defined for this material

  \return true if ambient reflection type is defined for this material,
  false - otherwise
*/
bool Material_Model::hasAmbientReflection()
{
  return ( !myColors.isEmpty() && myColors.contains(Ambient) || !myCoefficients.isEmpty() && myCoefficients.contains(Ambient) );
}

/*!
  \brief Check if diffuse reflection type is defined for this material

  \return true if diffuse reflection type is defined for this material,
  false - otherwise
*/
bool Material_Model::hasDiffuseReflection()
{
  return ( !myColors.isEmpty() && myColors.contains(Diffuse) || !myCoefficients.isEmpty() && myCoefficients.contains(Diffuse) );
}

/*!
  \brief Check if specular reflection type is defined for this material

  \return true if specular reflection type is defined for this material,
  false - otherwise
*/
bool Material_Model::hasSpecularReflection()
{
  return ( !myColors.isEmpty() && myColors.contains(Specular) || !myCoefficients.isEmpty() && myCoefficients.contains(Specular) );
}

/*!
  \brief Check if emission reflection type is defined for this material

  \return true if emission reflection type is defined for this material,
  false - otherwise
*/
bool Material_Model::hasEmissionReflection()
{
  return ( !myColors.isEmpty() && myColors.contains(Emission) || !myCoefficients.isEmpty() && myCoefficients.contains(Emission) );
}

/*!
  \brief Get color value for the given reflection type
  \param theReflectionType reflection type
  \return a color which should be used by the given reflection type
  \sa setColor()
*/
QColor Material_Model::color( ReflectionType theReflectionType ) const
{
  return myColors[ theReflectionType ];
}

/*!
  \brief Set color value for the given reflection type

  \param theReflectionType reflection type
  \param theColor a color to be used by the given reflection type
  \sa color()
*/
void Material_Model::setColor( ReflectionType theReflectionType,
			       const QColor& theColor )
{
  myColors[ theReflectionType ] = theColor;
}

/*!
  \brief Set color of the current material from the given string
  \param theProp the considered property
  \param theColorName the name of the color property
  \param theReflectionType the type of reflection
*/
void Material_Model::setColor( QString theProp,
			       QString theColorName,
			       ReflectionType theReflectionType )
{
  int anId = theProp.indexOf( theColorName );
  if ( anId != -1 ) {
    QColor c;
    if ( Qtx::stringToColor( theProp.right( theProp.length() - ( anId + theColorName.length() ) ), c ) )
      setColor( theReflectionType, c );
  }
}

/*!
  \brief Remove color value for the given reflection type

  \param theReflectionType reflection type
  \sa color(), setColor()
*/
void Material_Model::removeColor( ReflectionType theReflectionType )
{
  myColors.remove( theReflectionType );
}

/*!
  \brief Get coefficient value for the given reflection type
  \param theReflectionType reflection type
  \return a coefficient which should be used by the given reflection type
  \sa setCoefficient()
*/
double Material_Model::coefficient( ReflectionType theReflectionType ) const
{
  return myCoefficients[ theReflectionType ];
}

/*!
  \brief Set coefficient value for the given reflection type

  \param theReflectionType reflection type
  \param theCoefficient a coefficient to be used by the given reflection type
  \sa coefficient()
*/
void Material_Model::setCoefficient( ReflectionType theReflectionType,
				     double theCoefficient )
{
  myCoefficients[ theReflectionType ] = theCoefficient;
}

/*!
  \brief Set coefficient of the current material from the given string
  \param theProp the considered property
  \param theCoefName the name of the color property
  \param theReflectionType the type of reflection
*/
void Material_Model::setCoefficient( QString theProp,
				     QString theCoefName,
				     ReflectionType theReflectionType )
{
  int anId = theProp.indexOf( theCoefName );
  if ( anId != -1 ) {
    bool ok;
    double aCoef = theProp.right( theProp.length() - ( anId + theCoefName.length() ) ).toDouble( &ok );
    if ( ok )
      setCoefficient( theReflectionType, aCoef );
  }
}

/*!
  \brief Remove coefficient value for the given reflection type

  \param theReflectionType reflection type
  \sa coefficient(), setCoefficient()
*/
void Material_Model::removeCoefficient( ReflectionType theReflectionType )
{
  myCoefficients.remove( theReflectionType );
}

/*!
  \brief Get shininess value
  \return a shininess value of this material
  \sa setShininess()
*/
double Material_Model::shininess() const
{
  return myShininess;
}

/*!
  \brief Set shininess value

  \param theShininess a shininess value of this material
  \sa shininess()
*/
void Material_Model::setShininess( double theShininess)
{
  myShininess = theShininess;
}
