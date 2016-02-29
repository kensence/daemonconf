#include "Device.h"

#include <stdio.h>
#include "IoServer.h"

int count = 0;

IoServer0 io_server(20);

Device::Device(const std::string &name)
:m_name(name)
{
}


Device::~Device(void)
{
}
