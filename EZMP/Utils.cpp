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

Utils::PortTranslation Utils::getPortTranslation(uint16_t Port, std::string stunServer)
{
	std::string path = ExePath();
	printf(path.c_str());
	std::string command(255, ' ');
	snprintf((char*)command.c_str(), 255, "%s%sstunclient.exe %s --localport %i", path.c_str(), "\\Stun\\", stunServer.c_str(), Port);
	std::string stunReturn = exec(command.c_str());
	std::string Ports[2];
	bool found = false;
	int finds = -1;
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
			//printf("Found \\n");
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

void Utils::getPublicIPAddress(std::string stunServer, uint8_t* ipAddr)
{
	std::string path = ExePath();
	printf(path.c_str());
	std::string command(255, ' ');
	snprintf((char*)command.c_str(), 255, "%s%sstunclient.exe %s --localport %i", path.c_str(), "\\Stun\\", stunServer.c_str(), 20000);
	std::string stunReturn = exec(command.c_str());
	unsigned int firstIndex = stunReturn.find("Mapped address: ", 0) + 16;
	std::string ip = "";
	bool foundDelim = false;
	std::string ipBytes[4];
	ipAddr = (uint8_t*)malloc(4);
	if (ipAddr == nullptr) throw std::exception("ipAddr malloc failed line: %i", __LINE__);

	for (int i = firstIndex; i < stunReturn.size(); i++)
	{
		if (stunReturn[i] == ':') foundDelim = true;
		
		if (!foundDelim) ip.push_back(stunReturn[i]);
	}
	int finds = 0;
	for (int i = 0; i < ip.size(); i++)
	{
		if (ip[i] == '.')
		{
			finds++;
		}
		else
		{
			ipBytes[finds].push_back(ip[i]);
		}
	}
	for (int i = 0; i < 4; i++)
	{
		ipAddr[i] = std::stoi(ipBytes[i]);
	}
}

void Utils::getIPFromString(std::string ip, uint8_t* _ipAddr)
{
	int finds = 0;
	uint8_t ipAddr[4];
	std::string ipBytes[4];
	_ipAddr = (uint8_t*)malloc(4);
	if (ipAddr == nullptr) throw std::exception("ipAddr malloc failed line: %i", __LINE__);

	for (int i = 0; i < ip.size(); i++)
	{
		if (ip[i] == '.')
		{
			finds++;
		}
		else
		{
			ipBytes[finds].push_back(ip[i]);
		}
	}
	for (int i = 0; i < 4; i++)
	{
		ipAddr[i] = std::stoi(ipBytes[i]);
	}
	memcpy(_ipAddr, ipAddr, 4);
}

std::string Utils::getStringFromIP(uint8_t* ip)
{
	std::string ipStr;
	for (int i = 0; i < 3; i++) ipStr += std::to_string(ip[i]) + '.';
	ipStr += std::to_string(ip[3]);
	return ipStr;
}

std::string Utils::stringFromBytes(uint8_t* bytes, uint32_t size)
{
	std::string ret;
	for (size_t i = 0; i < size; i++)
	{
		ret.push_back(bytes[i]);
	}
	return ret;
}

uint8_t* Utils::bytesFromString(std::string string, uint32_t* p_size)
{
	uint8_t* ret = (uint8_t*)&string[0];
	memset(p_size, (uint32_t)string.size(), sizeof(uint32_t));
	return ret;
}

std::vector<uint8_t> Utils::vectorBytesFromString(std::string string)
{
	std::vector<uint8_t> ret;
	for (int i = 0; i < string.size(); i++)
	{
		ret.push_back(string[i]);
	}
	return ret;
}

std::string Utils::stringFromVectorBytes(std::vector<uint8_t> bytes)
{
	std::string ret;
	for (int i = 0; i < bytes.size(); i++)
	{
		ret.push_back(bytes[i]);
	}
	return ret;
}
