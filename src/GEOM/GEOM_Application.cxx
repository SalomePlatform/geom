
using namespace std;
#include "GEOM_Application.ixx"

//=======================================================================
//function : GEOM_Application
//purpose  : 
//=======================================================================

GEOM_Application::GEOM_Application() 
{
}


//=======================================================================
//function : Formats
//purpose  : 
//=======================================================================

void GEOM_Application::Formats(TColStd_SequenceOfExtendedString& Formats) 
{  
  Formats.Append(TCollection_ExtendedString ("SALOME_GEOM"));
}


//=======================================================================
//function : ResourcesName
//purpose  : 
//=======================================================================

Standard_CString GEOM_Application::ResourcesName() 
{
  return Standard_CString ("GEOMDS_Resources");
}





