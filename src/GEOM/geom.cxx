using namespace std;
// File :      geom.cxx
// Created :   5 November 2001
// Author :    Lucien PIGNOLONI
// Project :   SALOME
// Copyright : OPEN CASCADE
// $Header$

#include "GEOM_Gen_i.hh"
#include "SALOME_NamingService.hxx"

//==================================================================================
// function : main()                      MAIN 
// purpose  :
//==================================================================================
int main(int argc, char** argv)
{
  try {
    // Create and initialise the ORB.
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv, "omniORB3");
    
    // Obtain a reference to the root POA.
    CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(obj);

    // We allocate the objects on the heap.  Since these are reference
    // counted objects, they will be deleted by the POA when they are no
    // longer needed.    
    GEOM_Gen_i * myGEOM_Gen_i = new GEOM_Gen_i(orb);

    // Activate the objects.  This tells the POA that the objects are
    // ready to accept requests.
    PortableServer::ObjectId_var myGEOM_Gen_iid = poa->activate_object(myGEOM_Gen_i);
    myGEOM_Gen_i->register_name("/myGEOM_Gen");
 
    // Obtain a POAManager, and tell the POA to start accepting
    // requests on its objects.
    PortableServer::POAManager_var pman = poa->the_POAManager();
    pman->activate();

    orb->run();
    orb->destroy();
  }
  catch(CORBA::SystemException&) {
    cerr << "Caught CORBA::SystemException." << endl;
  }
  catch(CORBA::Exception&) {
    cerr << "Caught CORBA::Exception." << endl;
  }
  catch(omniORB::fatalException& fe) {
    cerr << "Caught omniORB::fatalException:" << endl;
    cerr << "  file: " << fe.file() << endl;
    cerr << "  line: " << fe.line() << endl;
    cerr << "  msg: " << fe.errmsg() << endl;
  }
  catch(...) {
    cerr << "Caught unknown exception." << endl;
  }
  return 0;
}
