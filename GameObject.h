#ifndef __GameObject_h_
#define __GameObject_h_


#include <btBulletDynamicsCommon.h>
#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>

class GameObject{
	public:
		GameObject(void);
		Ogre::SceneNode* sNode;
	    btRigidBody* rBody;
	    void construct(Ogre::SceneNode* s, btRigidBody* r,int i);
	    void sync(void);
	    void setMP(bool mp);
	    int updateCollisionCount(bool reset);
	    int id;
	    int collisionCount;
	    int highscore;
	    bool hostHit;
		bool clientHit;
		int hostScore;
		int clientScore;
		bool multiplayer;
		int aiLevel;
		bool correctSide;
		int dir;
};
#endif



