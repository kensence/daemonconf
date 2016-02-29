#include <boost/program_options/parsers.hpp>
#include <signal.h>
#include "ControlCenter.h"

#define TESTCLI 1

bool flag1 = true;
bool flag2 = true;

void fun1(int arg)
{
	flag1 = false;
}
void fun2(int arg)
{
	flag2 = false;
}
int main(int argc, char* argv[])
{
	signal(SIGINT,fun1);
	signal(SIGTERM,fun2);

	ControlCenter * cc = new ControlCenter();
	cc->Init();
	while(flag1&&flag2)
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
	}
	delete cc;
	return 0;
}
