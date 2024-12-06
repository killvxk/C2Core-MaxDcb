#pragma once

#include "ModuleCmd.hpp"


class Download : public ModuleCmd
{

public:
	Download();
	~Download();

	std::string getInfo();

	int init(std::vector<std::string>& splitedCmd, C2Message& c2Message);
	int process(C2Message& c2Message, C2Message& c2RetMessage);
	int followUp(const C2Message &c2RetMessage);
	int errorCodeToMsg(const C2Message &c2RetMessage, std::string& errorMsg);

private:

};


#ifdef _WIN32

extern "C" __declspec(dllexport) Download * DownloadConstructor();

#else

extern "C"  __attribute__((visibility("default"))) Download * DownloadConstructor();

#endif
