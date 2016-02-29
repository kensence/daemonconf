#pragma once

#include "string"

typedef enum _type{
	TX,
	RX
}TYPE_DEV;

class Device
{
public:
	Device(const std::string &name);
	virtual ~Device(void);

public:
	void ip(const std::string & ip){m_ip = ip;};
	std::string ip(){return m_ip;};
	TYPE_DEV type(){return m_type;};
	void type(TYPE_DEV _type){ m_type = _type; };
	unsigned int port(){ return m_port; };
	void port(unsigned int port){ m_port = port; };
	std::string name(){ return m_name; };
	void name(const std::string& name){ m_name = name; };
private:
	std::string m_name;
	TYPE_DEV m_type;
	std::string m_ip;
	unsigned int m_port;
};

#define MAX_COUNT 3
