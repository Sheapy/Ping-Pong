#ifndef __GameState_h_
#define __GameState_h_

#include <OISKeyboard.h>
#include <OISMouse.h>
#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>
#include <SdkTrays.h>
#include <SdkCameraMan.h>
#include <btBulletDynamicsCommon.h>
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h>
#include "GameObject.h"
#include "SoundManager.h"
#include <list>
#include <stdlib.h>
#include "GameNet.h" 



class GameState:OgreBites::SdkTrayListener{
	public:
		GameState(void);
		virtual void updatePaddle(const OIS::MouseEvent &arg);
		virtual void updateAIpaddle(void);
		virtual void stepSimulation(void);
		virtual void initialise(Ogre::SceneManager* m);
    bool rotateUp(void);
    bool rotateLeft(void);
    bool rotateRight(void);
    bool rotateDown(void);
		bool gameOver;
		bool multiplayer;
		int score;
		int pingPong;
		bool menuInput;
    GameObject* ball;




	protected:
	  int addScore(bool reset);
		Ogre::SceneManager* mSceneMgr;
	    btDiscreteDynamicsWorld* dynamicsWorld;
	    btRigidBody* fallRigidBody;
	    btRigidBody* groundRigidBody;
	    std::list<GameObject*> l;
	    GameObject* paddle;
	    
  	  GameObject* AIpaddle;
  	  OgreBites::SdkTrayManager* mTrayMgr;
  	  CEGUI::OgreRenderer* mRenderer;

  	typedef struct
  	{
  	  float bX;
  	  float bY;
  	  float bZ;
  	  float sX;
  	  float sY;
  	  float sZ;
  	  float cX;
  	  float cY;
  	  float cZ;

  	  template <typename Archive>
  	  void serialize(Archive& ar, const unsigned int version)
  	  {
  	    ar & bX;
  	    ar & bY;
  	    ar & bZ;
  	    ar & sX;
	  	ar & sY;
	  	ar & sZ;
	  	ar & cX;
	  	ar & cY;
	  	ar & cZ;
  	  }
  	} ServerStruct;

  	typedef struct
  	{
  	  float cX;
  	  float cY;
  	  float cZ;

  	  template <typename Archive>
  	  void serialize(Archive& ar, const unsigned int version)
  	  {
	  	ar & cX;
	  	ar & cY;
	  	ar & cZ;
  	  }
  	} ClientStruct;
};
#endif

