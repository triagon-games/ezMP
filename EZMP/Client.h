#pragma once
#include "MPInterfacer.h"
#include "pch.h"
class Client : public MPInterfacer
{
public:
	bool Login();

	MPInterfacer m_Interfacer;
};

