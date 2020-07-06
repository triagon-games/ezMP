#include "Utils.h"

std::string exec(const char* cmd)
{
	std::array<char, 128> buffer;
	std::string result;
	std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
	if (!pipe)
	{
		throw std::runtime_error("popen() failed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
	{
		result += buffer.data();
	}
	return result;
}

std::string ExePath()
{
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
	std::wstring ws = std::wstring(buffer).substr(0, pos);
	return std::string(ws.begin(), ws.end());
}

sockaddr_in Utils::MakeAddress(std::string IP, uint16_t Port)
{
	sockaddr_in sai;
	if (IP != "any")
	{
		inet_pton(AF_INET, (PSTR)IP.c_str(), &sai.sin_addr);
	}
	else
	{
		sai.sin_addr.S_un.S_addr = ADDR_ANY;
	}
	sai.sin_port = htons(Port);
	sai.sin_family = AF_INET;
	return sai;
}

std::string Utils::ByteAddrToString(uint8_t* addr)
{
	std::string ip;
	for (int i = 0; i < 3; i++) ip += std::to_string(addr[i]) + '.';
	ip += std::to_string(addr[3]);
	return ip;
}

Utils::PortTranslation Utils::getPortTranslation(uint16_t Port)
{
	std::string path = ExePath();
	printf(path.c_str());
	std::string command(255, ' ');
	snprintf((char*)command.c_str(), 255, "%s%sstunclient.exe %s --localport %i", path.c_str(), "\\Stun\\", PUBLIC_STUN_SERVER, Port);
	std::string stunReturn = exec(command.c_str());
	std::string Ports[2];
	//Ports[0] = 
	bool found = false;
	int finds = -1;
	char test = 92;
	stunReturn.erase(std::remove(stunReturn.begin(), stunReturn.end(), ' '), stunReturn.end());
	for (int i = 1; i < stunReturn.size()-1; i++)
	{
		if (stunReturn.c_str()[i] == ':' && std::isdigit(stunReturn.c_str()[i - 1]) && std::isdigit(stunReturn.c_str()[i + 1]))
		{
			found = true;
			finds++;
		}
		if (stunReturn.c_str()[i+1] == '\n')
		{
			printf("Found \\n");
			found = false;
		}
		if (found) 
			Ports[finds].push_back(((char*)stunReturn.c_str())[i+1]);
	}
	PortTranslation pt(1, 0);
	try { pt = PortTranslation(std::stoi(Ports[0]), std::stoi(Ports[1])); }
	catch (std::exception) { }

	return pt;
}