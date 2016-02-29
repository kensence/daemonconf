#include "Cnw8302.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

Cnw8302::Cnw8302(const std::string &name):Device(name)
{

}

Cnw8302::~Cnw8302(void)
{

}

bool Cnw8302::SetDevName(std::string &name)
{
	char buf[512] = {0};
	std::string cmd = "fw_setenv SERIAL_NUMBER ";
	cmd += name;
	FILE *fp = NULL;
	if((fp=popen(cmd.c_str(), "r")) != NULL)
	{
		while(fgets(buf, 512, fp) != NULL)
		{
			fprintf(stdout,"%s", buf);
		}
		pclose(fp);
	}
	else
	{
		fprintf(stderr, "popen %s error/n", cmd.c_str());
		return false;
	}
	return true;
}

bool Cnw8302::GetDevName(std::string &result)
{
	char buf[512]={0};
	std::string cmd = "fw_printenv -n SERIAL_NUMBER ";
	FILE *fp = NULL;
	if((fp=popen(cmd.c_str(), "r")) != NULL)
	{
		while(fgets(buf, 512, fp) != NULL)
		{
			fprintf(stdout,"%s", buf);
			result += buf;
		}
		pclose(fp);
	}
	else
	{
		fprintf(stderr, "popen %s error/n", cmd.c_str());
		return false;
	}
	return true;
}

bool Cnw8302::SetDevIP(std::string &ip)
{
	char buf[512] = {0};
	std::string cmd = "fw_setenv IP_ADDRESS ";
	cmd += ip;
	FILE *fp = NULL;
	if((fp=popen(cmd.c_str(), "r")) != NULL)
	{
		while(fgets(buf, 512, fp) != NULL)
		{
			fprintf(stdout,"%s", buf);
		}
		pclose(fp);
	}
	else
	{
		fprintf(stderr, "popen %s error/n", cmd.c_str());
		return false;
	}
	return true;
}

bool Cnw8302::GetDevIP(std::string &result)
{
	char buf[512]={0};
	std::string cmd = "fw_printenv -n IP_ADDRESS ";
	FILE *fp = NULL;
	if((fp=popen(cmd.c_str(), "r")) != NULL)
	{
		while(fgets(buf, 512, fp) != NULL)
		{
			fprintf(stdout,"%s", buf);
			result += buf;
		}
		pclose(fp);
	}
	else
	{
		fprintf(stderr, "popen %s error/n", cmd.c_str());
		return false;
	}
	return true;
}

bool Cnw8302::SetDevMac(std::string &mac)
{
	char buf[512] = {0};
	std::string cmd = "fw_setenv ethaddr ";
	cmd += mac;
	FILE *fp = NULL;
	if((fp=popen(cmd.c_str(), "r")) != NULL)
	{
		while(fgets(buf, 512, fp) != NULL)
		{
			fprintf(stdout,"%s", buf);
		}
		pclose(fp);
	}
	else
	{
		fprintf(stderr, "popen %s error/n", cmd.c_str());
	}
	return true;
}

bool Cnw8302::GetDevMac(std::string &result)
{
	char buf[512]={0};
	std::string cmd = "fw_printenv -n ethaddr ";
	FILE *fp = NULL;
	if((fp=popen(cmd.c_str(), "r")) != NULL)
	{
		while(fgets(buf, 512, fp) != NULL)
		{
			fprintf(stdout,"%s", buf);
			result += buf;
		}
		pclose(fp);
	}
	else
	{
		fprintf(stderr, "popen %s error/n", cmd.c_str());
		return false;
	}
	return true;
}

bool Cnw8302::SetDevSrcIP(std::string &srcip)
{
	char buf[512] = {0};
	std::string cmd = "fw_setenv SOURCE_IP ";
	cmd += srcip;
	FILE *fp = NULL;
	if((fp=popen(cmd.c_str(), "r")) != NULL)
	{
		while(fgets(buf, 512, fp) != NULL)
		{
			fprintf(stdout,"%s", buf);
		}
		pclose(fp);
	}
	else
	{
		fprintf(stderr, "popen %s error/n", cmd.c_str());
		return false;
	}
	return true;
}

bool Cnw8302::GetDevSrcIp(std::string &result)
{
	char buf[512]={0};
	std::string cmd = "fw_printenv -n SOURCE_IP ";
	FILE *fp = NULL;
	if((fp=popen(cmd.c_str(), "r")) != NULL)
	{
		while(fgets(buf, 512, fp) != NULL)
		{
			fprintf(stdout,"%s", buf);
			result += buf;
		}
		pclose(fp);
	}
	else
	{
		fprintf(stderr, "popen %s error/n", cmd.c_str());
		return false;
	}
	return true;
}

bool Cnw8302::SetDevBitRate(std::string &bitrate)
{
	char buf[512] = {0};
	std::string cmd = "fw_setenv BITRATE ";
	cmd += bitrate;
	FILE *fp = NULL;
	if((fp=popen(cmd.c_str(), "r")) != NULL)
	{
		while(fgets(buf, 512, fp) != NULL)
		{
			fprintf(stdout,"%s", buf);
		}
		pclose(fp);
	}
	else
	{
		fprintf(stderr, "popen %s error/n", cmd.c_str());
		return false;
	}
	return true;
}

bool Cnw8302::GetDevBitRate(std::string &result)
{
	char buf[512]={0};
	std::string cmd = "fw_printenv -n BITRATE ";
	FILE *fp = NULL;
	if((fp=popen(cmd.c_str(), "r")) != NULL)
	{
		while(fgets(buf, 512, fp) != NULL)
		{
			fprintf(stdout,"%s", buf);
			result += buf;
		}
		pclose(fp);
	}
	else
	{
		fprintf(stderr, "popen %s error/n", cmd.c_str());
		return false;
	}
	return true;
}
