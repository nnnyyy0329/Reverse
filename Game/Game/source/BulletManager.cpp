#include "BulletManager.h"

// ƒVƒ“ƒOƒ‹ƒgƒ“—pƒƒ“ƒo‰Šú‰»
BulletManager* BulletManager::_instance = nullptr;

void BulletManager::CreateInstance()
{
	if(_instance == nullptr)
	{
		_instance = new BulletManager();	// ƒCƒ“ƒXƒ^ƒ“ƒXì¬
		_instance->Initialize();			// ‰Šú‰»
	}
}

void BulletManager::DestroyInstance()
{
	if(_instance != nullptr)
	{
		delete _instance;		// ƒCƒ“ƒXƒ^ƒ“ƒX”jŠü
		_instance = nullptr;	// ƒCƒ“ƒXƒ^ƒ“ƒX‰Šú‰»
	}
}

BulletManager::BulletManager() 
{

}

BulletManager::~BulletManager() 
{
	// ‚·‚×‚Ä‚Ì’e‚ğíœ
	ClearAllBullets(GetAllBullets());
}

void BulletManager::Initialize()
{
	// ’†g‚Ì‰Šú‰»
	_registerBullets.clear();
}

void BulletManager::Process()
{
	// “o˜^‚³‚ê‚½’e‚ÌXV
	UpdateBullet();
}

void BulletManager::Render() 
{
	// “o˜^‚³‚ê‚½’e‚Ì•`‰æ
	RenderBullet();
}

void BulletManager::DebugRender()
{
	int registeredCount = static_cast<int>(_registerBullets.size());	// ’e‚Ì”

	// “o˜^‚³‚ê‚½’e‚Ì”‚Ì•`‰æ
	DrawFormatString(10, 250, GetColor(255, 255, 255), "Registered Bullets: %d", registeredCount);

	// ƒRƒŠƒWƒ‡ƒ“•`‰æ
	CollisionRender();
}

void BulletManager::CollisionRender()
{
	// “o˜^‚³‚ê‚½’e‚ÌƒRƒŠƒWƒ‡ƒ“•`‰æ
	for(auto& bulletInfo : _registerBullets)
	{
		if(!bulletInfo.bullet) { continue; }	// nullptrƒ`ƒFƒbƒN

		// —LŒø‚È‚çƒRƒŠƒWƒ‡ƒ“•`‰æ
		bulletInfo.bullet->CollisionRender();
	}
}


/* ’e”­ËŠÖ˜A */

void BulletManager::UpdateBullet()
{
	// “o˜^‚³‚ê‚½’e‚ÌXV
	for(auto& bulletInfo : _registerBullets)
	{
		if(!bulletInfo.bullet) { continue; }	// nullptrƒ`ƒFƒbƒN
		
		// —LŒø‚È‚çXV
		bulletInfo.bullet->Process();
	}

	// –³Œø‚È’e‚ÌƒNƒŠ[ƒ“ƒAƒbƒv
	CleanupInvalidBullets();
}

void BulletManager::RenderBullet()
{
	// “o˜^‚³‚ê‚½’e‚Ì•`‰æ
	for(auto& bulletInfo : _registerBullets)
	{
		if(!bulletInfo.bullet) { continue; }	// nullptrƒ`ƒFƒbƒN

		// —LŒø‚È‚ç•`‰æ
		bulletInfo.bullet->Render();
	}
}

std::shared_ptr<Bullet> BulletManager::Shoot(const BulletConfig& bulletConfig, const BulletEffectConfig& bulletEffectConfig, BULLET_OWNER_TYPE ownerType)
{
	// ’e‚ğ¶¬
	auto bullet = std::make_shared<Bullet>();

	// BulletConfig‚ğƒRƒs[‚µ‚ÄŠ—LÒ‚ğƒLƒƒƒ‰ƒ^ƒCƒv‚É•ÏŠ·
	BulletConfig modifiedConfig = bulletConfig;
	modifiedConfig.shooterType = ConvertOwnerTypeToCharType(ownerType);	// •ÏŠ·

	// ’e‚ğ”­Ë
	bullet->ActivateBullet(modifiedConfig, bulletEffectConfig);

	// ’e‚ğ“o˜^
	RegisterBullet(bullet, ownerType);

	return bullet;
}

std::shared_ptr<Bullet> BulletManager::ShootSimple(const BulletConfig& bulletConfig, BULLET_OWNER_TYPE ownerType)
{
	// ’e‚ğ¶¬
	auto bullet = std::make_shared<Bullet>();

	// BulletConfig‚ğƒRƒs[‚µ‚ÄŠ—LÒ‚ğƒLƒƒƒ‰ƒ^ƒCƒv‚É•ÏŠ·
	BulletConfig modifiedConfig = bulletConfig;
	modifiedConfig.shooterType = ConvertOwnerTypeToCharType(ownerType);	// •ÏŠ·

	// ’e‚ğ”­Ë
	bullet->ActivateBulletSimple(modifiedConfig);

	// ’e‚ğ“o˜^
	RegisterBullet(bullet, ownerType);

	return bullet;
}

CHARA_TYPE BulletManager::ConvertOwnerTypeToCharType(BULLET_OWNER_TYPE ownerType)
{
	// BULLET_OWNER_TYPE ‚ğ CHARA_TYPE ‚É•ÏŠ·
	CHARA_TYPE charaType = CHARA_TYPE::NONE;

	// Š—LÒƒ^ƒCƒv‚Å”»•Ê
	switch(ownerType)
	{
		case BULLET_OWNER_TYPE::BULLET_PLAYER: // ’eƒvƒŒƒCƒ„[
		{
			// ’eƒvƒŒƒCƒ„[
			return CHARA_TYPE::BULLET_PLAYER;
		}

		case BULLET_OWNER_TYPE::ENEMY: // “G
		{
			// “G
			return CHARA_TYPE::ENEMY;
		}

		default:
		{
			return CHARA_TYPE::NONE;
		}
	}
}


/* ’eŠÇ—ŠÖ˜A */

void BulletManager::RegisterBullet(std::shared_ptr<Bullet> bullet, BULLET_OWNER_TYPE ownerType)
{
	if(!bullet){ return; }

	// “o˜^î•ñ
	BULLET_INFO info;

	info.bullet = bullet;		// ’eƒNƒ‰ƒX‚ğİ’è
	info.ownerType = ownerType;	// Š—LÒ‚ğİ’è

	// “o˜^‚·‚é
	_registerBullets.push_back(info);
}

void BulletManager::RemoveBullet(std::shared_ptr<Bullet> bullet)
{
	if(!bullet){ return; }

	// “o˜^‚³‚ê‚½’e‚ğ‘–¸
	for(auto bullets = _registerBullets.begin(); bullets != _registerBullets.end(); ++bullets)
	{
		// ’e‚ª—LŒø‚È‚ç
		if(bullets->bullet && bullets->bullet == bullet)
		{
			// “o˜^‚³‚ê‚½’e‚©‚çíœ
			bullet->Terminate();
			_registerBullets.erase(bullets);


			break;
		}
	}
}

void BulletManager::RemoveBulletByOwnerType(BULLET_OWNER_TYPE ownerType)
{
	// “o˜^‚³‚ê‚½’e‚ğ‘–¸
	for(auto bullets = _registerBullets.begin(); bullets != _registerBullets.end();)
	{
		// ƒ^ƒCƒv‚ªˆê’v‚·‚é‚È‚ç
		if(bullets->ownerType == ownerType)
		{
			// ’e‚ğíœ
			bullets = _registerBullets.erase(bullets);
		}
		// ƒ^ƒCƒv‚ªˆê’v‚µ‚È‚¢‚È‚ç
		else
		{
			// Ÿ‚Ì’e‚ğ’²‚×‚é
			++bullets;
		}
	}
}

void BulletManager::ClearAllBullets(std::vector<std::shared_ptr<Bullet>> bullets)
{
	// “o˜^‚³‚ê‚½’e‚ğ‘–¸
	for(auto& bullet : bullets)
	{
		if(!bullet){ continue; }

		// “o˜^‚³‚ê‚½’e‚©‚çíœ
		bullet->Terminate();
	}

	// “o˜^‚³‚ê‚½’e‚ÌƒŠƒXƒg‚ğƒNƒŠƒA
	_registerBullets.clear();
}

void BulletManager::CleanupInvalidBullets()
{
	// “o˜^‚³‚ê‚½’e‚ğ‘–¸
	for(auto bullets = _registerBullets.begin(); bullets != _registerBullets.end();)
	{
		bool shouldRemove = false;	// íœ‚·‚×‚«‚©

		// ’e‚ªnull‚Ü‚½‚Í¶‘¶ŠÔ‚ªs‚«‚½ê‡íœ
		if(!bullets->bullet || !bullets->bullet->IsBulletAlive())
		{
			shouldRemove = true;
		}

		// íœ‚·‚×‚«‚È‚ç
		if(shouldRemove)
		{
			// íœ
			bullets->bullet->Terminate();
			bullets = _registerBullets.erase(bullets);
		}
		// íœ‚µ‚È‚¢‚×‚«‚È‚ç
		else
		{
			// Ÿ‚Ì’e‚ğ’²‚×‚é
			++bullets;
		}
	}
}

bool BulletManager::IsBulletRegistered(std::shared_ptr<Bullet> bullet) const
{
	if(!bullet) { return false; }

	// “o˜^‚³‚ê‚½’e‚ğ‘–¸
	for(auto& info : _registerBullets)
	{
		// ’e‚ª—LŒø‚È‚ç
		if(info.bullet && info.bullet == bullet)
		{
			// “o˜^Ï‚İ
			return true;
		}
	}

	// –¢“o˜^
	return false;
}


/* ’e‚Ì‰ñ”ğŠÖ˜A */

void BulletManager::RegisterDodgeBullet(std::vector<std::shared_ptr<Bullet>> bullet)
{
	// ‰ñ”ğ‚³‚ê‚½’e‚ğ‘–¸
	for(auto& bullets : bullet)
	{
		// ‰ñ”ğÏ‚İ‚Ì’e‚È‚çƒXƒLƒbƒv
		if(!bullets || IsDodgeBullet(bullet)) { continue; }	

		// ‰ñ”ğ¬Œ÷‚Ìˆ—
		ProcessEvadeBullet();

		// ‰ñ”ğÏ‚İ‚Ì’e‚É’Ç‰Á
		_dodgeBullets.push_back(bullets);

		// ‰ñ”ğ‚³‚ê‚½’e‚Í‘¦À‚Éíœ
		RemoveBullet(bullets);
	}
}

void BulletManager::ProcessEvadeBullet()
{
	// ƒGƒlƒ‹ƒM[ã¸
	auto energyManager = EnergyManager::GetInstance();
	energyManager->AddEnergy(energyManager->GetEvadeBulletEnergy());

	// ƒTƒEƒ“ƒh‚ÌÄ¶
	SoundServer::GetInstance()->Play("SE_Evade", DX_PLAYTYPE_BACK);
}

void BulletManager::ClearDodgeBullets()
{
	// íœ
	_dodgeBullets.clear();
}

bool BulletManager::IsDodgeBullet(std::vector<std::shared_ptr<Bullet>>bullet)const
{
	// ‰ñ”ğ‚³‚ê‚½’e‚ğ‘–¸
	for(auto& bullets : bullet)
	{
		 if(!bullets) { continue; }	

		 // ‰ñ”ğÏ‚İ‚Ì’e‚ğ‘–¸
		for(auto& dodgedBullet : _dodgeBullets)
		{
			// ‰ñ”ğÏ‚İ‚Ì’e‚ª—LŒø‚È‚ç
			if(!dodgedBullet.expired() && dodgedBullet.lock() == bullets)
			{
				// ‰ñ”ğÏ‚İ‚Ì’e‚¾‚Á‚½‚Ì‚ÅƒXƒLƒbƒv
				return true;
			}
		}
	}

	// ‰ñ”ğ‚µ‚Ä‚¢‚È‚¢’e
	return false;
}


/* ’eİ’èŠÖ˜A */

bool BulletManager::UpdateBulletConfig(std::shared_ptr<Bullet> bullet, const BulletConfig& newConfig)
{
	if(!bullet || !IsBulletRegistered(bullet)) { return false; }	// “o˜^Ï‚İ‚¶‚á‚È‚¢‚È‚çƒXƒLƒbƒv

	// ’e‚ÌŠî–{î•ñ‚ğİ’è
	bullet->SetBulletConfig(newConfig);

	// ’e‚ÌˆÊ’uŠÖŒW‚Ìî•ñİ’è
	bullet->SetCoordinateConfig(newConfig);

	return true;
}

bool BulletManager::UpdateBulletEffectConfig(std::shared_ptr<Bullet> bullet, const BulletEffectConfig& newConfig)
{
	if(!bullet || !IsBulletRegistered(bullet)) { return false; }	// “o˜^Ï‚İ‚¶‚á‚È‚¢‚È‚çƒXƒLƒbƒv

	// ’e‚Ì‰‰oî•ñİ’è
	bullet->SetEffectConfig(newConfig);

	return true;
}


/* î•ñæ“¾ŠÖ˜A */

std::vector<std::shared_ptr<Bullet>> BulletManager::GetBulletsByOwnerType(BULLET_OWNER_TYPE ownerType)const
{
	std::vector<std::shared_ptr<Bullet>> result;	// ’e

	for(auto& info : _registerBullets)
	{
		// ƒ^ƒCƒv‚ªˆê’v‚µ‚Ä‚¢‚ÄA’e‚ª—LŒø‚©ƒ`ƒFƒbƒN
		if(info.ownerType == ownerType && info.bullet)
		{
			// ’e‚ğ’Ç‰Á
			result.push_back(info.bullet);
		}
	}

	// ”‚ğ•Ô‚·
	return result;
}

std::vector<std::shared_ptr<Bullet>> BulletManager::GetAllBullets()const
{
	std::vector<std::shared_ptr<Bullet>> result;	// ’e

	// ’e‚ğ‘–¸
	for(auto& info : _registerBullets)
	{
		// ’e‚ª—LŒø‚È‚ç
		if(info.bullet)
		{
			// ’e‚ğ’Ç‰Á
			result.push_back(info.bullet);
		}
	}

	// ”‚ğ•Ô‚·
	return result;
}

BULLET_OWNER_TYPE BulletManager::GetBulletOwnerType(std::shared_ptr<Bullet> bullet)const
{
	if(!bullet) { return BULLET_OWNER_TYPE::NONE; }

	// ’e‚ğ‘–¸
	for(auto& info : _registerBullets)
	{
		// ’e‚ª—LŒø‚È‚ç
		if(info.bullet && info.bullet == bullet)
		{
			// Š—LÒ‚Ìƒ^ƒCƒv‚ğ•Ô‚·
			return info.ownerType;
		}
	}

	// –³Œø‚È’e‚È‚çŠ—LÒ‚È‚µ
	return BULLET_OWNER_TYPE::NONE;
}

