/*
 * ClientGameState.h
 *
 *  Created on: Apr 5, 2013
 *      Author: sva235
 */

#ifndef CLIENTGAMESTATE_H_
#define CLIENTGAMESTATE_H_

#include "GameState.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/asio.hpp>

class ClientGameState: public GameState {
public:
	ClientGameState(void);
	virtual ~ClientGameState();
	void initialise(Ogre::SceneManager* m, std::string ip);
	void setUpSockets(std::string ip);
	boost::asio::ip::tcp::socket* socket;
	bool connectionMade;
	void updateClientPos2(void);
	void stepSimulation(void);
	void updatePaddle(const OIS::MouseEvent &arg);
	void updateAIPaddle(void);
};

#endif /* CLIENTGAMESTATE_H_ */
