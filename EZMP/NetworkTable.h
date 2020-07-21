#pragma once
#include "pch.h"
#include "MPInterface.h"
class NetworkTable
{
public:
	NetworkTable();
	NetworkTable(MPInterfacer* mpi);

private:
	MPInterfacer* m_MPInterace;
};

