#pragma once

#include "Device.h"

class Cnw8302:public Device
{
public:
	Cnw8302(const std::string &name);
	~Cnw8302(void);
public:
	bool SetDevName(std::string &name);
	bool GetDevName(std::string &result);

	bool SetDevIP(std::string &ip);
	bool GetDevIP(std::string &result);

	bool SetDevMac(std::string &mac);
	bool GetDevMac(std::string &result);

	bool SetDevSrcIP(std::string &srcip);
	bool GetDevSrcIp(std::string &result);

	bool SetDevBitRate(std::string &bitrate);
	bool GetDevBitRate(std::string &result);

	bool SetDevType(TYPE_DEV &type);
	bool GetDevType(std::string &result);

	bool SetReboot();
};
