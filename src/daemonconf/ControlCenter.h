#pragma once

#include <string>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/thread.hpp>
#include <boost/threadpool.hpp>

#define RESULT_BUFFER 512
class CliAcceptor;
class Device;

typedef boost::function<void (std::string)> g_func;

class ControlCenter
{
public:
	ControlCenter(void);
	~ControlCenter(void);

private:
	CliAcceptor * m_cli;
	boost::shared_mutex m_devicesMutex1;
	std::map<std::string, boost::shared_ptr<Device> > m_devices;

public:
	bool Init();
	bool Message(const std::string & userid, const std::string & buf, std::string & ret, g_func func = 0);
	bool UserLogin(boost::property_tree::ptree & message, std::string &ret);

	bool SetDeviceIP(boost::property_tree::ptree & message, std::string &ret);
	bool SetDeviceName(boost::property_tree::ptree & message, std::string &ret);
	bool SetDeviceSrcIP(boost::property_tree::ptree & message, std::string &ret);
	bool SetDeviceBitRate(boost::property_tree::ptree & message, std::string &ret);
	bool SetReboot(boost::property_tree::ptree & message, std::string &ret);

	bool GetDevices(boost::property_tree::ptree & message, std::string &ret);
	bool GetDeviceInfo(boost::property_tree::ptree & message, std::string &ret);
private:
	boost::threadpool::pool m_tp;
};
