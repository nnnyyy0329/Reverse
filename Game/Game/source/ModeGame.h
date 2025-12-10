#include "appframe.h"
#include <string>

//Ç¢Ç¡ÇΩÇÒÇ±ÇÍ
class SurfacePlayer;

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

	//Ç¢Ç¡ÇΩÇÒÇ±ÇÍ
	std::shared_ptr<SurfacePlayer> _surfacePlayer;

}; 
