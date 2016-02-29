#include "RawPacket.h"
#include "OrderBuffer.h"

#include "boost/bind.hpp"

unsigned int RawPacket::m_count = 0;
boost::mutex RawPacket::m_mutex;

RawPacket::RawPacket(UdpSocket::pointer sock)
:m_transport(sock)
{
	//this->m_count ++;
}

RawPacket::~RawPacket(void)
{
	//this->m_count --;
}

void RawPacket::HandleReceive(const boost::system::error_code& error, size_t bytes_recvd)
{
	if(bytes_recvd > 0)
	{

		this->m_transport->StartReceive();
		this->m_length = bytes_recvd;

		//boost::mutex::scoped_lock lock(this->m_transport->m_mutex);
		//this->m_transport->m_packetDeque.push_back(shared_from_this());
		//unsigned short seq = ((unsigned short)this->m_buffer[2] << 8) | this->m_buffer[3];
		{
			boost::mutex::scoped_lock lock(RawPacket::m_mutex);
			RawPacket::m_count ++;
			RawPacket::m_count &= 0xFFFF; 
		}
		this->m_transport->m_orderBuffers->PutBuffer(RawPacket::m_count, shared_from_this());
	}
	else
	{

	}
	
}

void RawPacket::StartReceive()
{
	this->m_transport->m_socket.async_receive_from(
		boost::asio::buffer(this->m_buffer, RTP_BUFFER_SIZE),
		this->m_remoteEndpoint,
		boost::bind(&RawPacket::HandleReceive, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}
void RawPacket::StartSend(unsigned char* buf, unsigned int length, boost::asio::ip::udp::endpoint addr)
{
	this->m_transport->m_socket.async_send_to(
		boost::asio::buffer(this->m_buffer, this->m_length),
		addr,
		boost::bind(&RawPacket::HandleSend, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void RawPacket::HandleSend(const boost::system::error_code& error, size_t bytes_recvd)
{
	unsigned int ii = 0;
}