#ifndef SERVERGAMESTATE_H_
#define SERVERGAMESTATE_H_

#include "GameState.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/asio.hpp>
#include "tcpserver.cpp"


class ServerGameState : public GameState
{
public:
	ServerGameState(void);
	virtual ~ServerGameState();
	void stepSimulation(void);
	void initialise(Ogre::SceneManager* m,OgreBites::SdkTrayManager* mT);
	void setUpSockets(void);
	boost::asio::ip::tcp::socket* socket;
	boost::asio::io_service* io_service;
	bool connectionMade;
	void updateClientPos(void);
	void accept_handler(boost::asio::ip::tcp::socket* pSocket);
	void read_handler(const boost::system::error_code& error);
	OgreBites::SdkTrayManager* mTrayMgr;
	size_t readHeader;
};

#endif /*SERVERGAMESTATE_H_*/
