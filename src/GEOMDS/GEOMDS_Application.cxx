using namespace std;
//  File      : GEOMDS_Application.cxx
//  Created   :
//  Author    : Yves FRICAUD
//  Project   : SALOME
//  Module    : GEOM
//  Copyright : OPEN CASCADE
//  $Header$


#include "GEOMDS_Application.ixx"

//=======================================================================
//function : GEOMDS_Application
//purpose  : 
//=======================================================================

GEOMDS_Application::GEOMDS_Application() 
{
}


//=======================================================================
//function : Formats
//purpose  : 
//=======================================================================

void GEOMDS_Application::Formats(TColStd_SequenceOfExtendedString& Formats) 
{  
  Formats.Append(TCollection_ExtendedString ("SALOME_GEOM"));
}


//=======================================================================
//function : ResourcesName
//purpose  : 
//=======================================================================

Standard_CString GEOMDS_Application::ResourcesName() 
{
  return Standard_CString ("GEOMDS_Resources");
}





