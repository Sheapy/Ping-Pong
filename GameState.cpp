#include "GameState.h"

GameState::GameState(void) :
	mSceneMgr(0), score(0), gameOver(false), menuInput(false), mTrayMgr(0) {
}

bool callbackFunc(btManifoldPoint& cp, const btCollisionObject* obj1, int id1,
		int index1, const btCollisionObject* obj2, int id2, int index2) {

	GameObject* o = (GameObject*)obj1->getUserPointer();
	GameObject* o2= (GameObject*)obj2->getUserPointer();

	// Loading sounds
	int scoreReset;
	int pingPong;
	pingPong = SoundManager::SoundControl.OnLoad("media/audio/pingpong.wav");
	scoreReset
			= SoundManager::SoundControl.OnLoad("media/audio/scoreReset.wav");
	SoundManager::SoundControl.Play(pingPong);

	if (o2->id<o->id) {
		GameObject* temp = o;
		o=o2;
		o2=temp;
	}

	// Paddle and Ball
	if ((o->id==2 && o2->id==5) || (o2->multiplayer == true && ( o->id == 1 && o2->id==5))) {
		srand(time(NULL));
		btTransform bTrans;
		o2->rBody->getMotionState()->getWorldTransform(bTrans);
		btVector3 vec = btVector3(rand() % 100 - 50, rand() % 20 + 60, (-1 + 2
				* (o->id % 2)) * 90) - btVector3(bTrans.getOrigin().getX() , bTrans.getOrigin().getY() , bTrans.getOrigin().getZ());
		o2->rBody->setLinearVelocity(vec.normalize()*200); 
		o2->updateCollisionCount(false);
		o2->clientHit = true;
		o2->hostHit = false;
		o2->correctSide = false;

	}

	// Paddle and Ball
	if (o->id==1 && o2->id==5) {
		if (o2->aiLevel != 3) {
			srand(time(NULL));
			btTransform bTrans;
			o2->rBody->getMotionState()->getWorldTransform(bTrans);
			btVector3 vec = btVector3(rand() % 100 - 50, rand() % 20 + 60, (-1 + 2
				* (o->id % 2)) * 90) - btVector3(bTrans.getOrigin().getX() , bTrans.getOrigin().getY() , bTrans.getOrigin().getZ());			
			if(o2->dir==2){
				vec = btVector3(rand() % 50 - 50, rand() % 20 + 60, (-1 + 2
				* (o->id % 2)) * 90) - btVector3(bTrans.getOrigin().getX() , bTrans.getOrigin().getY() , bTrans.getOrigin().getZ());
			}
			if(o2->dir==1){
				vec = btVector3(rand() % 50 , rand() % 20 + 60, (-1 + 2
				* (o->id % 2)) * 90) - btVector3(bTrans.getOrigin().getX() , bTrans.getOrigin().getY() , bTrans.getOrigin().getZ());
			}			
			o2->rBody->setLinearVelocity(vec.normalize()*200); 
		}

		o2->updateCollisionCount(false);
		o2->hostHit = true;
		o2->clientHit = false;
		o2->correctSide = false;
	}

	// Floor and Ball
	if (o->id==-1 && o2->id==5) {
		Ogre::Vector3 pos;
		btTransform trans;
		trans.setIdentity();
		trans.setOrigin(btVector3(70, 40, 120));
		o2->rBody->setWorldTransform(trans);
		o2->rBody->setLinearVelocity(btVector3(-50, 0, -200));
		o2->updateCollisionCount(true);
		if ((o2->hostHit == true && o2->correctSide == true) || (o2->clientHit == true && o2->correctSide == false)) {
	  		o2->hostScore +=1;
	  		Ogre::String hostStr = Ogre::StringConverter::toString(o2->hostScore);
	  		Ogre::String clientStr = Ogre::StringConverter::toString(o2->clientScore);
	  		CEGUI::WindowManager::getSingleton().getWindow("CEGUIDemo/QuitButton")->setText("Player 1 : " + hostStr + "\n" + "Player 2 : " + clientStr);
	  		o2->hostHit = false;
	  		o2->clientHit = true;
	  	} else if ((o2->clientHit == true && o2->correctSide == true) || (o2->hostHit == true && o2->correctSide == false)) {
	  		o2->clientScore +=1;
	  		Ogre::String hostStr = Ogre::StringConverter::toString(o2->hostScore);
	  		Ogre::String clientStr = Ogre::StringConverter::toString(o2->clientScore);
	  		CEGUI::WindowManager::getSingleton().getWindow("CEGUIDemo/QuitButton")->setText("Player 1 : " + hostStr + "\n" + "Player 2 : " + clientStr);
	  		o2->hostHit = false;
	  		o2->clientHit = true;
	  	}

		SoundManager::SoundControl.Play(scoreReset);
	}

	// Net and Ball
	if (o->id==4 && o2->id==5) {
		Ogre::Vector3 pos;
		btTransform trans;
		trans.setIdentity();
		trans.setOrigin(btVector3(70, 40, 120));
		o2->rBody->setWorldTransform(trans);
		o2->rBody->setLinearVelocity(btVector3(-50, 0, -200));
		o2->updateCollisionCount(true);
		if (o2->hostHit == true) {
	  		o2->clientScore +=1;
	  		Ogre::String hostStr = Ogre::StringConverter::toString(o2->hostScore);
	  		Ogre::String clientStr = Ogre::StringConverter::toString(o2->clientScore);
	  		CEGUI::WindowManager::getSingleton().getWindow("CEGUIDemo/QuitButton")->setText("Player 1 : " + hostStr + "\n" + "Player 2 : " + clientStr);
	  		o2->hostHit = false;
	  		o2->clientHit = true;
	  }
	  else {
	  		o2->hostScore +=1;
	  		Ogre::String hostStr = Ogre::StringConverter::toString(o2->hostScore);
	  		Ogre::String clientStr = Ogre::StringConverter::toString(o2->clientScore);
	  		CEGUI::WindowManager::getSingleton().getWindow("CEGUIDemo/QuitButton")->setText("Player 1 : " + hostStr + "\n" + "Player 2 : " + clientStr);
	  		o2->hostHit = false;
	  		o2->clientHit = true;
	  }
		SoundManager::SoundControl.Play(scoreReset);

	}

	// Table and ball
	if (o->id==0 && o2->id==5) {
		Ogre::Vector3 pos;
		btTransform trans;
		/* std::cout<<"Ball position: " << o2->rBody->getCenterOfMassPosition().getZ() << "\n";
		std::cout<<"Host Hit: " << o2->hostHit << "\n";
		std::cout<<"Client Hit: " << o2->clientHit << "\n"; */
		if ((o2->rBody->getCenterOfMassPosition().getZ() > 0) && o2->clientHit == true) {
			o2->hostScore +=1;
			trans.setIdentity();
			trans.setOrigin(btVector3(70, 40, 120));
			o2->rBody->setWorldTransform(trans);
			o2->rBody->setLinearVelocity(btVector3(-50, 0, -200));
		} else {
			o2->correctSide = true;
		} 
		if ((o2->rBody->getCenterOfMassPosition().getZ() < 0) && o2->hostHit == true) {
			// std::cout<<"Inside Host hit + host side\n";
			o2->clientScore +=1;
			trans.setIdentity();
			trans.setOrigin(btVector3(70, 40, 120));
			o2->rBody->setWorldTransform(trans);
			o2->rBody->setLinearVelocity(btVector3(-50, 0, -200));
			o2->clientHit = true;
			o2->hostHit = false;
		} else {
			o2->correctSide = true;
		}
	}	

	Ogre::String hostStr = Ogre::StringConverter::toString(o2->hostScore);
	Ogre::String clientStr = Ogre::StringConverter::toString(o2->clientScore);
	CEGUI::WindowManager::getSingleton().getWindow("CEGUIDemo/QuitButton")->setText("Player 1 : " + hostStr + "\n" + "Player 2 : " + clientStr);

	return false;
}

void GameState::initialise(Ogre::SceneManager* m) {
	mSceneMgr=m;
	score = 0;
	menuInput=false;

	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/materials/scripts", "Materials");
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();


	//Setting up CEGUI scoreboard
	mRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
	CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
	CEGUI::Font::setDefaultResourceGroup("Fonts");
	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");

	CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");

	CEGUI::MouseCursor::getSingleton().setImage(CEGUI::System::getSingleton().getDefaultMouseCursor());
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window *sheet =
			wmgr.createWindow("DefaultWindow", "CEGUIDemo/Sheet");

	CEGUI::Window *scoreWindow = wmgr.createWindow("TaharezLook/Button",
			"CEGUIDemo/QuitButton");
	scoreWindow->setText("Player 1 : 0\nPlayer 2 : 0");
	scoreWindow->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.12,
			0)));
	CEGUI::Window *info = wmgr.createWindow("TaharezLook/StaticText",
			"CEGUIDemo/Info");
	info->setText("k to toggle music off\nl to toggle music on\nwasd to rotate paddle\nmouse controls paddle\nl/r mouse buttons to hit l/r");
	info->setProperty("VertFormatting", "VertCentred"); // TopAligned, BottomAligned, VertCentred
	info->setProperty("HorzFormatting", "HorzCentred");
	info->setSize(CEGUI::UVector2(CEGUI::UDim(0.3, 0), CEGUI::UDim(0.15, 0)));
	info->setPosition(CEGUI::UVector2(CEGUI::UDim(0.7, 0), CEGUI::UDim(0, 0)));

	sheet->addChildWindow(scoreWindow);
	sheet->addChildWindow(info);
	CEGUI::System::getSingleton().setGUISheet(sheet);

	//Start SDL_mixer
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	// Set the scene's ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f));
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

	// Build the broadphase
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();

	// Set up the collision configuration and dispatcher
	btDefaultCollisionConfiguration* collisionConfiguration =
			new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	// The actual physics solver
	btSequentialImpulseConstraintSolver* solver =
			new btSequentialImpulseConstraintSolver;

	// The world.
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -98.f, 0));
	dynamicsWorld->getSolverInfo().m_splitImpulse=true;

	// Floor
	Ogre::Entity* floorE = mSceneMgr->createEntity("floor", "cube.mesh");
	floorE->setMaterialName("Examples/Floor");
	Ogre::SceneNode* floorSNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("FloorNode");
	floorSNode->attachObject(floorE);
	floorSNode->scale(20.0, 0.05, 20.0);

	btCollisionShape* floorShape = new btBoxShape(btVector3(1000,2.5,1000));
	btDefaultMotionState* floorMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-45,0)));
	btRigidBody::btRigidBodyConstructionInfo floorRigidBodyCI(0,
			floorMotionState, floorShape, btVector3(0, 0, 0));
	btRigidBody* floorRigidBody = new btRigidBody(floorRigidBodyCI);
	floorRigidBody->setRestitution(1.0);
	floorRigidBody->setFriction(0);
	floorRigidBody->setCollisionFlags(floorRigidBody->getCollisionFlags()
			| btCollisionObject::CF_KINEMATIC_OBJECT
			| btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	floorRigidBody->setActivationState(DISABLE_DEACTIVATION);
	dynamicsWorld->addRigidBody(floorRigidBody);
	GameObject* floor = new GameObject();
	floor->construct(floorSNode, floorRigidBody, -1);
	floorRigidBody->setUserPointer(floor);
	l.push_back(floor);

	// Table
	Ogre::Entity* tableE = mSceneMgr->createEntity("table", "cube.mesh");
	tableE->setMaterialName("Examples/Table");
	Ogre::SceneNode* tableSNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("TableNode");
	tableSNode->attachObject(tableE);
	tableSNode->scale(1.52, 0.05, 2.74);

	btCollisionShape* groundShape = new btBoxShape(btVector3(76,2.5,137));
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,0,0)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0,
			groundMotionState, groundShape, btVector3(0, 0, 0));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	groundRigidBody->setRestitution(1.0);
	groundRigidBody->setFriction(0);
	groundRigidBody->setCollisionFlags(groundRigidBody->getCollisionFlags()
			| btCollisionObject::CF_KINEMATIC_OBJECT
			| btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	groundRigidBody->setActivationState(DISABLE_DEACTIVATION);
	dynamicsWorld->addRigidBody(groundRigidBody);
	GameObject* tgo = new GameObject();
	tgo->construct(tableSNode, groundRigidBody, 0);
	groundRigidBody->setUserPointer(tgo);
	l.push_back(tgo);

	//Paddle
	Ogre::Entity* paddleE = mSceneMgr->createEntity("paddle", "paddle1.mesh");
	Ogre::SceneNode* paddleSNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("paddleNode");
	paddleSNode->attachObject(paddleE);
	paddleSNode->scale(1.5, 1.5, 1.5);

	btCollisionShape* paddleShape = new btBoxShape(btVector3(5,7.5,.05));
	btDefaultMotionState* paddleMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,0,-137)));
	btRigidBody::btRigidBodyConstructionInfo paddleRigidBodyCI(0,
			paddleMotionState, paddleShape, btVector3(0, 0, 0));
	btRigidBody* paddleRigidBody = new btRigidBody(paddleRigidBodyCI);
	paddleRigidBody->setRestitution(1.0);
	paddleRigidBody->setFriction(0);
	paddleRigidBody->setCollisionFlags(paddleRigidBody->getCollisionFlags()
			| btCollisionObject::CF_KINEMATIC_OBJECT
			| btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	paddleRigidBody->setActivationState(DISABLE_DEACTIVATION);
	dynamicsWorld->addRigidBody(paddleRigidBody);
	paddle = new GameObject();
	paddle->construct(paddleSNode, paddleRigidBody, 1);
	paddleRigidBody->setUserPointer(paddle);
	l.push_back(paddle);

	//AI Paddle
	Ogre::Entity* AIpaddleE = mSceneMgr->createEntity("AIpaddle", "paddle1.mesh");
	Ogre::SceneNode* AIpaddleSNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("AIpaddleNode");
	AIpaddleSNode->attachObject(AIpaddleE);
	AIpaddleSNode->scale(1.5, 1.5, 1.5);

	btCollisionShape* AIpaddleShape = new btBoxShape(btVector3(5,7.5,.05));
	btDefaultMotionState* AIpaddleMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,0,137)));
	btRigidBody::btRigidBodyConstructionInfo AIpaddleRigidBodyCI(0,
			AIpaddleMotionState, AIpaddleShape, btVector3(0, 0, 0));
	btRigidBody* AIpaddleRigidBody = new btRigidBody(AIpaddleRigidBodyCI);
	AIpaddleRigidBody->setRestitution(1.0);
	AIpaddleRigidBody->setFriction(0);
	AIpaddleRigidBody->setCollisionFlags(AIpaddleRigidBody->getCollisionFlags()
			| btCollisionObject::CF_KINEMATIC_OBJECT
			| btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	AIpaddleRigidBody->setActivationState(DISABLE_DEACTIVATION);
	dynamicsWorld->addRigidBody(AIpaddleRigidBody);
	AIpaddle = new GameObject();
	AIpaddle->construct(AIpaddleSNode, AIpaddleRigidBody, 2);
	AIpaddleRigidBody->setUserPointer(AIpaddle);
	l.push_back(AIpaddle);

	//Net
	Ogre::Entity* netE = mSceneMgr->createEntity("net", "cube.mesh");
	netE->setMaterialName("Examples/Net");
	Ogre::SceneNode* netSNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("netNode");
	netSNode->attachObject(netE);
	netSNode->scale(1.52, .1, 0.01);

	btCollisionShape* netShape = new btBoxShape(btVector3(76,5,.05));
	btDefaultMotionState* netMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,5,0)));
	btRigidBody::btRigidBodyConstructionInfo netRigidBodyCI(0, netMotionState,
			netShape, btVector3(0, 0, 0));
	btRigidBody* netRigidBody = new btRigidBody(netRigidBodyCI);
	netRigidBody->setRestitution(1.0);
	netRigidBody->setFriction(0);
	netRigidBody->setCollisionFlags(netRigidBody->getCollisionFlags()
			| btCollisionObject::CF_KINEMATIC_OBJECT
			| btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	netRigidBody->setActivationState(DISABLE_DEACTIVATION);
	dynamicsWorld->addRigidBody(netRigidBody);
	GameObject* net = new GameObject();
	net->construct(netSNode, netRigidBody, 4);
	netRigidBody->setUserPointer(net);
	l.push_back(net);

	//Ball
	Ogre::Entity* sphereE = mSceneMgr->createEntity("BALL", "sphere.mesh");
	Ogre::SceneNode* sphereSNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("SphereNode");
	sphereSNode->attachObject(sphereE);
	sphereSNode->scale(.026, .026, .026);

	btCollisionShape* fallShape = new btSphereShape(3);
	btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(70,40,120)));
	btScalar mass = 1;
	btVector3 fallInertia(0, 0, 0);
	fallShape->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,
			fallMotionState, fallShape, fallInertia);
	fallRigidBody = new btRigidBody(fallRigidBodyCI);
	fallRigidBody->setRestitution(.7);
	fallRigidBody->setLinearVelocity(btVector3(-50, 0, -200));
	fallRigidBody->setFriction(1);
	fallRigidBody->setDamping(0, .2);
	fallRigidBody->setActivationState(DISABLE_DEACTIVATION);
	fallRigidBody->setCollisionFlags(fallRigidBody->getCollisionFlags()
			| btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	dynamicsWorld->addRigidBody(fallRigidBody);
	ball = new GameObject();
	ball->construct(sphereSNode, fallRigidBody, 5);
	fallRigidBody->setUserPointer(ball);
	l.push_back(ball);
	gContactAddedCallback=callbackFunc;

	//Legs
	Ogre::Entity* leg1E = mSceneMgr->createEntity("leg1", "cube.mesh");
	leg1E->setMaterialName("Examples/Leg");
	Ogre::SceneNode* leg1SNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("leg1Node");
	leg1SNode->attachObject(leg1E);
	leg1SNode->scale(0.025, .45, 0.025);

	btCollisionShape* leg1Shape = new btBoxShape(btVector3(1.25,22.5,1.25));
	btDefaultMotionState* leg1MotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(72,-22.5,132)));
	btRigidBody::btRigidBodyConstructionInfo leg1RigidBodyCI(0,
			leg1MotionState, leg1Shape, btVector3(0, 0, 0));
	btRigidBody* leg1RigidBody = new btRigidBody(leg1RigidBodyCI);
	leg1RigidBody->setRestitution(1.0);
	leg1RigidBody->setFriction(0);
	leg1RigidBody->setCollisionFlags(leg1RigidBody->getCollisionFlags()
			| btCollisionObject::CF_KINEMATIC_OBJECT
			| btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	leg1RigidBody->setActivationState(DISABLE_DEACTIVATION);
	dynamicsWorld->addRigidBody(leg1RigidBody);
	GameObject* leg1 = new GameObject();
	leg1->construct(leg1SNode, leg1RigidBody, 6);
	leg1RigidBody->setUserPointer(leg1);
	l.push_back(leg1);

	Ogre::Entity* leg2E = mSceneMgr->createEntity("leg2", "cube.mesh");
	leg2E->setMaterialName("Examples/Leg");
	Ogre::SceneNode* leg2SNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("leg2Node");
	leg2SNode->attachObject(leg2E);
	leg2SNode->scale(0.025, .45, 0.025);

	btCollisionShape* leg2Shape = new btBoxShape(btVector3(1.25,22.5,1.25));
	btDefaultMotionState* leg2MotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(-72,-22.5,132)));
	btRigidBody::btRigidBodyConstructionInfo leg2RigidBodyCI(0,
			leg2MotionState, leg2Shape, btVector3(0, 0, 0));
	btRigidBody* leg2RigidBody = new btRigidBody(leg2RigidBodyCI);
	leg2RigidBody->setRestitution(1.0);
	leg2RigidBody->setFriction(0);
	leg2RigidBody->setCollisionFlags(leg2RigidBody->getCollisionFlags()
			| btCollisionObject::CF_KINEMATIC_OBJECT
			| btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	leg2RigidBody->setActivationState(DISABLE_DEACTIVATION);
	dynamicsWorld->addRigidBody(leg2RigidBody);
	GameObject* leg2 = new GameObject();
	leg2->construct(leg2SNode, leg2RigidBody, 6);
	leg2RigidBody->setUserPointer(leg2);
	l.push_back(leg2);

	Ogre::Entity* leg3E = mSceneMgr->createEntity("leg3", "cube.mesh");
	leg3E->setMaterialName("Examples/Leg");
	Ogre::SceneNode* leg3SNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("leg3Node");
	leg3SNode->attachObject(leg3E);
	leg3SNode->scale(0.025, .45, 0.025);

	btCollisionShape* leg3Shape = new btBoxShape(btVector3(1.25,22.5,1.25));
	btDefaultMotionState* leg3MotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(72,-22.5,-132)));
	btRigidBody::btRigidBodyConstructionInfo leg3RigidBodyCI(0,
			leg3MotionState, leg3Shape, btVector3(0, 0, 0));
	btRigidBody* leg3RigidBody = new btRigidBody(leg3RigidBodyCI);
	leg3RigidBody->setRestitution(1.0);
	leg3RigidBody->setFriction(0);
	leg3RigidBody->setCollisionFlags(leg3RigidBody->getCollisionFlags()
			| btCollisionObject::CF_KINEMATIC_OBJECT
			| btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	leg3RigidBody->setActivationState(DISABLE_DEACTIVATION);
	dynamicsWorld->addRigidBody(leg3RigidBody);
	GameObject* leg3 = new GameObject();
	leg3->construct(leg3SNode, leg3RigidBody, 6);
	leg3RigidBody->setUserPointer(leg3);
	l.push_back(leg3);

	Ogre::Entity* leg4E = mSceneMgr->createEntity("leg4", "cube.mesh");
	leg4E->setMaterialName("Examples/Leg");
	Ogre::SceneNode* leg4SNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("leg4Node");
	leg4SNode->attachObject(leg4E);
	leg4SNode->scale(0.025, .45, 0.025);

	btCollisionShape* leg4Shape = new btBoxShape(btVector3(1.25,22.5,1.25));
	btDefaultMotionState* leg4MotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(-72,-22.5,-132)));
	btRigidBody::btRigidBodyConstructionInfo leg4RigidBodyCI(0,
			leg4MotionState, leg4Shape, btVector3(0, 0, 0));
	btRigidBody* leg4RigidBody = new btRigidBody(leg4RigidBodyCI);
	leg4RigidBody->setRestitution(1.0);
	leg4RigidBody->setFriction(0);
	leg4RigidBody->setCollisionFlags(leg4RigidBody->getCollisionFlags()
			| btCollisionObject::CF_KINEMATIC_OBJECT
			| btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	leg4RigidBody->setActivationState(DISABLE_DEACTIVATION);
	dynamicsWorld->addRigidBody(leg4RigidBody);
	GameObject* leg4 = new GameObject();
	leg4->construct(leg4SNode, leg4RigidBody, 6);
	leg4RigidBody->setUserPointer(leg4);
	l.push_back(leg4);
}

void GameState::stepSimulation(void) {
	dynamicsWorld->stepSimulation(1/60.f, 1);
	updateAIpaddle();
	std::list<GameObject*>::iterator p;
	for (p=l.begin(); p!=l.end(); ++p) {
		(*p)->sync();
	}

}

void GameState::updateAIpaddle(void) {
	btTransform ballTrans;
	btTransform aiTrans;
	AIpaddle->rBody->getMotionState()->getWorldTransform(aiTrans);
	ball->rBody->getMotionState()->getWorldTransform(ballTrans);
	float aiX = aiTrans.getOrigin().getX();
	float aiY = aiTrans.getOrigin().getY();
	float ballX = ballTrans.getOrigin().getX();
	float ballY = ballTrans.getOrigin().getY();
	switch(ball->aiLevel){
		case 0:{
			float speedX=1;
			float speedY=1;
			if(ball->clientHit){
				if(aiX>0)
					speedX=-1*speedX;
				if(aiY>25)
					speedY=-1*speedY;
				if(abs(aiX+speedX)<=speedX)
					aiTrans.getOrigin().setX(0);
				else
					aiTrans.getOrigin().setX(aiX+speedX);
				if(abs(25 - (aiY+speedY))<=speedY)
					aiTrans.getOrigin().setY(25);
				else
					aiTrans.getOrigin().setY(aiY+speedY);

			}
			else{
				float speedX=.4;
				float speedY=.4;
				if(ballX<aiX)
					speedX=-1*speedX;
				if(ballY<aiY)
					speedY=-1*speedY;
				if(abs(ballX-(aiX+speedX))<=speedX)
					aiTrans.getOrigin().setX(ballX);
				else
					aiTrans.getOrigin().setX(aiX+speedX);
				if(abs(ballY - (aiY+speedY))<=speedY)
					aiTrans.getOrigin().setY(ballY);
				else
					aiTrans.getOrigin().setY(aiY+speedY);
			}

		}
		break;
		case 1:{
			float speedX=1;
			float speedY=1;
			if(ball->clientHit){
				if(aiX>0)
					speedX=-1*speedX;
				if(aiY>25)
					speedY=-1*speedY;

				if(abs(aiX+speedX)<=speedX)
					aiTrans.getOrigin().setX(0);
				else
					aiTrans.getOrigin().setX(aiX+speedX);
				if(abs(25 - (aiY+speedY))<=speedY)
					aiTrans.getOrigin().setY(25);
				else
					aiTrans.getOrigin().setY(aiY+speedY);



			}
			else{
				float speedX=1;
				float speedY=.6;
				if(ballX<aiX)
					speedX=-1*speedX;
				if(ballY<aiY)
					speedY=-1*speedY;
				if(abs(ballX-(aiX+speedX))<=speedX)
					aiTrans.getOrigin().setX(ballX);
				else
					aiTrans.getOrigin().setX(aiX+speedX);
				if(abs(ballY - (aiY+speedY))<=speedY)
					aiTrans.getOrigin().setY(ballY);
				else
					aiTrans.getOrigin().setY(aiY+speedY);
			}

		}
		break;
		case 2:{
			aiTrans.getOrigin().setX(ballX);
			aiTrans.getOrigin().setY(ballY);
		}
		break;
		case 3:{
			aiTrans.getOrigin().setX(ballX);
			aiTrans.getOrigin().setY(ballY);			
		}
		break;
	}

	AIpaddle->rBody->getMotionState()->setWorldTransform(aiTrans);
	if (ballTrans.getOrigin().getY()<100) {
		gameOver=true;

	}
	if (!gameOver)
		score=score+(1/60.f);
}

void GameState::updatePaddle(const OIS::MouseEvent &arg) {
	btTransform trans;
	paddle->rBody->getMotionState()->getWorldTransform(trans);
	trans.getOrigin().setY(trans.getOrigin().getY()-(arg.state.Y.rel/2));
	trans.getOrigin().setX(trans.getOrigin().getX()-(arg.state.X.rel/2));
	paddle->rBody->getMotionState()->setWorldTransform(trans);
	if(arg.state.buttonDown(OIS::MB_Left))
		ball->dir=1;
	else if(arg.state.buttonDown(OIS::MB_Right))
		ball->dir=2;
	else
		ball->dir=0;
	
}

bool GameState::rotateUp(void) {
	paddle->sNode->pitch( Ogre::Degree (1));
	btTransform trans;
	paddle->rBody->getMotionState()->getWorldTransform(trans);
	btQuaternion quat;

	const btScalar yaw = (paddle->sNode->getOrientation()).getYaw().valueRadians();
	const btScalar pitch = (paddle->sNode->getOrientation()).getPitch().valueRadians();
	const btScalar roll = (paddle->sNode->getOrientation()).getRoll().valueRadians();

	/*float x = paddle->sNode->getOrientation().getYaw().valueDegrees();
	float y = paddle->sNode->getOrientation().getPitch().valueDegrees();
	float z = paddle->sNode->getOrientation().getRoll().valueDegrees();

	btQuaternion quatTest;
	quatTest = paddle->rBody->getOrientation();

	/* std::cout << "X: " << x << "\n";
	std::cout << "Y: " << y << "\n";
	std::cout << "Z: " << z << "\n";*/
	quat.setEuler(yaw, pitch, roll);
	// quat.setEuler(1, 1, 1);
	trans.setRotation(quat);
	/*const &Ogre::Quaternion o_quat = paddle->sNode->getOrientation();
	trans.setRotation(btQuaternion(o_quat.x, o_quat.y, o_quat.z, o_quat.w));*/
	paddle->rBody->getMotionState()->setWorldTransform(trans);
	return true;
}

bool GameState::rotateLeft(void) {
	paddle->sNode->yaw( Ogre::Degree (-2));
	btTransform trans;
	paddle->rBody->getMotionState()->getWorldTransform(trans);
	btQuaternion quat;
	const btScalar yaw = (paddle->sNode->getOrientation()).getYaw().valueRadians();
	const btScalar pitch = (paddle->sNode->getOrientation()).getPitch().valueRadians();
	const btScalar roll = (paddle->sNode->getOrientation()).getRoll().valueRadians();
	quat.setEuler(yaw, pitch, roll);
	trans.setRotation(quat);
	paddle->rBody->getMotionState()->setWorldTransform(trans);
	return true;
}

bool GameState::rotateRight(void) {
	paddle->sNode->yaw( Ogre::Degree (2));
	btTransform trans;
	paddle->rBody->getMotionState()->getWorldTransform(trans);
	btQuaternion quat;
	const btScalar yaw = (paddle->sNode->getOrientation()).getYaw().valueRadians();
	const btScalar pitch = (paddle->sNode->getOrientation()).getPitch().valueRadians();
	const btScalar roll = (paddle->sNode->getOrientation()).getRoll().valueRadians();
	quat.setEuler(yaw, pitch, roll);
	trans.setRotation(quat);
	paddle->rBody->getMotionState()->setWorldTransform(trans);
	return true;
}

bool GameState::rotateDown(void) {
	paddle->sNode->pitch( Ogre::Degree (-1));
	btTransform trans;
	paddle->rBody->getMotionState()->getWorldTransform(trans);
	btQuaternion quat;
	const btScalar yaw = (paddle->sNode->getOrientation()).getYaw().valueRadians();
	const btScalar pitch = (paddle->sNode->getOrientation()).getPitch().valueRadians();
	const btScalar roll = (paddle->sNode->getOrientation()).getRoll().valueRadians();
	quat.setEuler(yaw, pitch, roll);
	trans.setRotation(quat);
	paddle->rBody->getMotionState()->setWorldTransform(trans);
	return true;
}

void QuaternionToEuler(const btQuaternion &TQuat, btVector3 &TEuler)
{
   btScalar W = TQuat.getW();
   btScalar X = TQuat.getX();
   btScalar Y = TQuat.getY();
   btScalar Z = TQuat.getZ();
   float WSquared = W * W;
   float XSquared = X * X;
   float YSquared = Y * Y;
   float ZSquared = Z * Z;
   TEuler.setX(atan2f(2.0f * (Y * Z + X * W), -XSquared - YSquared + ZSquared + WSquared));
   TEuler.setY(asinf(-2.0f * (X * Z - Y * W)));
   TEuler.setZ(atan2f(2.0f * (X * Y + Z * W), XSquared - YSquared - ZSquared + WSquared));
   TEuler *= 57.2957795f;
};