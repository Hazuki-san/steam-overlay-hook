#pragma once
#include "Includes.h"

class SteamOverlaySetup
{
public:
	void* m_presentTarget;
	void* m_resizeBuffersTarget;

public:
	bool Setup();
	bool Hook();
};