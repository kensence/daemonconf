#include "ControlCenter.h"
#define WIN32_LEAN_AND_MEAN 
#include <cstdio>
#include <cstring>
#include <sstream>
#include <boost/foreach.hpp>  
#include <boost/property_tree/json_parser.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "json.h"
#include "version.h"
#include "clisession.h"
#include "IoServer.h"

extern std::string GenUuid();
extern std::string get_cur_time();

extern IoServer0 io_server;

bool validate_ip_address(const std::string& s)
{
	static const boost::regex e("(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)");
	return regex_match(s, e);
}

ControlCenter::ControlCenter(void)
:m_tp(60)
,m_cli(NULL)
{ 
}

ControlCenter::~ControlCenter(void)
{
	{
		boost::upgrade_lock<boost::shared_mutex> lock(m_devicesMutex1); 
		boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
		while(!this->m_devices.empty())
		{
			this->m_devices.erase(this->m_devices.begin());
		}
	}
	if(this->m_cli)
	{
		delete this->m_cli;
		this->m_cli = 0;
	}
}

extern std::string GenUuid();

bool ControlCenter::Init()
{
	this->DeviceInfo();

	std::string ret;
	std::cout << "Discover Device Info Finish!" << std::endl;

	this->m_cli = new CliAcceptor(0, this);
	
	return true;
}

bool ControlCenter::Message(const std::string & userid, const std::string & buf, std::string & ret, g_func func)
{
	std::istringstream xmlnIStream;
	boost::property_tree::ptree pt;
	xmlnIStream.str(buf);
	try
	{
		boost::property_tree::xml_parser::read_xml(xmlnIStream, pt, boost::property_tree::xml_parser::trim_whitespace);
		const char chflag = '/';
		boost::property_tree::ptree & message = pt.get_child(boost::property_tree::ptree::path_type("message", chflag));
		const std::string & function = message.get<std::string>("function");
		if(function == "user_login")
		{
			return this->UserLogin(message, ret);
		}
		else if(function == "link_add")
		{

		}
		else
		{

		}
		return false;
	}
	catch (std::exception& e)
	{
		std::cout << "Error: " << e.what() << "\n";
		return false;
	}
}

bool ControlCenter::UserLogin(boost::property_tree::ptree & message, std::string & ret)
{
	const char chflag = '/';
	boost::property_tree::ptree & user = message.get_child(boost::property_tree::ptree::path_type("user", chflag));

	const std::string & name = user.get<std::string>("name");
	const std::string & password = user.get<std::string>("password");

	ret += "<message><function>user_login</function>";
	if(name == "admin" && password == "admin")
	{
		//OK
		ret += "<status><code>0</code><info>success</info></status></message>";
		return true;
	}
	else
	{
		ret += "<status><code>1</code><info>user does not exist</info></status></message>";
	}
	return false;
}
bool ControlCenter::DeviceInfo()
{

	return true;
}
