/* Copyright (c) 2011 Cody Miller, Daniel Norris, Brett Hitchcock.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *****************************************************************************/

#include "Input.h"

//This is the only function that is Protected, not Public.
void Input::updateStats(){
  static String currFps = "Current FPS: ";
  static String avgFps = "Average FPS: ";
  static String bestFps = "Best FPS: ";
  static String worstFps = "Worst FPS: ";
  static String tris = "Triangle Count: ";
  static String batches = "Batch Count: ";

  // update stats when necessary
  try {
    OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");
    OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("Core/CurrFps");
    OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("Core/BestFps");
    OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("Core/WorstFps");

    const RenderTarget::FrameStats& stats = mWindow->getStatistics();
    guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
    guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
    guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS)
			+" "+StringConverter::toString(stats.bestFrameTime)+" ms");
    guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS)
			 +" "+StringConverter::toString(stats.worstFrameTime)+" ms");

    OverlayElement* guiTris = OverlayManager::getSingleton().getOverlayElement("Core/NumTris");
    guiTris->setCaption(tris + StringConverter::toString(stats.triangleCount));

    OverlayElement* guiBatches = OverlayManager::getSingleton().getOverlayElement("Core/NumBatches");
    guiBatches->setCaption(batches + StringConverter::toString(stats.batchCount));

    OverlayElement* guiDbg = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
    guiDbg->setCaption(mDebugText);
  }
  catch(...) { /* ignore */ }
}

Input::Input( RenderWindow* win, bool bufferedKeys, bool bufferedMouse, bool bufferedJoy)
//: mCamera (Graphics::instance()->camera_)
{
  std::cout << "INITIALIZING THE INPUT OBJECT" << std::endl;
  mTranslateVector     = Vector3::ZERO;
  mCurrentSpeed        = 0;
  mWindow              = win;
  mStatsOn             = true;
  mNumScreenShots      = 0;
  mMoveScale           = 0.0f; 
  mRotScale            = 0.0f;
  mTimeUntilNextToggle = 0;
  mFiltering           = TFO_BILINEAR;
  mAniso               = 1;
  mSceneDetailIndex    = 0;
  mMoveSpeed           = 100;
  mRotateSpeed         = 36;
  mDebugOverlay        = 0;
  mInputManager        = 0;
  mMouse               = 0;
  mKeyboard            = 0;
  mJoy                 = 0;
    
  mDebugOverlay = OverlayManager::getSingleton().getByName("Core/DebugOverlay");

  LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
  OIS::ParamList pl;
  size_t windowHnd = 0;
  std::ostringstream windowHndStr;

  win->getCustomAttribute("WINDOW", &windowHnd);
  windowHndStr << windowHnd;
  pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

  mInputManager = OIS::InputManager::createInputSystem( pl );

  //Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
  mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, bufferedKeys ));
  mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, bufferedMouse ));
  try {
    mJoy = static_cast<OIS::JoyStick*>(mInputManager->createInputObject( OIS::OISJoyStick, bufferedJoy ));
  }
  catch(...) {
    mJoy = 0;
  }

  //Set initial mouse clipping size
  windowResized(mWindow);

  showDebugOverlay(true);

  //Register as a Window listener
  WindowEventUtilities::addWindowEventListener(mWindow, this);
}

Input::~Input(){
  //Remove ourself as a Window listener
  WindowEventUtilities::removeWindowEventListener(mWindow, this);
  windowClosed(mWindow);
}

void Input::windowResized( RenderWindow* rw ){
  unsigned int width, height, depth;
  int left, top;
  rw->getMetrics(width, height, depth, left, top);

  const OIS::MouseState &ms = mMouse->getMouseState();
  ms.width = width;
  ms.height = height;
}

void Input::windowClosed( RenderWindow* rw ){
  //Only close for window that created OIS (the main window in these demos)
  if( rw == mWindow ){
    if( mInputManager ){
      mInputManager->destroyInputObject( mMouse );
      mInputManager->destroyInputObject( mKeyboard );
      mInputManager->destroyInputObject( mJoy );

      OIS::InputManager::destroyInputSystem(mInputManager);
      mInputManager = 0;
    }
  }
}

//CODY. THIS BEAST IS YOURS TO TAME.
bool Input::processUnbufferedKeyInput( const FrameEvent& evt ){
  std::cout << "PROCESSING UNBUFFEREDKEYINPUT" << std::endl;
  
  Real moveScale = mMoveScale;
  if(mKeyboard->isKeyDown(OIS::KC_LSHIFT))
    moveScale *= 10;
  if(mKeyboard->isKeyDown(OIS::KC_A))
    mTranslateVector.x = -moveScale;	// Move camera left
  if(mKeyboard->isKeyDown(OIS::KC_D))
    mTranslateVector.x = moveScale;	// Move camera RIGHT
  if(mKeyboard->isKeyDown(OIS::KC_UP) || mKeyboard->isKeyDown(OIS::KC_W) )
    mTranslateVector.z = -moveScale;	// Move camera forward
  if(mKeyboard->isKeyDown(OIS::KC_DOWN) || mKeyboard->isKeyDown(OIS::KC_S) )
    mTranslateVector.z = moveScale;	// Move camera backward
  if(mKeyboard->isKeyDown(OIS::KC_PGUP))
    mTranslateVector.y = moveScale;	// Move camera up
  if(mKeyboard->isKeyDown(OIS::KC_PGDOWN))
    mTranslateVector.y = -moveScale;	// Move camera down
  //if(mKeyboard->isKeyDown(OIS::KC_RIGHT))
    //mCamera->yaw(-mRotScale);
  //if(mKeyboard->isKeyDown(OIS::KC_LEFT))
    //mCamera->yaw(mRotScale);
  if( mKeyboard->isKeyDown(OIS::KC_ESCAPE) || mKeyboard->isKeyDown(OIS::KC_Q) )
    return false;
  if( mKeyboard->isKeyDown(OIS::KC_F) && mTimeUntilNextToggle <= 0 ){
    mStatsOn = !mStatsOn;
    showDebugOverlay(mStatsOn);
    mTimeUntilNextToggle = 1;
  }
  if( mKeyboard->isKeyDown(OIS::KC_T) && mTimeUntilNextToggle <= 0 ){
    switch(mFiltering){
    case TFO_BILINEAR:
      mFiltering = TFO_TRILINEAR;
      mAniso = 1;
      break;
    case TFO_TRILINEAR:
      mFiltering = TFO_ANISOTROPIC;
      mAniso = 8;
      break;
    case TFO_ANISOTROPIC:
      mFiltering = TFO_BILINEAR;
      mAniso = 1;
      break;
    default: break;
    }
    MaterialManager::getSingleton().setDefaultTextureFiltering(mFiltering);
    MaterialManager::getSingleton().setDefaultAnisotropy(mAniso);
    showDebugOverlay(mStatsOn);
    mTimeUntilNextToggle = 1;
  }
  if(mKeyboard->isKeyDown(OIS::KC_SYSRQ) && mTimeUntilNextToggle <= 0){
    std::ostringstream ss;
    ss << "screenshot_" << ++mNumScreenShots << ".png";
    mWindow->writeContentsToFile(ss.str());
    mTimeUntilNextToggle = 0.5;
    mDebugText = "Saved: " + ss.str();
  }
  /*if(mKeyboard->isKeyDown(OIS::KC_R) && mTimeUntilNextToggle <=0){
    mSceneDetailIndex = (mSceneDetailIndex+1)%3 ;
    switch(mSceneDetailIndex) {
    case 0 : mCamera->setPolygonMode(PM_SOLID); break;
    case 1 : mCamera->setPolygonMode(PM_WIREFRAME); break;
    case 2 : mCamera->setPolygonMode(PM_POINTS); break;
    }
    mTimeUntilNextToggle = 0.5;
  }*/
  static bool displayCameraDetails = false;
  if(mKeyboard->isKeyDown(OIS::KC_P) && mTimeUntilNextToggle <= 0){
    displayCameraDetails = !displayCameraDetails;
    mTimeUntilNextToggle = 0.5;
    if (!displayCameraDetails)
      mDebugText = "";
  }
  // Print camera details
  /*if(displayCameraDetails)
    mDebugText = "P: " + StringConverter::toString(mCamera->getDerivedPosition()) +
      " " + "O: " + StringConverter::toString(mCamera->getDerivedOrientation());*/
  // Return true to continue rendering
  return true;
}

bool Input::processUnbufferedMouseInput( const FrameEvent& evt ){
  // Rotation factors, may not be used if the second mouse button is pressed
  // 2nd mouse button - slide, otherwise rotate
  const OIS::MouseState &ms = mMouse->getMouseState();
  if( ms.buttonDown( OIS::MB_Right ) ){
    mTranslateVector.x += ms.X.rel * 0.13;
    mTranslateVector.y -= ms.Y.rel * 0.13;
  }
  else{
    mRotX = Degree(-ms.X.rel * 0.13);
    mRotY = Degree(-ms.Y.rel * 0.13);
  }
  return true;
}

void Input::moveCamera()
{
  Graphics::instance()->moveCamera(mTranslateVector.x, mTranslateVector.y, mTranslateVector.z);
//  Graphics::instance()->moveCamera(_, _, _);
//  Graphics::instance()->rotateCamera(_, _);



  // Make all the changes to the camera
  // Note that YAW direction is around a fixed axis (freelook style) rather than a natural YAW
  //(e.g. airplane)
  /*mCamera->yaw(mRotX);
  mCamera->pitch(mRotY);
  mCamera->moveRelative(mTranslateVector);*/
}

void Input::showDebugOverlay( bool show ){
  if (mDebugOverlay){
    if (show)
      mDebugOverlay->show();
    else
      mDebugOverlay->hide();
  }
}

bool Input::frameRenderingQueued( const FrameEvent& evt ){
  if(mWindow->isClosed())
    return false;
  
  mSpeedLimit = mMoveScale * evt.timeSinceLastFrame;
  
  //Need to capture/update each device
  mKeyboard->capture();
  mMouse->capture();
  if( mJoy ) mJoy->capture();

  bool buffJ = (mJoy) ? mJoy->buffered() : true;

  Ogre::Vector3 lastMotion = mTranslateVector;

  //Check if one of the devices is not buffered
  if( !mMouse->buffered() || !mKeyboard->buffered() || !buffJ ){
    // one of the input modes is immediate, so setup what is needed for immediate movement
    if (mTimeUntilNextToggle >= 0)
      mTimeUntilNextToggle -= evt.timeSinceLastFrame;
    
    // Move about 100 units per second
    mMoveScale = mMoveSpeed * evt.timeSinceLastFrame;
    // Take about 10 seconds for full rotation
    mRotScale = mRotateSpeed * evt.timeSinceLastFrame;
    
    mRotX = 0;
    mRotY = 0;
    mTranslateVector = Ogre::Vector3::ZERO;  
  }
  //Check to see which device is not buffered, and handle it
#if OGRE_PLATFORM != OGRE_PLATFORM_IPHONE
  if( !mKeyboard->buffered() )
    if( processUnbufferedKeyInput(evt) == false )
      return false;
  
#ifdef USE_RTSHADER_SYSTEM
  processShaderGeneratorInput();
#endif
  
#endif
  if( !mMouse->buffered() )
    if( processUnbufferedMouseInput(evt) == false )
      return false;
  
  // ramp up / ramp down speed
  if (mTranslateVector == Ogre::Vector3::ZERO){
    // decay (one third speed)
    mCurrentSpeed -= evt.timeSinceLastFrame * 0.3;
    mTranslateVector = lastMotion;
  }
  else{
    // ramp up
    mCurrentSpeed += evt.timeSinceLastFrame;
  }
  // Limit motion speed
  if (mCurrentSpeed > 1.0)
    mCurrentSpeed = 1.0;
  if (mCurrentSpeed < 0.0)
    mCurrentSpeed = 0.0;
  
  mTranslateVector *= mCurrentSpeed;
  
  if( !mMouse->buffered() || !mKeyboard->buffered() || !buffJ )
    moveCamera();
  
  return true;
}

bool Input::frameEnded(const FrameEvent& evt){
  updateStats();
  return true;
}




//!*********************************HERE BE DRAGONS****************************************
//!****************************************************************************************
//!****************************************************************************************
//!****************************************************************************************
#ifdef USE_RTSHADER_SYSTEM
void Input::processShaderGeneratorInput(){
  // Switch to default scheme.
  if (mKeyboard->isKeyDown(OIS::KC_F2)){	
    mCamera->getViewport()->setMaterialScheme(MaterialManager::DEFAULT_SCHEME_NAME);			
    mDebugText = "Active Viewport Scheme: ";
    mDebugText += MaterialManager::DEFAULT_SCHEME_NAME;						
  }
  // Switch to shader generator scheme.
  if (mKeyboard->isKeyDown(OIS::KC_F3)){
    mCamera->getViewport()->setMaterialScheme(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
    mDebugText = "Active Viewport Scheme: ";
    mDebugText += RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME;
  }	
  // Toggles per pixel per light model.
  if (mKeyboard->isKeyDown(OIS::KC_F4) && mTimeUntilNextToggle <= 0){	
    mTimeUntilNextToggle = 1.0;
    static bool userPerPixelLightModel = true;
    RTShader::ShaderGenerator* shaderGenerator = RTShader::ShaderGenerator::getSingletonPtr();		    
    RTShader::RenderState* renderState = shaderGenerator->getRenderState(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
    // Remove all global sub render states.
    renderState->reset();
    // Add per pixel lighting sub render state to the global scheme render state.
    // It will override the default FFP lighting sub render state.
    if (userPerPixelLightModel){
      RTShader::SubRenderState* perPixelLightModel = shaderGenerator->createSubRenderState(RTShader::PerPixelLighting::Type);
      renderState->addSubRenderState(perPixelLightModel);
      mDebugText = "Per pixel lighting model applied to shader generator default scheme";
    }
    else{
      mDebugText = "Per vertex lighting model applied to shader generator default scheme";
    }
    // Invalidate the scheme in order to re-generate all shaders based technique related to this scheme.
    shaderGenerator->invalidateScheme(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
    userPerPixelLightModel = !userPerPixelLightModel;
  }	
}
#endif
