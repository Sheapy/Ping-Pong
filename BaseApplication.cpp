/*
 -----------------------------------------------------------------------------
 Filename:    BaseApplication.cpp
 -----------------------------------------------------------------------------

 This source file is part of the
 ___                 __    __ _ _    _ 
 /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
 / \_// (_| | | |  __/  \  /\  /| |   <| |
 \___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
 |___/                              
 Tutorial Framework
 http://www.ogre3d.org/tikiwiki/
 -----------------------------------------------------------------------------
 */
#include "BaseApplication.h"

//-------------------------------------------------------------------------------------
float xVel=1;
float yVel=.5;
float zVel=0;
bool firstRun=true;
bool freeCam=false;

BaseApplication::BaseApplication(void) :
	mRoot(0), mCamera(0), mSceneMgr(0), mWindow(0),
			mResourcesCfg(Ogre::StringUtil::BLANK),
			mPluginsCfg(Ogre::StringUtil::BLANK), mTrayMgr(0), mCameraMan(0),
			mDetailsPanel(0), mCursorWasVisible(false), mShutDown(false),
			mInputManager(0), mMouse(0), mKeyboard(0), modes(0), ip(""),gs(0) {
}

//-------------------------------------------------------------------------------------
BaseApplication::~BaseApplication(void) {
	if (mTrayMgr)
		delete mTrayMgr;
	if (mCameraMan)
		delete mCameraMan;

	//Remove ourself as a Window listener
	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
	windowClosed(mWindow);
	delete mRoot;
}

//-------------------------------------------------------------------------------------
bool BaseApplication::configure(void) {
	// Show the configuration dialog and initialise the system
	// You can skip this and use root.restoreConfig() to load configuration
	// settings if you were sure there are valid ones saved in ogre.cfg
	if (mRoot->showConfigDialog()) {
		// If returned true, user clicked OK so initialise
		// Here we choose to let the system create a default rendering window by passing 'true'
		mWindow = mRoot->initialise(true, "TutorialApplication Render Window");

		return true;
	} else {
		return false;
	}
}
//-------------------------------------------------------------------------------------
void BaseApplication::chooseSceneManager(void) {
	// Get the SceneManager, in this case a generic one
	mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
}
//-------------------------------------------------------------------------------------
void BaseApplication::createCamera(void) {
	// Create the camera
	mCamera = mSceneMgr->createCamera("PlayerCam");

	// Position it at 500 in Z direction
	mCamera->setPosition(Ogre::Vector3(0, 0, 90));
	// Look back along -Z
	mCamera->lookAt(Ogre::Vector3(0, 0, -300));
	mCamera->setNearClipDistance(5);

	mCameraMan = new OgreBites::SdkCameraMan(mCamera); // create a default camera controller
}
//-------------------------------------------------------------------------------------
void BaseApplication::createFrameListener(void) {
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	mWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	mInputManager = OIS::InputManager::createInputSystem(pl);

	mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(
			OIS::OISKeyboard, true));
	mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(
			OIS::OISMouse, true));

	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);

	//Set initial mouse clipping size
	windowResized(mWindow);

	//Register as a Window listener
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
	mTrayMgr = new OgreBites::SdkTrayManager("InterfaceName", mWindow, mMouse, this);
	mTrayMgr->showCursor();
	Ogre::FontManager::getSingleton().getByName("SdkTrays/Caption")->load();
	mTrayMgr->createLabel(OgreBites::TL_CENTER, "mode", "Mode Selection", 250);
	mTrayMgr->createButton(OgreBites::TL_CENTER, "single", "Single Player", 250);
	mTrayMgr->createButton(OgreBites::TL_CENTER, "server", "Server mode", 250);
	mTrayMgr->createButton(OgreBites::TL_CENTER, "client", "Client mode", 250);
	mTrayMgr->showCursor();

	mRoot->addFrameListener(this);
}
//-------------------------------------------------------------------------------------
void BaseApplication::destroyScene(void) {
}
//-------------------------------------------------------------------------------------
void BaseApplication::createViewports(void) {
	// Create one viewport, entire window
	Ogre::Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth())
			/ Ogre::Real(vp->getActualHeight()));
}
//-------------------------------------------------------------------------------------
void BaseApplication::setupResources(void) {
	// Load resource paths from config file
	Ogre::ConfigFile cf;
	cf.load(mResourcesCfg);

	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

	Ogre::String secName, typeName, archName;
	while (seci.hasMoreElements()) {
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i) {
			typeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
		}
	}
}
//-------------------------------------------------------------------------------------
void BaseApplication::createResourceListener(void) {

}
//-------------------------------------------------------------------------------------
void BaseApplication::loadResources(void) {
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
//-------------------------------------------------------------------------------------
void BaseApplication::go(void) {
#ifdef _DEBUG
	mResourcesCfg = "resources_d.cfg";
	mPluginsCfg = "plugins_d.cfg";
#else
	mResourcesCfg = "resources.cfg";
	mPluginsCfg = "plugins.cfg";
#endif

	if (!setup())
		return;

	mRoot->startRendering();

	// clean up
	destroyScene();
}
//-------------------------------------------------------------------------------------
bool BaseApplication::setup(void) {
	mRoot = new Ogre::Root(mPluginsCfg);

	setupResources();

	bool carryOn = configure();
	if (!carryOn)
		return false;

	chooseSceneManager();
	createCamera();
	createViewports();

	// Set default mipmap level (NB some APIs ignore this)
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

	// Create any resource listeners (for loading screens)
	createResourceListener();
	// Load resources
	loadResources();

	// Create the scene

	createScene();

	createFrameListener();

	return true;
}
;

void BaseApplication::createScene(void) {

	//	gs = new GameState();
	//	gs->initialise(mSceneMgr);
	//	
	//	mCamera->setPosition(Ogre::Vector3(0,100,-300));
	//	mCamera->lookAt(Ogre::Vector3(0,0,0));
	//
	//    Ogre::Light* light = mSceneMgr->createLight("Light1");
	//	light->setPosition(90.0f, 90.0f,90.0f);
}

//-------------------------------------------------------------------------------------
bool BaseApplication::frameRenderingQueued(const Ogre::FrameEvent& evt) {

	if (mWindow->isClosed())
		return false;

	if (mShutDown)
		return false;

	//Need to capture/update each device
	mKeyboard->capture();
	mMouse->capture();
	if (left == 1) {
		gs->rotateLeft();
	} else if (right == 1) {
		gs->rotateRight();
	} else if (up == 1) {
		gs->rotateUp();
	} else if (down == 1) {
		gs->rotateDown();
	}
	mTrayMgr->frameRenderingQueued(evt);
	mTrayMgr->refreshCursor();
	if(gs!=0)
		gs->stepSimulation();

	return true;
}
//-------------------------------------------------------------------------------------
bool BaseApplication::keyPressed(const OIS::KeyEvent &arg) {
	if (modes==1) {
		OgreBites::TextBox* l =
				(OgreBites::TextBox*)(mTrayMgr->getWidget("inp"));
		char temp = (char)arg.text;
		if (arg.key==OIS::KC_BACK) {
			ip = ip.substr(0, ip.size()-1);
		} else if(!isspace(temp)){
			ip=ip+temp;
		}
		l->setCaption(ip);
		if (arg.key == OIS::KC_ESCAPE) {
				mShutDown = true;
		}

		return true;
	}

	if (arg.key == OIS::KC_W) {
		up = 1;
	} else if (arg.key == OIS::KC_A) {

		left = 1;
	} else if (arg.key == OIS::KC_S) {
		down = 1;
	} else if (arg.key == OIS::KC_D) {
		right = 1;
	}

	if (arg.key == OIS::KC_K) {
		SoundManager::SoundControl.OnCleanup();
		Mix_CloseAudio();
	} else if (arg.key == OIS::KC_L) {
		Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	} else if (arg.key == OIS::KC_ESCAPE) {
		mShutDown = true;
	}

	mCameraMan->injectKeyDown(arg);
	return true;
}

CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID) {
	switch (buttonID) {
	case OIS::MB_Left:
		return CEGUI::LeftButton;

	case OIS::MB_Right:
		return CEGUI::RightButton;

	case OIS::MB_Middle:
		return CEGUI::MiddleButton;

	default:
		return CEGUI::LeftButton;
	}
}

bool BaseApplication::keyReleased(const OIS::KeyEvent &arg) {
	mCameraMan->injectKeyUp(arg);
	if (arg.key == OIS::KC_W) {
		up = 0;
	} else if (arg.key == OIS::KC_A) {
		left = 0;
	} else if (arg.key == OIS::KC_S) {
		down = 0;
	} else if (arg.key == OIS::KC_D) {
		right = 0;
	}
	return true;
}

bool BaseApplication::mouseMoved(const OIS::MouseEvent &arg) {
	//mCameraMan->injectMouseMove(arg);
	if(gs!=0)
		gs->updatePaddle(arg);
	return true;
}

bool BaseApplication::mousePressed(const OIS::MouseEvent &arg,
		OIS::MouseButtonID id) {
	if(gs!=0)
		gs->updatePaddle(arg);

	if (mTrayMgr->injectMouseDown(arg, id))
		return true;
	mCameraMan->injectMouseDown(arg, id);
	return true;
}

bool BaseApplication::mouseReleased(const OIS::MouseEvent &arg,
		OIS::MouseButtonID id) {

	if(gs!=0)
		gs->updatePaddle(arg);

	if (mTrayMgr->injectMouseUp(arg, id))
		return true;
	mCameraMan->injectMouseUp(arg, id);
	return true;
}

//Adjust mouse clipping area
void BaseApplication::windowResized(Ogre::RenderWindow* rw) {
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

void BaseApplication::buttonHit(OgreBites::Button* b) {
	if (b->getName().compare("client")==0) {
		showClientMenu();
	}
	else if(b->getName().compare("connect")==0){
		gs = new ClientGameState();
		ClientGameState* cgs =(ClientGameState *)gs;
		cgs->initialise(mSceneMgr,ip);
		mCamera->setPosition(Ogre::Vector3(0,100,300));
		mCamera->lookAt(Ogre::Vector3(0,0,0));

	    Ogre::Light* light = mSceneMgr->createLight("Light1");
		light->setPosition(90.0f, 90.0f,90.0f);
		mTrayMgr->destroyWidget("inst");
		mTrayMgr->destroyWidget("inp");
		mTrayMgr->destroyWidget("connect");
		mTrayMgr->hideCursor();
		modes=0;
	}
	else if(b->getName().compare("single")==0){
		mTrayMgr->destroyWidget("server");
		mTrayMgr->destroyWidget("client");
		mTrayMgr->destroyWidget("mode");
		mTrayMgr->destroyWidget("single");
		
		mTrayMgr->createLabel(OgreBites::TL_CENTER, "diff", "Select Difficulty", 250);
		mTrayMgr->createButton(OgreBites::TL_CENTER, "d1", "Normal", 250);
		mTrayMgr->createButton(OgreBites::TL_CENTER, "d2", "Hard", 250);
		mTrayMgr->createButton(OgreBites::TL_CENTER, "d3", "Impossible", 250);
		mTrayMgr->createButton(OgreBites::TL_CENTER, "d4", "QWOP", 250);

	}
	else if(b->getName().compare("server")==0){

		mTrayMgr->hideCursor();
		mTrayMgr->destroyWidget("server");
		mTrayMgr->destroyWidget("client");
		mTrayMgr->destroyWidget("mode");
		mTrayMgr->destroyWidget("single");
		mTrayMgr->createButton(OgreBites::TL_CENTER, "status", "Waiting for connection", 250);
		gs = new ServerGameState();
				ServerGameState* sgs = (ServerGameState*)gs;
				sgs->initialise(mSceneMgr,mTrayMgr);
				mCamera->setPosition(Ogre::Vector3(0,100,-300));
				mCamera->lookAt(Ogre::Vector3(0,0,0));

			    Ogre::Light* light = mSceneMgr->createLight("Light1");
				light->setPosition(90.0f, 90.0f,90.0f);
	}
	else if(b->getName().compare("d1")==0){
		gs = new GameState();
		gs->initialise(mSceneMgr);
		mCamera->setPosition(Ogre::Vector3(0,100,-300));
		mCamera->lookAt(Ogre::Vector3(0,0,0));

	    Ogre::Light* light = mSceneMgr->createLight("Light1");
		light->setPosition(90.0f, 90.0f,90.0f);
		mTrayMgr->hideCursor();
		gs->ball->aiLevel=0;
		mTrayMgr->destroyWidget("diff");
		mTrayMgr->destroyWidget("d1");
		mTrayMgr->destroyWidget("d2");
		mTrayMgr->destroyWidget("d3");		
		mTrayMgr->destroyWidget("d4");		
	}
	else if(b->getName().compare("d2")==0){
		gs = new GameState();
		gs->initialise(mSceneMgr);
		mCamera->setPosition(Ogre::Vector3(0,100,-300));
		mCamera->lookAt(Ogre::Vector3(0,0,0));

	    Ogre::Light* light = mSceneMgr->createLight("Light1");
		light->setPosition(90.0f, 90.0f,90.0f);
		mTrayMgr->hideCursor();
		gs->ball->aiLevel=1;
		mTrayMgr->destroyWidget("diff");
		mTrayMgr->destroyWidget("d1");
		mTrayMgr->destroyWidget("d2");
		mTrayMgr->destroyWidget("d3");		
		mTrayMgr->destroyWidget("d4");		
	}
	else if(b->getName().compare("d3")==0){
		gs = new GameState();
		gs->initialise(mSceneMgr);
		mCamera->setPosition(Ogre::Vector3(0,100,-300));
		mCamera->lookAt(Ogre::Vector3(0,0,0));

	    Ogre::Light* light = mSceneMgr->createLight("Light1");
		light->setPosition(90.0f, 90.0f,90.0f);
		mTrayMgr->hideCursor();
		gs->ball->aiLevel=2;
		mTrayMgr->destroyWidget("diff");
		mTrayMgr->destroyWidget("d1");
		mTrayMgr->destroyWidget("d2");
		mTrayMgr->destroyWidget("d3");		
		mTrayMgr->destroyWidget("d4");		
	}
	else if(b->getName().compare("d4")==0){
		gs = new GameState();
		gs->initialise(mSceneMgr);
		mCamera->setPosition(Ogre::Vector3(0,100,-300));
		mCamera->lookAt(Ogre::Vector3(0,0,0));

	    Ogre::Light* light = mSceneMgr->createLight("Light1");
		light->setPosition(90.0f, 90.0f,90.0f);
		mTrayMgr->hideCursor();
		gs->ball->aiLevel=3;
		mTrayMgr->destroyWidget("diff");
		mTrayMgr->destroyWidget("d1");
		mTrayMgr->destroyWidget("d2");
		mTrayMgr->destroyWidget("d3");		
		mTrayMgr->destroyWidget("d4");		
	}			
}

void BaseApplication::showClientMenu(void) {
	mTrayMgr->destroyWidget("server");
	mTrayMgr->destroyWidget("client");
	mTrayMgr->destroyWidget("mode");
	mTrayMgr->destroyWidget("single");

	mTrayMgr->createLabel(OgreBites::TL_CENTER, "inst", "Enter IP", 250);
	OgreBites::TextBox* t = mTrayMgr->createTextBox(OgreBites::TL_CENTER,
			"inp", "", 250, 30);
	t->setCaption("");
	mTrayMgr->createButton(OgreBites::TL_CENTER, "connect", "Connect", 250);
	modes=1;
}

//Unattach OIS before window shutdown (very important under Linux)
void BaseApplication::windowClosed(Ogre::RenderWindow* rw) {
	//Only close for window that created OIS (the main window in these demos)
	if (rw == mWindow) {
		if (mInputManager) {
			mInputManager->destroyInputObject(mMouse);
			mInputManager->destroyInputObject(mKeyboard);

			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = 0;
		}
	}
}
