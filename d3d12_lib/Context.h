#pragma once

#include "Device.h"
#include "CommandQueueManager.h"

class Context
{
public:
	
	Context();
	~Context() = default;
	void Test();

private:

	std::shared_ptr<Device> m_Device;
	std::shared_ptr<CommandQueueManager> m_CommandQueueManager;

};

