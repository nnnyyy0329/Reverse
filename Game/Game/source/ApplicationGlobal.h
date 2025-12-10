#pragma once
#include "appframe.h"

class ApplicationGlobal
{
public:
	ApplicationGlobal();
	virtual ~ApplicationGlobal();

	bool Init();

	int _cgCursor;
};

extern ApplicationGlobal gGlobal;

