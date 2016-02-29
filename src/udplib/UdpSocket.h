#pragma once

#include "boost/asio.hpp"
#include <boost/enable_shared_from_this.hpp>
#include "boost/thread.hpp"

#include "queue"

class OrderBuffer;
class RawPacket;
typedef boost::shared_ptr<RawPacket> RawPacketPointer;
//Ò»¸öUDP SOCKET
class UdpSocket : public boost::enable_shared_from_this<UdpSocket> 
{
	friend class RawPacket;

public:
	typedef boost::shared_ptr<UdpSocket> pointer;

	static pointer Create()
	{
		return pointer(new UdpSocket());
	};

private:
	UdpSocket(void);

public:
	~UdpSocket(void);

public:
	bool Create(unsigned int ip, unsigned short port, unsigned int concurrent = 1, unsigned int mcip = 0);
	bool GetSockName(unsigned int ip, unsigned short port);
	bool Terminate(void);
	int SendTo(unsigned char* buf, unsigned int len, unsigned int ip, unsigned short port);
	int RecvFrom(unsigned char* buf, unsigned int len , unsigned int& ip, unsigned short& port);
	bool Valid();
	void Clear();


private:
	boost::asio::ip::udp::socket m_socket;
	boost::asio::ip::udp::endpoint m_localAddr;
	boost::asio::ip::udp::endpoint m_remoteAddr;

	//std::deque<RawPacketPointer> m_packetDeque;

	OrderBuffer * m_orderBuffers;
	
	//boost::mutex m_mutex;
	bool started;

private:
	void StartReceive();
};
