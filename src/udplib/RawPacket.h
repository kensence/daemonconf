#pragma once

#include "UdpSocket.h"

const unsigned int RTP_BUFFER_SIZE = 8192;
//代表一个接收或发送的RTP数据包
class RawPacket : public boost::enable_shared_from_this<RawPacket> 
{
	friend class UdpSocket;
public:
	static RawPacketPointer Create(UdpSocket::pointer sock)
	{
		return RawPacketPointer(new RawPacket(sock));
	};

private:
	RawPacket(UdpSocket::pointer sock);
public:
	~RawPacket();

private:
	UdpSocket::pointer m_transport;

	unsigned char m_buffer[RTP_BUFFER_SIZE];

	unsigned int m_length;
	boost::asio::ip::udp::endpoint m_remoteEndpoint;

	static unsigned int m_count;
	static boost::mutex m_mutex;
private:
	void HandleReceive(const boost::system::error_code& error, size_t bytes_recvd);
	void StartReceive();
	void StartSend(unsigned char* buf, unsigned int length, boost::asio::ip::udp::endpoint addr);
	void HandleSend(const boost::system::error_code& error, size_t bytes_recvd);
};