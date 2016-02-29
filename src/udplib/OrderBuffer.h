#pragma once

#include "UdpSocket.h"

extern RawPacketPointer NULL_RTP_PACKET_POINTER;

//rtp°ü»º´æÅÅÐò
class OrderBuffer
{
public:
	OrderBuffer();
	~OrderBuffer(void);
private:
	std::map<unsigned short, RawPacketPointer> m_usedBuffer;
	boost::mutex m_criticalSection;
	unsigned int m_nMaxSize;//the max size of the map buffer
	unsigned int m_nKeyForGet;//the getting buffer's key 
	unsigned int m_nPreKey;//the previous getting buffer's key 

	bool m_isStart;
public:
	RawPacketPointer GetBuffer(void);
	bool PutBuffer(unsigned int nKey, RawPacketPointer ioData);
	//int GetLength();
	//void SetMaxSize(int nMaxsize);
	unsigned int GetFirstKey();
	void Clear();
};