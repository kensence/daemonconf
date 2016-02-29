#include "OrderBuffer.h"
#include "RawPacket.h"

static const unsigned int INVALID_KEY = 65536;

//RawPacketPointer NULL_RTP_PACKET_POINTER(RawPacket::Create(UdpSocket::Create()));

OrderBuffer::OrderBuffer()
: m_nMaxSize(0)
, m_nKeyForGet(INVALID_KEY)
, m_nPreKey(INVALID_KEY)
, m_isStart(false)
{
	boost::mutex::scoped_lock lock(m_criticalSection);
	m_usedBuffer.clear();
	m_isStart = true;
};

OrderBuffer::~OrderBuffer(void)
{
	this->Clear();
	m_isStart = false;
};
RawPacketPointer OrderBuffer::GetBuffer(void)
{
	boost::mutex::scoped_lock lock(m_criticalSection);

	if(!m_isStart) return NULL_RTP_PACKET_POINTER;

	unsigned int size = m_usedBuffer.size();
	if(size >= 50 && size % 50 == 0)
	{
		//std::cout << "OrderBuffer::GetBuffer : length = " << m_usedBuffer.size() << std::endl;
	}

	if(m_usedBuffer.size() <= m_nMaxSize)//buffer pool not full
	{
		return NULL_RTP_PACKET_POINTER;
	}
	std::map<unsigned short, RawPacketPointer>::iterator iter,iterFirst,iterEnd,iterNext;
	RawPacketPointer ioData;

	iter = m_usedBuffer.find(m_nKeyForGet);

	if (iter == m_usedBuffer.end())//not found
	{
		//search the proper output buffer
		iterFirst = m_usedBuffer.begin();
		iterEnd =  m_usedBuffer.end();
		if(iterFirst == iterEnd)//empty
		{
			return NULL_RTP_PACKET_POINTER;
		}
		iter = iterEnd;
		while(--iter != iterFirst)
		{
			if(abs(iter->first - iterFirst->first) > 60000) 
			{
				continue;
			}
			break;
		};
		if(++iter == iterEnd) 
		{
			iter = iterFirst;	//sequence not restarted
		}
	}

	ioData = iter->second;
	m_nPreKey = iter->first;
	iterNext = iter;		//next buffer
	iterNext++;
	m_usedBuffer.erase(iter);

	//get the next output buffer's key
	if(m_usedBuffer.size() == 0)
	{
		m_nKeyForGet = INVALID_KEY;
	}
	else
	{
		if(iterNext == m_usedBuffer.end())
		{
			m_nKeyForGet = (m_usedBuffer.begin())->first;
		}
		else
		{
			m_nKeyForGet = iterNext->first;
		}
	}
	//Debug(DebugWarn, "key = %d : ioData = %x\n",m_nKeyForGet,ioData);
	return ioData;
};

bool OrderBuffer::PutBuffer(unsigned int nKey, RawPacketPointer ioData)
{
	boost::mutex::scoped_lock lock(m_criticalSection);

	if(!m_isStart) return 0;

	typedef std::map<unsigned short, RawPacketPointer>::value_type Buffer_Pair;

	if(INVALID_KEY == m_nPreKey)// m_nPreKey = 65536,never get buffer
	{
		std::map<unsigned short, RawPacketPointer>::iterator iterFirst;
		m_usedBuffer.insert(Buffer_Pair(nKey,ioData));
		m_nKeyForGet = GetFirstKey();
		return true;
	}

	int subValue = abs((int)(nKey-m_nPreKey));
	if(nKey > m_nPreKey)
	{
		if(subValue > 60000) return false;//lost package ,throw it. m_nPreKey = 0, nKey = 65534
	}else
	{
		if(subValue < 60000) return false;//lost package ,throw it. m_nPreKey = 9, nKey = 0
	}

	// nKey is valid sequence after m_nPreKey
	int nbufSize = m_usedBuffer.size();
	if (nbufSize == 0)
	{
		m_usedBuffer.insert(Buffer_Pair(nKey,ioData));
		m_nKeyForGet = nKey;
		return true;
	}

	if(nKey>=m_nKeyForGet)
	{
		m_usedBuffer.insert(Buffer_Pair(nKey,ioData));
	}else
	{
		if(abs(int(nKey-m_nKeyForGet)) < 60000 )//nKey not restart
		{
			m_nKeyForGet = nKey;
		}
		m_usedBuffer.insert(Buffer_Pair(nKey,ioData));
	}

	return true;
};

//int OrderBuffer::GetLength()
//{
//	return m_usedBuffer.size();
//};

//void OrderBuffer::SetMaxSize(int nMaxsize)
//{
//	m_nMaxSize = nMaxsize;
//};

unsigned int OrderBuffer::GetFirstKey()
{
	std::map<unsigned short, RawPacketPointer>::iterator iter,iterFirst,iterEnd;
	RawPacketPointer ioData = NULL_RTP_PACKET_POINTER;

	//search the proper output buffer
	iterFirst = m_usedBuffer.begin();
	iterEnd =  m_usedBuffer.end();
	if(iterFirst == iterEnd)//empty
	{
		return INVALID_KEY;
	}
	iter = iterEnd;
	while(--iter != iterFirst)
	{
		if(abs(iter->first - iterFirst->first) > 60000) 
		{
			continue;//sequence  restarted
		}
		break;
	};
	if(++iter == iterEnd) 
	{
		iter = iterFirst;	//sequence not restarted
	}

	return iter->first;
}

void OrderBuffer::Clear()
{
	std::map<unsigned short, RawPacketPointer>::iterator iter;
	RawPacketPointer ioData;

	boost::mutex::scoped_lock lock(m_criticalSection);
	//printf("OrderBuffer::Clear: usedbuffer len = %d\n", this->m_usedBuffer.size());
	//OutputDebugPrintf("RTPBufferMap:~RTPBufferMap : size = %d\n ",m_usedBuffer.size());

	while (m_usedBuffer.size() > 0)
	{
		iter = m_usedBuffer.begin();
		//OutputDebugPrintf("key = %d : ioData = %x\n",iter->first,iter->second);
		ioData = iter->second;
		m_usedBuffer.erase(iter);
		//delete ioData;
	}
}