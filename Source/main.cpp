
//#include <OGRE/ExampleApplication.h>
//#include <OGRE/OgreLogManager.h>
#include "Actions/Action.h"
#include "Common/Common.h"
#include "Networking/Network.h"
#include <fstream>
#include <OGRE/OgreLogManager.h>
#include <OGRE/OgreRoot.h>
//#include <bullet/btBulletDynamicsCommon.h>
using namespace std;

/*class Example : public ExampleApplication {
public:
	
  void createScene()
  {
    Ogre::Entity* peng = mSceneMgr->createEntity("Peng", "penguin.mesh");
    Ogre::Entity* ogre = mSceneMgr->createEntity("Ogre", "ogrehead.mesh");
    
    mSceneMgr->getRootSceneNode()->attachObject(peng);
    Ogre::SceneNode* headNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    headNode->attachObject(ogre);
    
    // Set ambient light
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
    
    // Create a light
    Ogre::Light* l = mSceneMgr->createLight("MainLight");
    l->setPosition(20,80,50);
    

  }
  
private:
  
  
};*/

int main(int argc, char **argv)
{
  Network* x = Network::instance();
  x->connect("127.0.0.1");

  // This is necessary to have the networking branch off on its own thread.
  asio::thread t(boost::bind(&asio::io_service::run, &Network::service()));

  Action a;
  Network::instance()->send(a, UNKNOWN);
  Network::instance()->send(a, UNKNOWN);
  Network::instance()->send(a, UNKNOWN);



  /* 
  Action a(func);
  
  if (a.handle()) {
    return 0;
  }*/
  
  //Example app;
  //try {
  //  app.go();
  //} 
  //catch( Ogre::Exception& e ){
  //  std::cerr << "An exception has occured: " << e.getFullDescription().c_str() << std::endl;

  //}
  
  while (1) { }
  
  
  return 0;
}
