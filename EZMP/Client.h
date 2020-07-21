#pragma once
#include "MPInterface.h"
#include "pch.h"
class Client : public MPInterfacer
{
public:
	bool Login();

	MPInterfacer m_Interfacer;
};

