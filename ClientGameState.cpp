/*
 * ClientGameState.cpp
 *
 *  Created on: Apr 5, 2013
 *      Author: sva235
 */

#include "ClientGameState.h"
using boost::asio::ip::tcp;
ClientGameState::ClientGameState(void) {

}

ClientGameState::~ClientGameState() {
	// TODO Auto-generated destructor stub
}

void ClientGameState::initialise(Ogre::SceneManager* m, std::string ip) {
	GameState::initialise(m);
	ball->rBody->setCollisionFlags(ball->rBody->getCollisionFlags()
			| btCollisionObject::CF_KINEMATIC_OBJECT
			| btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	ball->setMP(true);
	setUpSockets(ip);
	std::cout << "hello" << std::endl;
}

void ClientGameState::setUpSockets(std::string ip) {
	try {
		boost::asio::io_service io_service;
		socket = new tcp::socket(io_service);
		boost::asio::ip::tcp::resolver resolver(io_service);
		boost::asio::ip::tcp::resolver::query query(ip, "");
		boost::asio::ip::tcp::endpoint end;
		for (boost::asio::ip::tcp::resolver::iterator i = resolver.resolve(
				query); i != boost::asio::ip::tcp::resolver::iterator(); ++i) {
			end = *i;
			end.port(2911);
			std::cout << end.address() << ' ';
			break;
		}
		std::cout << '\n';
		socket->connect(end);
		connectionMade=true;
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}

void ClientGameState::updateClientPos2(void) {
	// read header
	size_t header;
	boost::asio::read(*socket, boost::asio::buffer(&header, sizeof(header)));
	std::cout << "body is " << header << " bytes" << std::endl;

	// read body
	boost::asio::streambuf buf;
	const size_t rc = boost::asio::read(*socket, buf.prepare(header));
	buf.commit(header);
	std::cout << "read " << rc << " bytes" << std::endl;

	// deserialize
	std::istream is(&buf);
	boost::archive::text_iarchive ar(is);
	ServerStruct msg;
	ar & msg;
	btTransform t;
	paddle->rBody->getMotionState()->getWorldTransform(t);
	t.getOrigin().setX(msg.sX);
	t.getOrigin().setY(msg.sY);
	t.getOrigin().setZ(msg.sZ);
	paddle->rBody->getMotionState()->setWorldTransform(t);
	btTransform b;
	ball->rBody->getMotionState()->getWorldTransform(b);
	b.getOrigin().setX(msg.bX);
	b.getOrigin().setY(msg.bY);
	b.getOrigin().setZ(msg.bZ);
	ball->rBody->getMotionState()->setWorldTransform(b);

	ClientStruct c;
	btTransform pad;

	AIpaddle->rBody->getMotionState()->getWorldTransform(pad);
	c.cX=pad.getOrigin().getX();
	c.cY=pad.getOrigin().getY();
	c.cZ=pad.getOrigin().getZ();
    boost::asio::streambuf buf2;
    std::ostream os( &buf2 );
    boost::archive::text_oarchive ar2( os );
    ar2 & c;

    const size_t header2 = buf2.size();
    std::cout << "buffer size " << header2 << " bytes" << std::endl;

    // send header and buffer using scatter
    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back( boost::asio::buffer(&header2, sizeof(header2)) );
    buffers.push_back( buf2.data() );
    const size_t rc2 = boost::asio::write(
            *socket,
            buffers
            );
    std::cout << "wrote " << rc2 << " bytes2" << std::endl;;


}

void ClientGameState::stepSimulation(void) {
	if (connectionMade) {
		updateClientPos2();
		dynamicsWorld->stepSimulation(1 / 60.f, 1);
		std::list<GameObject*>::iterator p;
		for (p = l.begin(); p != l.end(); ++p) {
			(*p)->sync();
		}
	}
}

void ClientGameState::updatePaddle(const OIS::MouseEvent &arg){
	btTransform trans;
	AIpaddle->rBody->getMotionState()->getWorldTransform(trans);
	trans.getOrigin().setY(trans.getOrigin().getY()-(arg.state.Y.rel/2));
	trans.getOrigin().setX(trans.getOrigin().getX()+(arg.state.X.rel/2));
	AIpaddle->rBody->getMotionState()->setWorldTransform(trans);
}

void ClientGameState::updateAIPaddle(void){

}
