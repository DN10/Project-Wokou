
//#include <OGRE/ExampleApplication.h>
//#include <OGRE/OgreLogManager.h>
#include "Actions/Action.h"
#include "Actions/ActionPump.h"
#include "Application/Application.h"
#include "Common/Common.h"
#include "Input/Input.h"
#include "Networking/Network.h"
#include "Graphics/Graphics.h"

#include <iostream>
#include <fstream>
#include <OGRE/ExampleApplication.h>
#include <OGRE/ExampleFrameListener.h>

using namespace std;

int main(int argc, char **argv)
{
  /*Network* x = Network::instance();
  x->connect("127.0.0.1");

  // This is necessary to have the networking branch off on its own thread.
  asio::thread t(boost::bind(&asio::io_service::run, &Network::service()));

  Action a;
  a["id"] = 4;
  a["target"] = 300;
  Network::instance()->send(a, UNKNOWN);

  t.join();*/
  
  //Example app;
  Application app2;
  try {
    //app.go();
    app2.go();
  } 
  catch( Ogre::Exception& e ) {
    std::cerr << "An exception has occured: " << e.getFullDescription().c_str() << std::endl;
  }

  
  return 0;
}
