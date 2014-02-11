#include "GameObject.h"

GameObject::GameObject(void)
{
	
}

void GameObject::construct(Ogre::SceneNode* s, btRigidBody* r, int i){
  sNode=s;
  rBody=r;
  id=i;
  collisionCount=0;
  highscore=0;
  hostHit = false;
  clientHit = true;
  hostScore = 0;
  clientScore = 0;
  multiplayer = false;
  aiLevel=3;
  correctSide=true;
  dir=0;
}

void GameObject::sync(void){
  btTransform trans;
    rBody->getMotionState()->getWorldTransform(trans);
    sNode->setPosition(Ogre::Vector3(trans.getOrigin().getX(),trans.getOrigin().getY(),trans.getOrigin().getZ()));
}

int GameObject::updateCollisionCount(bool reset)
{
  if (reset) {
    if (collisionCount > highscore)
      highscore = collisionCount;
    collisionCount = 0;
  }
  else
    collisionCount++;
  return collisionCount;
}

void GameObject::setMP(bool mp) {
  multiplayer = mp;
}
  
