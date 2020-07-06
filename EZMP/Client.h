#pragma once
#include "MPInterfacer.h"
class Client : public MPInterfacer
{
public:
	bool Login();

	MPInterfacer m_Interfacer;
};

