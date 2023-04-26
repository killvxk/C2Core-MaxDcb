#pragma once

#include "ModuleCmd.hpp"

#ifdef _WIN32
	#include <Windows.h>
#endif


class AssemblyExec : public ModuleCmd
{

public:
	AssemblyExec();
	~AssemblyExec();

	std::string getInfo();

	int init(std::vector<std::string>& splitedCmd, C2Message& c2Message);
	int process(C2Message& c2Message, C2Message& c2RetMessage);

private:
#ifdef _WIN32
	int createNewProcess(const std::string& payload, std::string& result);
	int createNewThread(const std::string& payload, std::string& result);

	bool m_isProcessRuning;
	HANDLE m_processHandle;
	int killProcess();
#endif

};

#ifdef _WIN32

extern "C" __declspec(dllexport) AssemblyExec * A_AssemblyExecConstructor();

#endif
