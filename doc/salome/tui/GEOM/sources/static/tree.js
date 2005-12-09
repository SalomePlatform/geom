foldersTree = gFld("<b>SALOME v.3.1.0 </b>", "", "")
     insDoc(foldersTree, gLnk("Main Page", "", "main.html"))

aux1 = insFld(foldersTree, gFld("TUI Reference Guide", ""))
  aux2 = insFld(aux1, gFld("Modules", ""))
    aux3 = insFld(aux2, gFld("SALOME GEOMETRY module", ""))
/*!             insDoc(aux3, gLnk("Overview", "", "overview_Geometry.html"))*/
      aux4 = insFld(aux3, gFld("Packages", "")) 		
               insDoc(aux4, gLnk("GEOM", "", "namespaceGEOM.html"))
/*!             insDoc(aux3, gLnk("Examples", "", "examples_GEOM.html"))
*/

/*! Data structures
*/
         insDoc(aux1, gLnk("Data Structures", "", "annotated.html"))

/*!    insDoc(aux1, gLnk("GEOM::GEOM_Gen", "", "interfaceGEOM_1_1GEOM__Gen.html"))
       insDoc(aux1, gLnk("GEOM::GEOM_Shape", "", "interfaceGEOM_1_1GEOM__Shape.html"))
*/
         insDoc(aux1, gLnk("Class Hierarchy", "", "hierarchy.html"))

/*!    
    aux3 = insFld(aux2, gFld("GEOM::GEOM_Gen", "", "interfaceGEOM_1_1GEOM__Gen.html"))
    aux3 = insFld(aux2, gFld("GEOM::GEOM_Shape", "", "interfaceGEOM_1_1GEOM__Shape.html"))
    aux3 = insFld(aux2, gFld("GEOM::AxisStruct", "", "structGEOM_1_1AxisStruct.html"))
    aux3 = insFld(aux2, gFld("GEOM::DirStruct", "", "structGEOM_1_1DirStruct.html"))
    aux3 = insFld(aux2, gFld("GEOM::PointStruct", "", "structGEOM_1_1PointStruct.html"))
*/
         insDoc(aux1, gLnk("Class methods list", "", "functions.html"))
/*!
aux1 = insFld(foldersTree, gFld("Namespace List", "", "namespaces.html"))
       insDoc(aux1, gLnk("GEOM", "", "namespaceGEOM.html"))
*/
         insDoc(aux1, gLnk("Namespace Members", "", "namespacemembers.html"))

         insDoc(aux1, gLnk("File List", "", "files.html"))
      
/*!
      
       insDoc(aux1, gLnk("GEOM_Gen.idl", "", "GEOM__Gen_8idl.html"))
       insDoc(aux1, gLnk("GEOM_Shape.idl", "", "GEOM__Shape_8idl.html"))
*/

aux1 = insFld(foldersTree, gFld("IDL/Python mapping", ""))
         insDoc(aux1, gLnk("Mapping of GEOMETRY IDL definitions to Python language", "", "page2.html"))

/*!aux1 = insFld(foldersTree, gFld("Python Commands", "", "geompy_doc/main.html"))
         insDoc(aux1, gLnk("Package geompy", "", "geompy_doc/namespacegeompy.html"))
         insDoc(aux1, gLnk("Namespace Members", "", "geompy_doc/namespacemembers.html"))
         insDoc(aux1, gLnk("File geompy.py", "", "geompy_doc/geompy_8py.html"))
*/

aux1 = insFld(foldersTree, gFld("Python Commands", "", "geompy_doc/main.html"))
         insDoc(aux1, gLnk("Package List", "", "geompy_doc/namespaces.html"))
         insDoc(aux1, gLnk("Namespace Members", "", "geompy_doc/namespacemembers.html"))
         insDoc(aux1, gLnk("File List", "", "geompy_doc/files.html"))

/*!  insDoc(foldersTree, gLnk("Graphical Class Hierarchy", "", "inherits.html"))
*/
