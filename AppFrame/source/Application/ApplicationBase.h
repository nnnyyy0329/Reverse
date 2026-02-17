
#include "dxlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../Mode//ModeServer.h"
#include "EffekseerForDXLib.h"

class ApplicationBase
{
public:
	ApplicationBase();
	virtual ~ApplicationBase();

	virtual bool Initialize(HINSTANCE hInstance);
	virtual bool Terminate();
	virtual bool Input();
	virtual bool Process();
	virtual bool Render();

	virtual bool AppWindowed() { return true; }
	virtual int DispSizeW() { return 1920; }
	virtual int DispSizeH() { return 1080; }

	static	ApplicationBase	*GetInstance() { return _lpInstance; }
	virtual int GetKey() { return _gKey; }
	virtual int GetTrg() { return _gTrg; }

	// アナログスティック対応用
	struct AnalogStickState {
		float lx = 0.0f;
		float ly = 0.0f;
		float lz = 0.0f;
		float rx = 0.0f;
		float ry = 0.0f;
		float rz = 0.0f;
	};
	virtual AnalogStickState GetAnalog() const { return _analog; }
	virtual float GetAnalogMin() const { return _analogMin; }



protected:
	static	ApplicationBase	*_lpInstance;

	int		_gKey, _gTrg;

	// アナログスティック対応用
	AnalogStickState _analog;
	float _analogMin = 0.3f;// アナログ閾値

	ModeServer* _serverMode;

};
