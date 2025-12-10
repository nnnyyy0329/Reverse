
#include "appframe.h"

#include <string>


// ÉÇÅ[Éh
class ModeGame : public ModeBase
{
	typedef ModeBase base;
public:
	virtual bool Initialize();
	virtual bool Terminate();
	virtual bool Process();
	virtual bool Render();

protected:


}; 
