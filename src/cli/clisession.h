#pragma once

#include "boost/asio.hpp"
#include "boost/shared_ptr.hpp"
#include <boost/enable_shared_from_this.hpp>
#include "boost/weak_ptr.hpp"
#include "boost/thread.hpp"
#include "map"
#include "boost/function.hpp"

class CliAcceptor;
class ControlCenter;

class CliSession : public boost::enable_shared_from_this<CliSession>
{
	friend class CliAcceptor;
public:
	typedef boost::shared_ptr<CliSession> pointer;

	static pointer Create(unsigned int ip, CliAcceptor* acceptor, ControlCenter* cc)
	{
		return pointer(new CliSession(ip, acceptor, cc));
	};

public:
	CliSession(unsigned int ip, CliAcceptor * acceptor, ControlCenter* cc);
	~CliSession(void);

public:
	void Start(bool callout = false, unsigned int ip = 0,unsigned short port = 0);

	boost::asio::ip::tcp::socket& Socket();

private:
	boost::asio::io_service& io_service_;
	boost::asio::io_service::strand strand_;
	boost::asio::ip::tcp::socket socket_;

	ControlCenter *m_cc;
	CliAcceptor * m_acceptor;

	unsigned int m_localIp;

	unsigned char* m_h245SignalPacket;

	std::string m_h245SignalBuffer;
	boost::mutex m_h245SignalBufferMutex;

	boost::mutex m_h245Decode;

	std::string m_userId;
	std::string m_group;

	boost::mutex m_processMutex;
	//boost::recursive_mutex m_processMutex;
private:
	
	//boost::asio::ip::tcp::socket& Socket();
	void StartRead();

	void HandleRead(const boost::system::error_code& err, size_t length);
	void HandleH245SignelPacket();

	bool HandleH245SignalMessage(boost::shared_ptr<std::string> buf);

	bool SendMessage(std::string &msg);
	bool SendMessage1(std::string &msg);
	bool SendMessage2(std::string &msg);
	bool SendMessage(const std::string & userid, CliSession::pointer exclude, std::string &msg);

	std::string UserId(){return this->m_userId;};

};

class ControlCenter;
class CliAcceptor
{
public:
	CliAcceptor(unsigned int ip, ControlCenter *cc);
	~CliAcceptor(void);

public:
	bool GetH245Address(unsigned int &ip, unsigned short &port);
	bool SendMessage(const std::string & userid, CliSession::pointer exclude, std::string &msg);
	bool SendMessage1(std::string &msg);
	bool GetOnLineUsers(CliSession::pointer exclude, std::vector<std::string> &users);
	bool HandleOneUserLogin(CliSession::pointer exclude);

private:
	void StartAccept();
	void HandleAccept(CliSession::pointer new_session, const boost::system::error_code& err);
	
private:
	boost::asio::io_service& m_io_service;
	boost::asio::ip::tcp::acceptor m_CliAcceptor;

	unsigned int m_acceptIp;

	ControlCenter *m_cc;

	boost::recursive_mutex m_mutex;
	std::vector<boost::weak_ptr<CliSession> > m_sessions;
};

