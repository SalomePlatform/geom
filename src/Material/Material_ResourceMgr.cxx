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

// File   : Material_ResourceMgr.cxx
// Author : Margarita KARPUNINA, Open CASCADE S.A.S. (margarita.karpunina@opencascade.com)
//
#include "Material_ResourceMgr.h"

//#include "Qtx.h"    // used to print colors of global materials
//#include <iostream> // used to print colors of global materials

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
  : QtxResourceMgr( "SalomeMaterial", "%1Config" )
{
  if ( dirList().isEmpty() && ::getenv( "GEOM_ROOT_DIR" ) )
    setDirList( QStringList() << Qtx::addSlash( ::getenv( "GEOM_ROOT_DIR" ) ) + "share/salome/resources/geom" );
  setCurrentFormat( "xml" );

  /*
  // Get string equivalent for colors of global materials ---->
  QColor c;
  QString s;

  // 1. ============= Plastic
  std::cout << "---- Plastic:" << std::endl;
  // ambient
  c.setRgbF(0.2, 0.2, 0.2);
  s = Qtx::colorToString( c );
  std::cout << "        ambient color: " << s.toStdString().c_str() << std::endl;  
  // diffuse
  c.setRgbF(0.0, 0.0, 0.0);
  s = Qtx::colorToString( c );
  std::cout << "        diffuse color: " << s.toStdString().c_str() << std::endl;  
  // specular
  c.setRgbF(1.0, 1.0, 1.0);
  s = Qtx::colorToString( c );
  std::cout << "        specular color: " << s.toStdString().c_str() << std::endl;

  // 2. ============= Shiny plastic
  std::cout << "---- Shiny plastic:" << std::endl;
  // ambient
  c.setRgbF(0.2, 0.2, 0.2);
  s = Qtx::colorToString( c );
  std::cout << "        ambient color: " << s.toStdString().c_str() << std::endl;  
  // diffuse
  c.setRgbF(0.0, 0.0, 0.0);
  s = Qtx::colorToString( c );
  std::cout << "        diffuse color: " << s.toStdString().c_str() << std::endl;  
  // specular
  c.setRgbF(1.0, 1.0, 1.0);
  s = Qtx::colorToString( c );
  std::cout << "        specular color: " << s.toStdString().c_str() << std::endl;

  // 3. ============= Satin
  std::cout << "---- Satin:" << std::endl;
  // ambient
  c.setRgbF(0.2, 0.2, 0.2);
  s = Qtx::colorToString( c );
  std::cout << "        ambient color: " << s.toStdString().c_str() << std::endl;  
  // diffuse
  c.setRgbF(0.0, 0.0, 0.0);
  s = Qtx::colorToString( c );
  std::cout << "        diffuse color: " << s.toStdString().c_str() << std::endl;  
  // specular
  c.setRgbF(1.0, 1.0, 1.0);
  s = Qtx::colorToString( c );
  std::cout << "        specular color: " << s.toStdString().c_str() << std::endl;

  // 4. ============= Metal
  std::cout << "---- Metal:" << std::endl;
  // diffuse
  c.setRgbF(0.0, 0.0, 0.0);
  s = Qtx::colorToString( c );
  std::cout << "        diffuse color: " << s.toStdString().c_str() << std::endl;  
  // specular
  c.setRgbF(1.0, 1.0, 1.0);
  s = Qtx::colorToString( c );
  std::cout << "        specular color: " << s.toStdString().c_str() << std::endl;

  // 5. ============= Brass
  std::cout << "---- Brass:" << std::endl;
  // ambient
  c.setRgbF(0.329412, 0.223529, 0.027451);
  s = Qtx::colorToString( c );
  std::cout << "        ambient color: " << s.toStdString().c_str() << std::endl;  
  // diffuse
  c.setRgbF(0.780392, 0.568627, 0.113725);
  s = Qtx::colorToString( c );
  std::cout << "        diffuse color: " << s.toStdString().c_str() << std::endl;  
  // specular
  c.setRgbF(0.992157, 0.941176, 0.807843);
  s = Qtx::colorToString( c );
  std::cout << "        specular color: " << s.toStdString().c_str() << std::endl;  

  // 6. ============= Bronze
  std::cout << "---- Bronze:" << std::endl;
  // ambient
  c.setRgbF(0.2125, 0.1275, 0.054);
  s = Qtx::colorToString( c );
  std::cout << "        ambient color: " << s.toStdString().c_str() << std::endl;  
  // diffuse
  c.setRgbF(0.714, 0.4284, 0.18144);
  s = Qtx::colorToString( c );
  std::cout << "        diffuse color: " << s.toStdString().c_str() << std::endl;  
  // specular
  c.setRgbF(0.393548, 0.271906, 0.166721);
  s = Qtx::colorToString( c );
  std::cout << "        specular color: " << s.toStdString().c_str() << std::endl;

  // 7. ============= Copper
  std::cout << "---- Copper:" << std::endl;
  // ambient
  c.setRgbF(0.33, 0.26, 0.23);
  s = Qtx::colorToString( c );
  std::cout << "        ambient color: " << s.toStdString().c_str() << std::endl;  
  // diffuse
  c.setRgbF(0.50, 0.11, 0.0);
  s = Qtx::colorToString( c );
  std::cout << "        diffuse color: " << s.toStdString().c_str() << std::endl;  
  // specular
  c.setRgbF(0.95, 0.73, 0.0);
  s = Qtx::colorToString( c );
  std::cout << "        specular color: " << s.toStdString().c_str() << std::endl;

  // 8. ============= Gold
  std::cout << "---- Gold:" << std::endl;
  // ambient
  c.setRgbF(1.0, 0.76862745, 0.31764706);
  s = Qtx::colorToString( c );
  std::cout << "        ambient color: " << s.toStdString().c_str() << std::endl;  
  // diffuse
  c.setRgbF(1.0, 0.69, 0.0);
  s = Qtx::colorToString( c );
  std::cout << "        diffuse color: " << s.toStdString().c_str() << std::endl;  
  // specular
  c.setRgbF(1.0, 0.98, 0.78);
  s = Qtx::colorToString( c );
  std::cout << "        specular color: " << s.toStdString().c_str() << std::endl;

  // 9. ============= Pewter
  std::cout << "---- Pewter:" << std::endl;
  // ambient
  c.setRgbF(0.105882, 0.058824, 0.113725);
  s = Qtx::colorToString( c );
  std::cout << "        ambient color: " << s.toStdString().c_str() << std::endl;  
  // diffuse
  c.setRgbF(0.427451, 0.470588, 0.541176);
  s = Qtx::colorToString( c );
  std::cout << "        diffuse color: " << s.toStdString().c_str() << std::endl;  
  // specular
  c.setRgbF(0.333333, 0.333333, 0.521569);
  s = Qtx::colorToString( c );
  std::cout << "        specular color: " << s.toStdString().c_str() << std::endl;

  // 10. ============= Plaster
  std::cout << "---- Plaster:" << std::endl;
  // ambient
  c.setRgbF(0.19225, 0.19225, 0.19225);
  s = Qtx::colorToString( c );
  std::cout << "        ambient color: " << s.toStdString().c_str() << std::endl;  
  // diffuse
  c.setRgbF(0.50754, 0.50754, 0.50754);
  s = Qtx::colorToString( c );
  std::cout << "        diffuse color: " << s.toStdString().c_str() << std::endl;  
  // specular
  c.setRgbF(0.508273, 0.508273, 0.508273);
  s = Qtx::colorToString( c );
  std::cout << "        specular color: " << s.toStdString().c_str() << std::endl;

  // 11. ============= Silver
  std::cout << "---- Silver:" << std::endl;
  // ambient
  c.setRgbF(0.19225, 0.19225, 0.19225);
  s = Qtx::colorToString( c );
  std::cout << "        ambient color: " << s.toStdString().c_str() << std::endl;  
  // diffuse
  c.setRgbF(0.50754, 0.50754, 0.50754);
  s = Qtx::colorToString( c );
  std::cout << "        diffuse color: " << s.toStdString().c_str() << std::endl;  
  // specular
  c.setRgbF(0.508273, 0.508273, 0.508273);
  s = Qtx::colorToString( c );
  std::cout << "        specular color: " << s.toStdString().c_str() << std::endl;

  // 12. ============= Steel
  std::cout << "---- Steel:" << std::endl;
  // ambient
  c.setRgbF(0.2, 0.2, 0.2);
  s = Qtx::colorToString( c );
  std::cout << "        ambient color: " << s.toStdString().c_str() << std::endl;  
  // diffuse
  c.setRgbF(0.0, 0.0, 0.0);
  s = Qtx::colorToString( c );
  std::cout << "        diffuse color: " << s.toStdString().c_str() << std::endl;  
  // specular
  c.setRgbF(1.0, 1.0, 1.0);
  s = Qtx::colorToString( c );
  std::cout << "        specular color: " << s.toStdString().c_str() << std::endl;

  // 13. ============= Stone
  std::cout << "---- Stone:" << std::endl;
  // ambient
  c.setRgbF(1.0, 0.8, 0.62);
  s = Qtx::colorToString( c );
  std::cout << "        ambient color: " << s.toStdString().c_str() << std::endl;  
  // diffuse
  c.setRgbF(1.0, 0.8, 0.62);
  s = Qtx::colorToString( c );
  std::cout << "        diffuse color: " << s.toStdString().c_str() << std::endl;  
  // specular
  c.setRgbF(0.98, 1.0, 0.60);
  s = Qtx::colorToString( c );
  std::cout << "        specular color: " << s.toStdString().c_str() << std::endl;

  // 14. ============= Chrome
  std::cout << "---- Chrome:" << std::endl;
  // ambient
  c.setRgbF(0.35, 0.35, 0.35);
  s = Qtx::colorToString( c );
  std::cout << "        ambient color: " << s.toStdString().c_str() << std::endl;  
  // diffuse
  c.setRgbF(0.4, 0.4, 0.4);
  s = Qtx::colorToString( c );
  std::cout << "        diffuse color: " << s.toStdString().c_str() << std::endl;  
  // specular
  c.setRgbF(0.974597, 0.974597, 0.974597);
  s = Qtx::colorToString( c );
  std::cout << "        specular color: " << s.toStdString().c_str() << std::endl;

  // 15. ============= Neon
  std::cout << "---- Neon:" << std::endl;
  // ambient
  c.setRgbF(1.0, 1.0, 1.0);
  s = Qtx::colorToString( c );
  std::cout << "        ambient color: " << s.toStdString().c_str() << std::endl;  
  // diffuse
  c.setRgbF(1.0, 1.0, 1.0);
  s = Qtx::colorToString( c );
  std::cout << "        diffuse color: " << s.toStdString().c_str() << std::endl;  
  // specular
  c.setRgbF(1.0, 1.0, 1.0);
  s = Qtx::colorToString( c );
  std::cout << "        specular color: " << s.toStdString().c_str() << std::endl;
  // emissive
  c.setRgbF(0.0, 1.0, 0.46);
  s = Qtx::colorToString( c );
  std::cout << "        emissive color: " << s.toStdString().c_str() << std::endl;  

  // 16. ============= Aluminium
  std::cout << "---- Aluminium:" << std::endl;
  // ambient
  c.setRgbF(0.30, 0.30, 0.30);
  s = Qtx::colorToString( c );
  std::cout << "        ambient color: " << s.toStdString().c_str() << std::endl;  
  // diffuse
  c.setRgbF(0.30, 0.30, 0.30);
  s = Qtx::colorToString( c );
  std::cout << "        diffuse color: " << s.toStdString().c_str() << std::endl;  
  // specular
  c.setRgbF(0.70, 0.70, 0.80);
  s = Qtx::colorToString( c );
  std::cout << "        specular color: " << s.toStdString().c_str() << std::endl;

  // 17. ============= Obsidian
  std::cout << "---- Obsidian:" << std::endl;
  // ambient
  c.setRgbF(0.05375, 0.05, 0.06625);
  s = Qtx::colorToString( c );
  std::cout << "        ambient color: " << s.toStdString().c_str() << std::endl;  
  // diffuse
  c.setRgbF(0.18275, 0.17, 0.22525);
  s = Qtx::colorToString( c );
  std::cout << "        diffuse color: " << s.toStdString().c_str() << std::endl;  
  // specular
  c.setRgbF(0.332741, 0.328634, 0.346435);
  s = Qtx::colorToString( c );
  std::cout << "        specular color: " << s.toStdString().c_str() << std::endl;

  // 18. ============= Jade
  std::cout << "---- Jade:" << std::endl;
  // ambient
  c.setRgbF(0.135, 0.2225, 0.1575);
  s = Qtx::colorToString( c );
  std::cout << "        ambient color: " << s.toStdString().c_str() << std::endl;  
  // diffuse
  c.setRgbF(0.54, 0.89, 0.63);
  s = Qtx::colorToString( c );
  std::cout << "        diffuse color: " << s.toStdString().c_str() << std::endl;  
  // specular
  c.setRgbF(0.316228, 0.316228, 0.316228);
  s = Qtx::colorToString( c );
  std::cout << "        specular color: " << s.toStdString().c_str() << std::endl;

  // 19. ============= Default material
  std::cout << "---- Default material:" << std::endl;
  // ambient
  c.setRgbF(0.2, 0.2, 0.2);
  s = Qtx::colorToString( c );
  std::cout << "        ambient color: " << s.toStdString().c_str() << std::endl;  
  // diffuse
  c.setRgbF(0.0, 0.0, 0.0);
  s = Qtx::colorToString( c );
  std::cout << "        diffuse color: " << s.toStdString().c_str() << std::endl;  
  // specular
  c.setRgbF(1.0, 1.0, 1.0);
  s = Qtx::colorToString( c );
  std::cout << "        specular color: " << s.toStdString().c_str() << std::endl;

  // Get string equivalent for colors of global materials <----
  */
}

/*!
  \brief Destructor
*/
Material_ResourceMgr::~Material_ResourceMgr()
{
}

/*!
  \brief Get list of avaiable materials
  \param theType material type
  \param theSort if \c true (default), returns a list of materials sorted by name
  \return list of avaiable materials names
*/
QStringList Material_ResourceMgr::materials( MaterialType theType, bool theSort )
{
  QStringList sl;
  
  WorkingMode m = workingMode();

  switch ( theType ) {
  case Global:
    setWorkingMode( IgnoreUserValues );
    sl = sections();
    break;
  case User:
    {
      setWorkingMode( AllowUserValues );
      sl = sections();
      setWorkingMode( IgnoreUserValues );
      QMutableListIterator<QString> it( sl );
      while ( it.hasNext() ) {
        QString s = it.next();
        if ( hasSection( s ) ) it.remove();
      }
    }
    break;
  case All:
    setWorkingMode( AllowUserValues );
    sl = sections();
    break;
  default:
    break;
  }

  setWorkingMode( m );

  if ( theSort )
    qSort( sl );

  return sl;
}

/*!
  \brief Get list of materials names for preferences dialog
  \return list of materials names
*/
QStringList Material_ResourceMgr::getPreferenceMaterialsNames()
{
  QStringList aMaterialsList = materials( Material_ResourceMgr::All );
  return aMaterialsList;
}
