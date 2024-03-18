#include "ListenerHttp.hpp"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

namespace logging = boost::log;


using namespace std;
using namespace httplib;


ListenerHttp::ListenerHttp(const std::string& ip, int localPort, const nlohmann::json& config, bool isHttps)
	: Listener(ip, std::to_string(localPort), (isHttps==true) ? ListenerHttpsType : ListenerHttpType)
	, m_isHttps(isHttps)
	, m_config(config)
{	
#ifdef __linux__

	bool isPortInUse = port_in_use(localPort);
	if(isPortInUse)
		throw std::runtime_error("Port Already Used.");
		
#elif _WIN32
#endif

	m_host=ip;
	m_port=localPort;

	m_listenerHash = random_string(SizeListenerHash);
	m_listenerHash += "-";
	if(isHttps)
		m_listenerHash += ListenerHttpsType;
	else
		m_listenerHash += ListenerHttpType;
	m_listenerHash += "/";
	m_listenerHash += m_hostname;
	m_listenerHash += "/";
	m_listenerHash += ip;
	m_listenerHash += "/";
	m_listenerHash += std::to_string(localPort);

	if(m_isHttps)
	{
		try
    	{
			std::string servCrtFile = m_config[0]["ServHttpsListenerCrtFile"].get<std::string>();
			std::string servKeyFile = m_config[0]["ServHttpsListenerKeyFile"].get<std::string>();
			m_svr = std::make_unique<httplib::SSLServer>(servCrtFile.c_str(), servKeyFile.c_str());
		}
		catch (const json::out_of_range)
		{
			BOOST_LOG_TRIVIAL(fatal) << "No ServHttpsListenerCrtFile or ServHttpsListenerKeyFile in config.";
			return;
		}		
	}
	else
		m_svr = std::make_unique<httplib::Server>();

	this->m_httpServ = std::make_unique<std::thread>(&ListenerHttp::lauchHttpServ, this);
}


ListenerHttp::~ListenerHttp()
{
	m_svr->stop();
	m_httpServ->join();
}


void ListenerHttp::lauchHttpServ()
{
	httplib::Response res;

	json uri;
	try
	{
		uri = m_config[0]["uri"];
	}
	catch (const json::out_of_range)
	{
		BOOST_LOG_TRIVIAL(fatal) << "No uri in config.";
		return;
	}

	m_svr->set_post_routing_handler([&](const auto& req, auto& res) 
	{
		bool isUri = false;
		for (json::iterator it = uri.begin(); it != uri.end(); ++it)
			if(req.path ==*it)
				isUri=true;

		if ( isUri ) 
		{
			return Server::HandlerResponse::Unhandled;
		}
		else
		{
			BOOST_LOG_TRIVIAL(info) << "Unauthorized connection " << req.path;
			res.status = 401;
			return Server::HandlerResponse::Handled;
		}
	});

	if(m_isHttps==false)
		for (json::iterator it = uri.begin(); it != uri.end(); ++it)
			m_svr->Post(*it, [&](const auto& req, auto& res)
			{
				try 
				{

					BOOST_LOG_TRIVIAL(info) << "Connection " << req.path;
					this->HandleCheckIn(req, res);
					res.status = 200;
				} 
				catch(const std::exception& ex)
				{
					BOOST_LOG_TRIVIAL(info) << "Execption " << ex.what();
					res.status = 401;
				}
				catch (...) 
				{
					BOOST_LOG_TRIVIAL(info) << "Unknown failure occurred.";
					res.status = 401;
				}
			});

	if(m_isHttps==true)
		for (json::iterator it = uri.begin(); it != uri.end(); ++it)
			m_svr->Post(*it, [&](const auto& req, auto& res)
			{
				try 
				{
					BOOST_LOG_TRIVIAL(info) << "Connection " << req.path;
					this->HandleCheckIn(req, res);
					res.status = 200;
				} 
				catch(const std::exception& ex)
				{
					BOOST_LOG_TRIVIAL(info) << "Execption " << ex.what();
					res.status = 401;
				}
				catch (...) 
				{
					BOOST_LOG_TRIVIAL(info) << "Unknown failure occurred.";
					res.status = 401;
				}
			});

	m_svr->listen(m_host.c_str(), m_port);
}


int ListenerHttp::HandleCheckIn(const httplib::Request& req, httplib::Response& res)
{
	string input = req.body;

	BOOST_LOG_TRIVIAL(trace) << "m_isHttps " << std::to_string(m_isHttps);
	BOOST_LOG_TRIVIAL(trace) << "input.size " << std::to_string(input.size());

	string output;
	bool ret = handleMessages(input, output);


	json httpHeaders;
	try
	{
		httpHeaders = m_config[0]["server"][0]["headers"][0];
	}
	catch (const json::out_of_range)
	{
		BOOST_LOG_TRIVIAL(fatal) << "No server headers in config.";
		return -1;
	}

	httplib::Headers httpServerHeaders;
	for (auto& it : httpHeaders.items())
		httpServerHeaders.insert({(it).key(), (it).value()});
	res.headers = httpServerHeaders;

	BOOST_LOG_TRIVIAL(trace) << "output.size " << std::to_string(output.size());

	if(ret)
		res.body = output;
	else
		res.status = 200;

	return 0;
}
