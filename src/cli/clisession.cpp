#include "clisession.h"
#include "iostream"
#include "boost/bind.hpp"
#include "ControlCenter.h"
#include "IoServer.h"
extern IoServer0 io_server;

#define BOOST_SPIRIT_THREADSAFE

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/foreach.hpp>
#include "boost/typeof/typeof.hpp"
#include "json.h"
#include "xml.h"

CliAcceptor::CliAcceptor(unsigned int ip, ControlCenter *cc)
:m_acceptIp(ip)
,m_io_service(io_server.GetIoService())
,m_CliAcceptor(io_server.GetIoService())
,m_cc(cc)
{
	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4(m_acceptIp), 8888);
	m_CliAcceptor.open(endpoint.protocol());
	m_CliAcceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(1));
	m_CliAcceptor.bind(endpoint);
	m_CliAcceptor.listen();
	this->StartAccept();
}

CliAcceptor::~CliAcceptor(void)
{
	this->m_CliAcceptor.close();
	//std::cout << "CliAcceptor::~CliAcceptor\n";
}

void CliAcceptor::StartAccept()
{

	CliSession::pointer new_session = CliSession::Create(m_acceptIp, this, this->m_cc);

	m_CliAcceptor.async_accept(new_session->Socket(), 
		boost::bind(&CliAcceptor::HandleAccept, this, new_session, boost::asio::placeholders::error));
}

void CliAcceptor::HandleAccept(CliSession::pointer new_session, const boost::system::error_code& err)
{
	if (!err)
	{
		{
			boost::weak_ptr<CliSession> session = new_session->shared_from_this();
			boost::recursive_mutex::scoped_lock lock(m_mutex);
			this->m_sessions.insert(this->m_sessions.end(), session);
		}
		new_session->Start();
		this->StartAccept();
	}
}

bool CliAcceptor::GetH245Address(unsigned int &ip, unsigned short &port)
{
	if(this->m_CliAcceptor.is_open())
	{
		std::string sip = this->m_CliAcceptor.local_endpoint().address().to_string();
		
		ip = ntohl(inet_addr(sip.c_str()));

		port = this->m_CliAcceptor.local_endpoint().port();
		
		return true;
	}
	return false;
}
//keep one user id  in sessions
bool CliAcceptor::HandleOneUserLogin(CliSession::pointer exclude)
{
	boost::recursive_mutex::scoped_lock lock(m_mutex);
	std::vector<boost::weak_ptr<CliSession> >::iterator iter = m_sessions.begin();
	while(iter != m_sessions.end())
	{
		if((*iter).expired())
		{
			this->m_sessions.erase(iter);
			iter = m_sessions.begin();
			//printf("session count = %d\n", m_sessions.size());
		}
		else
		{
			iter++;
		}
	}
	for(iter = m_sessions.begin(); iter != m_sessions.end(); iter++)
	{
		boost::shared_ptr<CliSession> session = iter->lock();
		std::string str = session->UserId();
		if(session != exclude &&  exclude->UserId() == str)
		{
			// delete other's sessions
			session->Socket().close();
		}
	}
	return true;
}

bool CliAcceptor::GetOnLineUsers(CliSession::pointer exclude, std::vector<std::string> &users)
{
	boost::recursive_mutex::scoped_lock lock(m_mutex);

	std::vector<boost::weak_ptr<CliSession> >::iterator iter = m_sessions.begin();

	while(iter != m_sessions.end())
	{
		if((*iter).expired())
		{
			this->m_sessions.erase(iter);
			iter = m_sessions.begin();
		}
		else
		{
			iter++;
		}
	}
	for(iter = m_sessions.begin(); iter != m_sessions.end(); iter++)
	{
		boost::shared_ptr<CliSession> session = iter->lock();
		std::string str = session->UserId();
		if(session != exclude && !str.empty())
		{
			users.push_back(str);
		}
	}
	users.push_back(exclude->UserId());

	return true;
}

bool CliAcceptor::SendMessage(const std::string & userid, CliSession::pointer exclude, std::string &msg)
{
	boost::recursive_mutex::scoped_lock lock(m_mutex);

	std::vector<boost::weak_ptr<CliSession> >::iterator iter = m_sessions.begin();

	while(iter != m_sessions.end())
	{
		if((*iter).expired())
		{
			this->m_sessions.erase(iter);
			iter = m_sessions.begin();
		}
		else
		{
			iter++;
		}
	}

	for(iter = m_sessions.begin(); iter != m_sessions.end(); iter++)
	{
		boost::shared_ptr<CliSession> session = iter->lock();
		std::string str = session->UserId();
		if(session != exclude && userid == session->UserId())
		{
			session->SendMessage(msg);
		}
	}
	return true;
}

bool CliAcceptor::SendMessage1(std::string &msg)
{
	boost::recursive_mutex::scoped_lock lock(m_mutex);
	std::vector<boost::weak_ptr<CliSession> >::iterator iter = m_sessions.begin();
	while(iter != m_sessions.end())
	{
		if((*iter).expired())
		{
			this->m_sessions.erase(iter);
			iter = m_sessions.begin();
		}
		else
		{
			iter++;
		}
	}
	for(iter = m_sessions.begin(); iter != m_sessions.end(); iter++)
	{
		boost::shared_ptr<CliSession> session = iter->lock();
		session->SendMessage2(msg);
	}
	return true;
}

unsigned int H245_SOCKET_BUFFER_LENGTH = 65536;

CliSession::CliSession(unsigned int ip,  CliAcceptor * acceptor, ControlCenter* cc)
:m_localIp(ip)
,io_service_(io_server.GetIoService())
,socket_(io_server.GetIoService())
,strand_(io_server.GetIoService())
,m_acceptor(acceptor)
,m_cc(cc)
{
	m_h245SignalPacket = new unsigned char[H245_SOCKET_BUFFER_LENGTH];
}

CliSession::~CliSession(void)
{
	delete[] this->m_h245SignalPacket;
}


boost::asio::ip::tcp::socket& CliSession::Socket()
{
	return socket_;
}

void CliSession::StartRead()
{
	socket_.async_read_some(boost::asio::buffer(m_h245SignalPacket, H245_SOCKET_BUFFER_LENGTH), 
		strand_.wrap(boost::bind(&CliSession::HandleRead, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)));
}
void CliSession::Start(bool callout, unsigned int ip, unsigned short port)
{
	if(callout)
	{
		boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4(ip),port);
		this->socket_.connect(endpoint);
		this->StartRead();
	}
	else
	{
		this->StartRead();
	}

}

void CliSession::HandleRead(const boost::system::error_code& err, size_t length)
{
	if(length == 0)
	{
		return;
	}
	{
		boost::mutex::scoped_lock lock(this->m_h245SignalBufferMutex);
		std::string buf(this->m_h245SignalPacket,this->m_h245SignalPacket + length);
		this->m_h245SignalBuffer.insert(this->m_h245SignalBuffer.end(), this->m_h245SignalPacket, this->m_h245SignalPacket + length);
	}

	this->io_service_.post(boost::bind(&CliSession::HandleH245SignelPacket, shared_from_this()));

	this->StartRead();
}

#include "boost/tokenizer.hpp"
void CliSession::HandleH245SignelPacket()
{
	{
		boost::mutex::scoped_lock lock(this->m_h245SignalBufferMutex);
		std::string sep("\n");
		int first0 = 0;
		int first1 = 0;
		unsigned int len = 0;
		while (std::string::npos != (first1 = this->m_h245SignalBuffer.find_first_of(sep, first0)))
		{
			boost::shared_ptr<std::string> buffer(new std::string(this->m_h245SignalBuffer.substr(first0, first1 + 1 - first0)));
			this->io_service_.post(boost::bind(&CliSession::HandleH245SignalMessage, shared_from_this(), buffer));
			len += first1 + 1- first0;
			first0 = first1 + 1;

		}
		this->m_h245SignalBuffer.erase(0,len);
	}
}

std::string StringReplace(const std::string& input, const std::string& find)
{
	int curPos = 0;
	int pos;
	std::string ou;
	const char * p = input.c_str();
	unsigned int l = find.size();
	while((pos = input.find(find, curPos)) != -1)
	{
		ou.insert(ou.end(), &p[curPos], &p[pos]);
		
		char a1 = input[pos + l];
		char a2 = input[pos + l + 1];
		char byt  = 0;
		if(a1 <= '9')
		{
			byt += (a1 - 0x30) * 16;
		}
		else
		{
			byt += (a1 - 0x37) * 16;
		}

		if(a2 <= '9')
		{
			byt += (a2 - 0x30);
		}
		else
		{
			byt += (a2 - 0x37);
		}

		ou.insert(ou.end(), &byt, &byt+1);
		curPos = pos + l + 2;
	}

	if(curPos < input.size() && curPos > 0)
	{
		ou.insert(ou.end(), &p[curPos], &p[input.size()]);
	}

	if(ou.empty())
	{
		ou += input;
	}
	return ou;
}
bool CliSession::HandleH245SignalMessage(boost::shared_ptr<std::string> buf)
{
	printf("RecvMessage: \n%s\n", buf->c_str());
	std::string buf1;
	std::string buf2;
	try
	{
		std::istringstream xmlnIStream;

		boost::property_tree::ptree pt;

		xmlnIStream.str(*buf);

		boost::property_tree::json_parser::read_json(xmlnIStream, pt);

		std::ostringstream xmlOutIStream;
		boost::property_tree::xml_parser1::write_xml(xmlOutIStream, pt);
		buf2 = xmlOutIStream.str();
		xmlnIStream.str(buf2);

		boost::property_tree::xml_parser::read_xml(xmlnIStream, pt, boost::property_tree::xml_parser::trim_whitespace);

		const char chflag = '/';

		boost::property_tree::ptree & message = pt.get_child(boost::property_tree::ptree::path_type("message", chflag));

		const std::string & function = message.get<std::string>("function");

		if(this->m_userId.empty())
		{
			if(function != "user_login")
			{
				printf("Register First, Command = %s\n", function.c_str());
				return false;
			}
		}
		std::string response;
		{
			bool ret = this->m_cc->Message(this->m_userId, buf2, response , boost::bind(&CliSession::SendMessage1, shared_from_this(), _1));
		}
	
		if(!response.empty())
		{
			std::istringstream xmlnIStream;
			boost::property_tree::ptree pt;
			xmlnIStream.str(response);
			boost::property_tree::xml_parser::read_xml(xmlnIStream, pt, boost::property_tree::xml_parser::trim_whitespace);

			const char chflag = '/';

			boost::property_tree::ptree & message = pt.get_child(boost::property_tree::ptree::path_type("message", chflag));

			const std::string & function = message.get<std::string>("function");

			if(function == "user_login")
			{
				//const std::string & status = message.get<std::string>("status");
				boost::property_tree::ptree &pstatus = message.get_child(boost::property_tree::ptree::path_type("status", chflag));
				std::string code = pstatus.get<std::string>("code");
				if(code == "0")
				{
					boost::property_tree::ptree & user = message.get_child(boost::property_tree::ptree::path_type("user", chflag));
					m_userId = user.get<std::string>("id");
					m_group = user.get<std::string>("group");
					///delete exists same users id session
					///handle single user login problem
					this->m_acceptor->HandleOneUserLogin(this->shared_from_this());
				}
			}
			
			std::ostringstream xmlOutIStream;
			boost::property_tree::json_parser1::write_json(xmlOutIStream, pt);
			std::string resp1 = xmlOutIStream.str();
			std::string resp2 = StringReplace(resp1, "\\u00");
			this->SendMessage(resp2);
			return true;
		}
	}
	catch (std::exception& e)
	{
		printf("Error parse json\n");
		return false;
	}
	return true;
}

bool CliSession::SendMessage(std::string &msg)
{
	std::string tmpmsg = msg;
	size_t nSend = 0;
	size_t len = msg.size();
	do{
		std::string str = tmpmsg.substr(nSend ,len-1);
		boost::system::error_code ec;
		try{
			size_t t = this->socket_.send(boost::asio::buffer(str), 0, ec);
			if(ec)
			{
				throw boost::system::error_code(ec);
			}
			nSend += t;	
		}catch(std::exception &e)
		{
			std::cerr << e.what() << std::endl;
			return false;
		}catch(boost::system::error_code &e)
		{
			std::cerr << e.message() << std::endl;
			return false;
		}
	}while(nSend < len);

	printf("SendMessage: \n%s\n", msg.c_str());

#ifdef DEBUG
	std::cout << "*****************************\n";
	std::cout << "msg len:" << len << std::endl;;
	std::cout << "Send len:" << nSend << std::endl;;
	std::cout << "*****************************\n";
#endif
	return !!nSend;
}
bool CliSession::SendMessage1(std::string &msg)
{	
	std::istringstream xmlnIStream;
	boost::property_tree::ptree pt;
	xmlnIStream.str(msg);
	boost::property_tree::xml_parser::read_xml(xmlnIStream, pt, boost::property_tree::xml_parser::trim_whitespace);
	std::ostringstream xmlOutIStream;
	boost::property_tree::json_parser1::write_json(xmlOutIStream, pt);
	std::string resp1 = xmlOutIStream.str();
	std::string resp2 = StringReplace(resp1, "\\u00");

	const char chflag = '/';
	boost::property_tree::ptree & message = pt.get_child(boost::property_tree::ptree::path_type("message", chflag));
	const std::string & function = message.get<std::string>("function");
	//const std::string & status = message.get<std::string>("status");
	boost::property_tree::ptree &pstatus = message.get_child(boost::property_tree::ptree::path_type("status", chflag));
	std::string code = pstatus.get<std::string>("code");
	if(
		(function == "link_add") && code == "0"
	 )
	{

	}
	this->SendMessage(resp2);
	return true;
}

bool CliSession::SendMessage2(std::string &msg)
{	
	std::istringstream xmlnIStream;
	boost::property_tree::ptree pt;
	xmlnIStream.str(msg);
	boost::property_tree::xml_parser::read_xml(xmlnIStream, pt, boost::property_tree::xml_parser::trim_whitespace);
	std::ostringstream xmlOutIStream;
	boost::property_tree::json_parser1::write_json(xmlOutIStream, pt);
	std::string resp1 = xmlOutIStream.str();
	std::string resp2 = StringReplace(resp1, "\\u00");
	this->SendMessage(resp2);
	return true;
}

bool CliSession::SendMessage(const std::string & userid, CliSession::pointer exclude, std::string &msg)
{
	return this->m_acceptor->SendMessage(userid, exclude, msg);
}
