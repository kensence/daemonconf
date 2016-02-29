#include "UdpSocket.h"
#include "RawPacket.h"

#include "IoServer.h"

IoServer0 io_service;
RawPacketPointer NULL_RTP_PACKET_POINTER(RawPacket::Create(UdpSocket::Create()));

#undef open
#undef close

#include "RawPacket.h"
#include "OrderBuffer.h"

UdpSocket::UdpSocket(void)
:m_socket(io_service.GetIoService())
,started(false)
{
	this->m_orderBuffers = new OrderBuffer();
}

UdpSocket::~UdpSocket(void)
{
	started = false;
	delete this->m_orderBuffers;
}

bool UdpSocket::Create(unsigned int ip, unsigned short port, unsigned int concurrent, unsigned int mcip)
{
	boost::system::error_code ec;

	boost::asio::ip::udp::endpoint endpoint(boost::asio::ip::address_v4(ip), port);

	m_socket.open(endpoint.protocol(), ec);
	if(ec)
	{
		return false;
	}

	m_socket.bind(endpoint, ec);

	if(mcip != 0)
	{
		// Join the multicast group.
		std::ostringstream smcip;
		smcip << mcip;
		m_socket.set_option(boost::asio::ip::multicast::join_group(boost::asio::ip::address::from_string(smcip.str())));
	}
	//boost::asio::ip::udp::socket::receive_buffer_size so_rcvbuf;
	//m_socket.get_option(so_rcvbuf);

	boost::asio::ip::udp::socket::send_buffer_size so_sndbuf(0);
	m_socket.set_option(so_sndbuf);

	if(ec)
	{
		m_socket.close();
		return false;
	}


	if(concurrent == 0)
	{
		concurrent = 1;
	}
	if(concurrent > 4)
	{
		concurrent = 4;
	}

	for(unsigned int i = 0; i < concurrent; i++)
	{
		this->StartReceive();
	}

	started = true;
	return true;
}

bool UdpSocket::GetSockName(unsigned int ip, unsigned short port)
{
	return false;
}

bool UdpSocket::Terminate(void)
{
	//this->m_socket.cancel();
	this->m_socket.close();
	this->m_orderBuffers->Clear();

	return true;
}

int UdpSocket::SendTo(unsigned char* buf, unsigned int len, unsigned int ip, unsigned short port)
{
	RawPacketPointer packet = RawPacket::Create(this->shared_from_this());

	if(len > RTP_BUFFER_SIZE)
	{
		len = RTP_BUFFER_SIZE;
	}
	memcpy(packet->m_buffer,buf,len);
	packet->m_length = len;

//	boost::asio::ip::udp::endpoint  peerAddr(boost::asio::ip::address_v4(ip));

	boost::asio::ip::udp::endpoint addr(boost::asio::ip::address_v4(ip), port);

	packet->StartSend(buf, len, addr);

	return len;
}

int UdpSocket::RecvFrom(unsigned char* buf, unsigned int len , unsigned int& ip, unsigned short& port)
{

	if (!started)
	{
		return 0;
	}

	RawPacketPointer packet;

	{
		//boost::mutex::scoped_lock lock(m_mutex);

		//if(this->m_packetDeque.size() == 0)
		//{
		//	return 0;
		//}
		//else
		//{
		//	packet = this->m_packetDeque.front();
		//	this->m_packetDeque.pop_front();
		//}

		packet = this->m_orderBuffers->GetBuffer();
		if(packet == NULL_RTP_PACKET_POINTER)
		{
			return 0;
		}
	}

	unsigned int len0 = len > packet->m_length ? packet->m_length : len;
	
	memcpy(buf, packet->m_buffer, len0);

	std::string sip = packet->m_remoteEndpoint.address().to_string();
	ip = ntohl(inet_addr(sip.c_str()));
	port = packet->m_remoteEndpoint.port();

	//addr.assign(AF_INET);
	//addr.ip(ip);
	//addr.port(mediaPort);
	
	return len0;
}

void UdpSocket::StartReceive()
{
	RawPacketPointer packet = RawPacket::Create(this->shared_from_this());
	packet->StartReceive();
}

bool UdpSocket::Valid()
{
	return this->m_socket.is_open();
}

void UdpSocket::Clear()
{
	this->m_orderBuffers->Clear();
}