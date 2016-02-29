#pragma once

#include "boost/noncopyable.hpp"
#include "boost/asio.hpp"
#include "boost/thread.hpp"
#include "boost/bind.hpp"

class IoServer0 : private boost::noncopyable
{
private:
	// Structure to perform the actual initialisation.
	struct do_init
	{
		do_init(unsigned int threadnum)
			:m_worker(m_ioService),m_barrier(threadnum+1)
		{
			if(threadnum == 0) 
			{
				threadnum = 1;
			}
			for (std::size_t i = 0; i < threadnum; i++)
			{
				this->m_flags[i] = -1000;
				boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&do_init::WorkThread, this, i)));
				m_threads.push_back(thread);
			}
		};

		~do_init()
		{
			this->m_ioService.stop();
			this->m_barrier.wait();
		};
		
		boost::asio::io_service & getios()
		{
			return this->m_ioService;
		};

		// Helper function to manage a do_init singleton. The static instance of the
		// winsock_init object ensures that this function is always called before
		// main, and therefore before any other threads can get started. The do_init
		// instance must be static in this function to ensure that it gets
		// initialised before any other global objects try to use it.
		static boost::shared_ptr<do_init> instance(unsigned int threadnum)
		{
			static boost::shared_ptr<do_init> init(new do_init(threadnum));
			return init;
		};

		void WorkThread(unsigned int index)
		{
			printf("Thread Pool : Thread %d Started.\n",index);
			this->m_flags[index] = 1000;
			this->m_ioService.run();
			printf("Thread Pool : Thread %d Stopped.\n",index);
			this->m_barrier.wait();
		}
		bool IsStarted()
		{
			bool ret = true;
			std::map<unsigned int, int>::iterator iter = this->m_flags.begin();
			for(; iter != this->m_flags.end(); iter++)
			{
				ret = ret & (iter->second == 1000);
			}
			return ret;
		};
	private:
		boost::asio::io_service m_ioService;
		boost::asio::io_service::work m_worker;
		std::vector<boost::shared_ptr<boost::thread> > m_threads;
		std::map<unsigned int, int> m_flags;
		boost::barrier m_barrier;
	};

public:
	// Constructor.
	IoServer0(unsigned int threadnum = 2)
		:ref_(do_init::instance(threadnum))
	{
		//printf("IoServer0 Started.\n");
		// Check whether winsock was successfully initialised. This check is not
		// performed for the global instance since there will be nobody around to
		// catch the exception.
		//if (this != &instance_)
		//{
		//	boost::throw_exception();
		//}
	};

	// Destructor.
	~IoServer0()
	{
	};

	boost::asio::io_service & GetIoService()
	{
		return ref_->getios();
	};
	 
	bool IsStarted()
	{
		return ref_->IsStarted();
	};
private:
	// Reference to singleton do_init object to ensure that winsock does not get
	// cleaned up until the last user has finished with it.
	boost::shared_ptr<do_init> ref_;
};


