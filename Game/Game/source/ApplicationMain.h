
#include "appframe.h"


class ApplicationMain : public ApplicationBase
{
	typedef ApplicationBase base;
public:
	virtual bool Initialize(HINSTANCE hInstance);
	virtual bool Terminate();
	virtual bool Input();
	virtual bool Process();
	virtual bool Render();

	virtual bool AppWindowed() { return true; }
	virtual int DispSizeW() { return 1280; }
	virtual int DispSizeH() { return 720; }

protected:

}; 
