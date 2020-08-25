#include "PortForwardEngine.h"

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

bool PortForwardEngine::UPnPportForward(long internalPort, long externalPort)
{
	CoInitialize(NULL);

	std::vector<std::string> gateways;
	PIP_ADAPTER_ADDRESSES IPADINF;
	ULONG outBufLen = 15000;
	IPADINF = (IP_ADAPTER_ADDRESSES*)MALLOC(outBufLen);
	GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_GATEWAYS, NULL, IPADINF, &outBufLen);
	while (IPADINF)
	{
		PIP_ADAPTER_GATEWAY_ADDRESS_LH FGA = IPADINF->FirstGatewayAddress;
		while (FGA)
		{
			std::vector<uint8_t> addr;
			addr.push_back(FGA->Address.lpSockaddr->sa_data[2]);
			addr.push_back(FGA->Address.lpSockaddr->sa_data[3]);
			addr.push_back(FGA->Address.lpSockaddr->sa_data[4]);
			addr.push_back(FGA->Address.lpSockaddr->sa_data[5]);
			gateways.push_back(Utils::getStringFromIP(&addr[0]));
			FGA = FGA->Next;
		}
		IPADINF = IPADINF->Next;
	}

	std::string used_IP = "";
	for (int i = 0; i < gateways.size(); i++)
	{
		std::string ping_string = Utils::exec(std::string("ping ").append(gateways[i].c_str()).append(" -n 1 -w 10").c_str());
		if (ping_string.find("Request timed out") == std::string::npos)
		{
			used_IP = (gateways[i].c_str());
		}
	}

	if (used_IP.empty()) return false;
	BSTR clientName = _com_util::ConvertStringToBSTR(used_IP.c_str());
	BSTR protocol = _com_util::ConvertStringToBSTR("UDP");
	BSTR serviceName = _com_util::ConvertStringToBSTR("UDP");

	IUPnPNAT* nat = NULL;
	IStaticPortMappingCollection* mappingCollection = NULL;
	IStaticPortMapping* staticMapping = NULL;

	printf("%lu\n", HRESULT_CODE(CoCreateInstance(__uuidof(UPnPNAT), NULL, CLSCTX_INPROC_SERVER, __uuidof(IUPnPNAT), (void**)&nat)));

	printf("%lu\n", HRESULT_CODE(nat->get_StaticPortMappingCollection(&mappingCollection)));

	printf("%lu\n", HRESULT_CODE(mappingCollection->Add(internalPort , protocol, externalPort, clientName, VARIANT_TRUE, serviceName, &staticMapping)));

	staticMapping->Release();
	staticMapping = NULL;
	mappingCollection->Release();
	mappingCollection = NULL;
	nat->Release();
	nat = NULL;

	CoUninitialize();
	return true;
}
