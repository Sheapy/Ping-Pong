#include "GameNet.h"
 
bool GameNet::Init() {
    if (SDLNet_Init() < 0) {
        std::cerr << "SDLNet_Init: " << SDLNet_GetError() << std::endl;
          return false;
        }
        else
            return true;
}
 
void GameNet::Quit() {
    SDLNet_Quit();
}
 
 
GameNetMessage::GameNetMessage() {
    reset();
}
 
void GameNetMessage::reset() {
    for (int i =0; i < 256; i++)
        buffer [i] = 0;
    state = EMPTY;
}
 
void GameNetMessage::finish() {
    if (state == READING)
        state = FULL;
}
 
int GameNetMessage::NumToLoad() {
    if (state == EMPTY)
        return 255;
    else
        return 0;
}
 
int GameNetMessage::NumToUnLoad() {
    if (state == FULL)
        return strlen(buffer) + 1;
    else
        return 0;
}
 
void GameNetMessage::LoadBytes(charbuf& inputbuffer, int n) {
    for (int i = 0; i < n; i++)
        buffer[i] = inputbuffer[i];
    state = READING;
}
 
void GameNetMessage::UnLoadBytes(charbuf& destbuffer) {
    for (int i=0; i < this->NumToUnLoad(); i++)
        destbuffer[i] = buffer[i];
    reset();
}

GameIpAddress::GameIpAddress (Uint16 port) {
    if (SDLNet_ResolveHost(&m_Ip, NULL, port) < 0){
        std::cerr << "SDLNet_ResolveHost: " << SDLNet_GetError() << std::endl;
        m_Ip.host = 0;
        m_Ip.port = 0;
    }
}
 
 
GameIpAddress::GameIpAddress (std::string host, Uint16 port) {
    if (SDLNet_ResolveHost(&m_Ip, host.c_str(), port) < 0) {
        std::cerr << "SDLNet_ResolveHost: " << SDLNet_GetError() << std::endl;
        m_Ip.host = 0;
        m_Ip.port = 0;
    }
}
 
GameIpAddress::GameIpAddress() {
    m_Ip.host = 0;
    m_Ip.port = 0;
}
 
bool GameIpAddress::Ok() const {
    return !(m_Ip.port == 0);
}
 
void GameIpAddress::SetIp (IPaddress sdl_ip) { //sets a CTcpSocket object from a existing SDL socket
    m_Ip = sdl_ip;
}
 
IPaddress GameIpAddress::GetIpAddress() const {
    return m_Ip;
}
 
Uint32 GameIpAddress::GetHost() const {
    return m_Ip.host;
}
 
Uint16 GameIpAddress::GetPort() const {
    return m_Ip.port;
}

GameTcpSocket::GameTcpSocket() {
    m_Socket = NULL;
    set = SDLNet_AllocSocketSet(1);
}
 
GameTcpSocket::~GameTcpSocket() {
    if (!(m_Socket == NULL)) {
        SDLNet_TCP_DelSocket(set,m_Socket);
        SDLNet_FreeSocketSet(set);
        SDLNet_TCP_Close (m_Socket);
    }
}
 
void GameTcpSocket::SetSocket (TCPsocket the_sdl_socket) {
    if (!(m_Socket == NULL)) {
          SDLNet_TCP_DelSocket(set,m_Socket);
          SDLNet_TCP_Close (m_Socket);
      }
    m_Socket = the_sdl_socket;
    if(SDLNet_TCP_AddSocket(set,m_Socket)==-1) {
        std::cerr << "SDLNet_TCP_AddSocket: " << SDLNet_GetError() << std::endl;
        m_Socket = NULL;
    }
}
 
bool GameTcpSocket::Ok() const {
    return !(m_Socket == NULL);
}
 
bool GameTcpSocket::Ready() const {
    bool rd = false;
    int numready = SDLNet_CheckSockets(set, 0);
    if (numready == -1)
            std::cerr << "SDLNet_CheckSockets: " << SDLNet_GetError() << std:: endl;
      else
            if (numready)
                rd = SDLNet_SocketReady (m_Socket);
      return rd;
}
 
void GameTcpSocket::OnReady() {
 
}

GameHostSocket::GameHostSocket (GameIpAddress& the_ip_address) {
    GameTcpSocket();
    IPaddress iph = the_ip_address.GetIpAddress();
    if (!(m_Socket = SDLNet_TCP_Open(&iph))) {
            SDLNet_FreeSocketSet(set);
            std::cerr << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
    }
}
 
GameHostSocket::GameHostSocket (Uint16 port) {
    GameIpAddress iplistener (port);
    if (!iplistener.Ok()) {
        m_Socket = NULL;
    }
    else {
        GameTcpSocket();
        IPaddress iph = iplistener.GetIpAddress();
        if (!(m_Socket = SDLNet_TCP_Open(&iph))) {
            SDLNet_FreeSocketSet(set);
            std::cerr << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
        }
    }
}
 
 
bool GameHostSocket::Accept (GameClientSocket& the_client_socket) {
    TCPsocket cs;
    if ((cs = SDLNet_TCP_Accept(m_Socket))) {
        the_client_socket.SetSocket(cs);
        return true;
    }
    else
        return false;
}
 
void GameHostSocket::OnReady() {
 
}

GameClientSocket::GameClientSocket() {
    GameTcpSocket();
}
 
GameClientSocket::GameClientSocket (std::string host, Uint16 port) {
    GameIpAddress remoteip (host.c_str(), port);
    if (!remoteip.Ok()) {
        m_Socket = NULL;
    }
    else {
        GameTcpSocket();
        Connect(remoteip);
    }
}
 
bool GameClientSocket::Connect (GameHostSocket& the_listener_socket) {
    return false;
}
 
bool GameClientSocket::Connect (GameIpAddress& remoteip) {
    TCPsocket cs;
    IPaddress ip = remoteip.GetIpAddress();
    if ((cs = SDLNet_TCP_Open(&ip)))
    {
        SetSocket(cs);
        return true;
    }
    else {
        std::cerr << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
        return false;
    }
}
 
void GameClientSocket::SetSocket (TCPsocket the_sdl_socket) {
    GameTcpSocket::SetSocket (the_sdl_socket);
    IPaddress* ips;
    if ((ips = SDLNet_TCP_GetPeerAddress(m_Socket))) {
    /* Print the address, converting it onto the host format */
        m_RemoteIp.SetIp(*ips);
        Uint32 hbo = m_RemoteIp.GetHost();
        Uint16 pbo = m_RemoteIp.GetPort();
        std::cout << "Client connected: " << SDLNet_Read32(&hbo) << ' '
            << SDLNet_Read16 (&pbo) << std::endl;
    }
    else
        std::cerr << "SDLNet_TCP_GetPeerAddress: " << SDLNet_GetError() << std::endl;
}
 
GameIpAddress GameClientSocket::getIpAddress () const {
    return m_RemoteIp;
}
 
bool GameClientSocket::Receive(GameNetMessage& rData) {
//Firstly, check if there is a socket
    if (m_Socket == NULL)
        return false;
    charbuf buf;
 
//Check if the instance can receive bytes, if it can, load the number of bytes specified by NumToLoad() virtual function
    while (rData.NumToLoad() > 0) {
        if (SDLNet_TCP_Recv(m_Socket, buf, rData.NumToLoad()) > 0) {
            rData.LoadBytes (buf, rData.NumToLoad());
        }
        else {
            return false;
        }
    }
    rData.finish();
    return true;
}
 
bool GameClientSocket::Send (GameNetMessage& sData) {
//check if there is a socket
    if (m_Socket == NULL)
        return false;
    charbuf buf;
    int len;
 
//Check if the instance can send bytes, if it can, unload the number of bytes specified by NumToLoad() virtual function
    while ((len = sData.NumToUnLoad()) > 0) {
        sData.UnLoadBytes (buf);
        if (SDLNet_TCP_Send(m_Socket, (void *)buf, len) < len) {
            std::cerr << "SDLNet_TCP_Send: " << SDLNet_GetError() << std::endl;
            return false;
        }
    }
    return true;
}
 
void GameClientSocket::OnReady() {
 
}
