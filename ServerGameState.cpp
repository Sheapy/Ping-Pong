#include "ServerGameState.h"

using boost::asio::ip::tcp;

ServerGameState::ServerGameState(void) : connectionMade(false)
{
}

ServerGameState::~ServerGameState()
{
}

void ServerGameState::initialise(Ogre::SceneManager* m,OgreBites::SdkTrayManager* mT){
	GameState::initialise(m);
	mTrayMgr=mT;
	ball->setMP(true);
	setUpSockets();

}

void ServerGameState::setUpSockets(void){

	try
	  {
		io_service=new boost::asio::io_service();
	    socket = new tcp::socket(*io_service);
	    tcp::acceptor* acceptor = new tcp::acceptor(*io_service, tcp::endpoint(tcp::v4(), 2911)) ;
	    acceptor->listen();
        acceptor->async_accept(*socket,boost::bind(&ServerGameState::accept_handler, this, socket));
        boost::thread bt(boost::bind(&boost::asio::io_service::run, io_service));
	  }
	  catch (std::exception& e)
	  {
	    std::cerr << "what" << std::endl;
	  }
}

void ServerGameState::updateClientPos(void){
	ServerStruct ss;
	btTransform t;
	GameState::AIpaddle->rBody->getMotionState()->getWorldTransform(t);
	ss.cX=t.getOrigin().getX();
	ss.cY=t.getOrigin().getY();
	ss.cZ=t.getOrigin().getZ();

	GameState::ball->rBody->getMotionState()->getWorldTransform(t);
	ss.bX=t.getOrigin().getX();
	ss.bY=t.getOrigin().getY();
	ss.bZ=t.getOrigin().getZ();

	GameState::paddle->rBody->getMotionState()->getWorldTransform(t);
	ss.sX=t.getOrigin().getX();
	ss.sY=t.getOrigin().getY();
	ss.sZ=t.getOrigin().getZ();

    boost::asio::streambuf buf;
    std::ostream os( &buf );
    boost::archive::text_oarchive ar( os );
    ar & ss;

    const size_t header = buf.size();
    // std::cout << "buffer size " << header << " bytes" << std::endl;

    // send header and buffer using scatter
    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back( boost::asio::buffer(&header, sizeof(header)) );
    buffers.push_back( buf.data() );
    const size_t rc = boost::asio::write(
            *socket,
            buffers
            );
    // std::cout << "wrote " << rc << " bytes" << std::endl;;

	// read header
	size_t header2;
	boost::asio::read(*socket, boost::asio::buffer(&header2, sizeof(header2)));
	// std::cout << "body is " << header2 << " bytes" << std::endl;

	// read body
	boost::asio::streambuf buf2;
	const size_t rc2 = boost::asio::read(*socket, buf2.prepare(header2));
	buf2.commit(header2);
	// std::cout << "read " << rc2 << " bytes" << std::endl;

	// deserialize
	std::istream is(&buf2);
	boost::archive::text_iarchive ar2(is);
	ClientStruct c;
	ar2 & c;

	btTransform aipad;
	AIpaddle->rBody->getMotionState()->getWorldTransform(aipad);
	aipad.getOrigin().setX(c.cX);
	aipad.getOrigin().setY(c.cY);
	aipad.getOrigin().setZ(c.cZ);
	AIpaddle->rBody->getMotionState()->setWorldTransform(aipad);





}

void ServerGameState::stepSimulation(void){
	if(connectionMade){
		updateClientPos();
		dynamicsWorld->stepSimulation(1/60.f, 1);
		std::list<GameObject*>::iterator p;
		for (p=l.begin(); p!=l.end(); ++p) {
			(*p)->sync();
		}		
	}
}

void ServerGameState::accept_handler(boost::asio::ip::tcp::socket* pSocket)
{
	connectionMade=true;
	mTrayMgr->hideAll();
	std::cout << "hell" << std::endl;;
}

void ServerGameState::read_handler(const boost::system::error_code& error)
{
	// read body
	boost::asio::streambuf buf;
	const size_t rc = boost::asio::read(*socket, buf.prepare(readHeader));
	buf.commit(readHeader);
	std::cout << "read " << rc << " bytes22" << std::endl;

	// deserialize
	std::istream is(&buf);
	boost::archive::text_iarchive ar(is);
	ClientStruct msg;
	ar & msg;
	btTransform t;
	AIpaddle->rBody->getMotionState()->getWorldTransform(t);
	t.getOrigin().setX(msg.cX);
	t.getOrigin().setY(msg.cY);
	t.getOrigin().setZ(msg.cZ);
	AIpaddle->rBody->getMotionState()->setWorldTransform(t);
}
