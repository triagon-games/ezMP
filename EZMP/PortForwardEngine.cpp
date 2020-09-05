#include "PortForwardEngine.h"

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

bool PortForwardEngine::UPnPportForward(long internalPort, long externalPort)
{
	try
	{
		CoInitialize(NULL);

		std::vector<std::string> gateways;
		PIP_ADAPTER_ADDRESSES IPADINF;
		ULONG outBufLen = 15000;
		IPADINF = (IP_ADAPTER_ADDRESSES*)MALLOC(outBufLen);
		GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_GATEWAYS, NULL, IPADINF, &outBufLen);
		std::string used_IP = "";
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
				std::string ping_string = Utils::exec(std::string("ping ").append(Utils::getStringFromIP(&addr[0]).c_str()).append(" -n 1 -w 10").c_str());
				if (ping_string.find("Request timed out") == std::string::npos)
				{
					std::vector<uint8_t> Localaddr;
					Localaddr.push_back(IPADINF->FirstUnicastAddress->Address.lpSockaddr->sa_data[2]);
					Localaddr.push_back(IPADINF->FirstUnicastAddress->Address.lpSockaddr->sa_data[3]);
					Localaddr.push_back(IPADINF->FirstUnicastAddress->Address.lpSockaddr->sa_data[4]);
					Localaddr.push_back(IPADINF->FirstUnicastAddress->Address.lpSockaddr->sa_data[5]);
					used_IP = Utils::ByteAddrToString(&Localaddr[0]);
					break;
				}
				FGA = FGA->Next;
			}
			if (!used_IP.empty()) break;
			IPADINF = IPADINF->Next;
		}

		if (used_IP.empty()) return false;
		BSTR clientName = _com_util::ConvertStringToBSTR(used_IP.c_str());
		BSTR protocol = _com_util::ConvertStringToBSTR("UDP");
		BSTR serviceName = _com_util::ConvertStringToBSTR("EZMP");

		IUPnPNAT* nat = NULL;
		IStaticPortMappingCollection* mappingCollection = NULL;
		IStaticPortMapping* staticMapping = NULL;

		printf("%lu\n", HRESULT_CODE(CoCreateInstance(__uuidof(UPnPNAT), NULL, CLSCTX_INPROC_SERVER, __uuidof(IUPnPNAT), (void**)&nat)));
		if (nat == NULL) DEBUG::printVerboseDebug("UPnP NAT discovery failed", 2);
		else printf("%lu\n", HRESULT_CODE(nat->get_StaticPortMappingCollection(&mappingCollection)));
		if (mappingCollection == NULL) DEBUG::printVerboseDebug("UPnP mapping collection discovery failed", 2);
		else printf("%lu\n", HRESULT_CODE(mappingCollection->Add(internalPort, protocol, externalPort, clientName, VARIANT_TRUE, serviceName, &staticMapping)));
		if (staticMapping == NULL) DEBUG::printVerboseDebug("UPnP port mapping creation failed", 2);
		else
		{
			staticMapping->Release();
			staticMapping = NULL;
			mappingCollection->Release();
			mappingCollection = NULL;
			nat->Release();
			nat = NULL;

			CoUninitialize();
			return true;
		}
		return false;
	}
	catch (std::exception ex)
	{
		return false;
	}
}

bool PortForwardEngine::UPnPRemoveMapping(long externalPort)
{
	try
	{
		CoInitialize(NULL);
		BSTR protocol = _com_util::ConvertStringToBSTR("UDP");

		IUPnPNAT* nat = NULL;
		IStaticPortMappingCollection* mappingCollection = NULL;
		IStaticPortMapping* staticMapping = NULL;

		printf("%lu\n", HRESULT_CODE(CoCreateInstance(__uuidof(UPnPNAT), NULL, CLSCTX_INPROC_SERVER, __uuidof(IUPnPNAT), (void**)&nat)));

		printf("%lu\n", HRESULT_CODE(nat->get_StaticPortMappingCollection(&mappingCollection)));

		printf("%lu\n", HRESULT_CODE(mappingCollection->Remove(externalPort, protocol)));

		staticMapping->Release();
		staticMapping = NULL;
		mappingCollection->Release();
		mappingCollection = NULL;
		nat->Release();
		nat = NULL;

		CoUninitialize();
		return true;
	}
	catch (std::exception ex)
	{
		return false;
	}
}

