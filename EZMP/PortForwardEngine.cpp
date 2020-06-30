#include "PortForwardEngine.h"

void PortForwardEngine::UPnPportForward(long internalPort, long externalPort)
{
	CoInitialize(NULL);

	BSTR clientName = _com_util::ConvertStringToBSTR("192.168.3.1");
	BSTR protocol = _com_util::ConvertStringToBSTR("UDP");
	BSTR serviceName = _com_util::ConvertStringToBSTR("UDP");

	IUPnPNAT* nat = NULL;
	IStaticPortMappingCollection* mappingCollection = NULL;
	IStaticPortMapping* staticMapping = NULL;

	printf("%lu\n", HRESULT_CODE(CoCreateInstance(__uuidof(UPnPNAT), NULL, CLSCTX_INPROC_SERVER, __uuidof(IUPnPNAT), (void**)&nat)));

	printf("%lu\n", HRESULT_CODE(nat->get_StaticPortMappingCollection(&mappingCollection)));

	printf("%lu\n", HRESULT_CODE(mappingCollection->Add(10001, protocol, 10001, clientName, VARIANT_TRUE, serviceName, &staticMapping)));

	staticMapping->Release();
	staticMapping = NULL;
	mappingCollection->Release();
	mappingCollection = NULL;
	nat->Release();
	nat = NULL;

	CoUninitialize();
}
