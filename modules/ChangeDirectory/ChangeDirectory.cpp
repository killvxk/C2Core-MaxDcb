#include "ChangeDirectory.hpp"

#include <cstring>
#include <array>
#include <filesystem>

using namespace std;


const std::string moduleName = "cd";


#ifdef _WIN32

__declspec(dllexport) ChangeDirectory* ChangeDirectoryConstructor() 
{
    return new ChangeDirectory();
}

#endif

ChangeDirectory::ChangeDirectory()
	: ModuleCmd(moduleName)
{
}

ChangeDirectory::~ChangeDirectory()
{
}

std::string ChangeDirectory::getInfo()
{
	std::string info;
	info += "cd:\n";
	info += "ChangeDirectory\n";
	info += "exemple:\n";
	info += "- cd /tmp\n";

	return info;
}

int ChangeDirectory::init(std::vector<std::string> &splitedCmd, C2Message &c2Message)
{
    string path;
    for (int idx = 1; idx < splitedCmd.size(); idx++) 
    {
        if(!path.empty())
            path+=" ";
        path+=splitedCmd[idx];
    }

	c2Message.set_instruction(splitedCmd[0]);
	c2Message.set_cmd(path);

	return 0;
}


int ChangeDirectory::process(C2Message &c2Message, C2Message &c2RetMessage)
{
	string path = c2Message.cmd();
	std::string outCmd = changeDirectory(path);

	c2RetMessage.set_instruction(m_name);
	c2RetMessage.set_cmd(path);
	c2RetMessage.set_returnvalue(outCmd);

	return 0;
}


std::string ChangeDirectory::changeDirectory(const std::string& path)
{
	std::error_code ec;
    try
    {
        if(!path.empty())
            std::filesystem::current_path(path, ec); 
    } catch (...) 
    {
    }

    std::string result;
    result=std::filesystem::current_path(ec).string();

	return result;
}