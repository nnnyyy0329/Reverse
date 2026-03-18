#include "CameraManager.h"
#include "CameraBase.h"
#include "GameCamera.h"
#include "DebugCamera.h"
#include "AimCamera.h"
#include "EventCamera.h"

CameraManager::CameraManager()
{
	_gameCamera = std::make_unique<GameCamera>();
	_debugCamera = std::make_unique<DebugCamera>();
	_aimCamera = std::make_unique<AimCamera>();
	_eventCamera = std::make_unique<EventCamera>();

	_pActiveCamera = _gameCamera.get();

	_eCameraType = CAMERA_TYPE::GAME_CAMERA;// 初期タイプはゲームカメラ
	_ePrevCameraType = CAMERA_TYPE::NONE;// 前回のカメラタイプ初期化
}

CameraManager::~CameraManager()
{
}

bool CameraManager::Initialize()
{
	return true;
}

bool CameraManager::Terminate()
{
	return true;
}

bool CameraManager::Process()
{
	// アクティブなカメラの更新
	if (_pActiveCamera)
	{
		_pActiveCamera->Process();
	}

	// アドオンの更新と適用
	for (auto it = _addons.begin(); it != _addons.end();)
	{
		auto& addon = *it;

		addon->Process();

		// アクティブなカメラに効果を適用
		if (_pActiveCamera)
		{
			addon->Apply(_pActiveCamera);
		}

		// 効果が終了しているアドオンはリストから削除
		if (addon->IsFinished())
		{
			it = _addons.erase(it);
		}
		else
		{
			++it;
		}
	}

	SetUp();

	return true;
}

bool CameraManager::Render()
{
	if(_pActiveCamera)
	{
		_pActiveCamera->Render();
	}

	return true;
}

bool CameraManager::DebugRender()
{
	if(_pActiveCamera)
	{
		_pActiveCamera->DebugRender();
	}

	return true;
}

void CameraManager::SetUp()
{
	if(_pActiveCamera)
	{
		_pActiveCamera->SetUp();
	}
}

void CameraManager::SetTarget(std::shared_ptr<PlayerBase> target)
{
	if (_gameCamera) { _gameCamera->SetTarget(target); }
	if (_aimCamera) { _aimCamera->SetTarget(target); }
	if (_debugCamera) { _debugCamera->SetTarget(target); }
	if (_eventCamera) { _eventCamera->SetTarget(target); }
}

// カメラ切り替えはここにまとめる
void CameraManager::SetCameraType(CAMERA_TYPE type)
{
	if (_eCameraType == type) { return; }

	// 切り替え前のカメラの角度を保存
	float prevAngleH = 0.0f;
	float prevAngleV = 0.0f;
	VECTOR prevPos = VGet(0.0f, 0.0f, 0.0f);
	VECTOR prevTarget = VGet(0.0f, 0.0f, 0.0f);

	// 前のカメラの終了処理
	if (_pActiveCamera) 
	{
		prevAngleH = _pActiveCamera->GetAngleH();
		prevAngleV = _pActiveCamera->GetAngleV();
		prevPos = _pActiveCamera->GetPos();
		prevTarget = _pActiveCamera->GetTarget();

		_pActiveCamera->OnExit();
	}

	// 履歴を更新
	_ePrevCameraType = _eCameraType;
	_eCameraType = type;

	// ポインタを切り替え
	switch (_eCameraType)
	{
		case CAMERA_TYPE::GAME_CAMERA: _pActiveCamera = _gameCamera.get(); break;
		case CAMERA_TYPE::DEBUG_CAMERA: _pActiveCamera = _debugCamera.get(); break;
		case CAMERA_TYPE::AIM_CAMERA: _pActiveCamera = _aimCamera.get(); break;
		case CAMERA_TYPE::EVENT_CAMERA: _pActiveCamera = _eventCamera.get(); break;
		default: _pActiveCamera = nullptr; break;
	}

	// 新しいカメラの開始処理
	if (_pActiveCamera) 
	{
		// 前のカメラの状態を引き継ぐ
		_pActiveCamera->SetAngleH(prevAngleH);
		_pActiveCamera->SetAngleV(prevAngleV);
		_pActiveCamera->SetPos(prevPos);
		_pActiveCamera->SetLookAtPoint(prevTarget);

		_pActiveCamera->OnEnter();
		_pActiveCamera->SetUp();
	}
}

void CameraManager::SetIsUseDebugCamera(bool isUse)
{
	if (isUse) 
	{
		SetCameraType(CAMERA_TYPE::DEBUG_CAMERA);
	}
	else 
	{
		SetCameraType(CAMERA_TYPE::GAME_CAMERA);
	}
}

// アクティブなカメラの向いている方向を取得
VECTOR CameraManager::GetCameraDir()
{
	if (_pActiveCamera) { return _pActiveCamera->GetCameraDir(); }
	return VGet(0.0f, 0.0f, 1.0f);// デフォルト
}

float CameraManager::GetCurrentCameraAngleH()
{
	if(_pActiveCamera) { return _pActiveCamera->GetAngleH(); }
	return 0.0f;
}

VECTOR CameraManager::GetActiveCameraPos()
{
	if (_pActiveCamera) { return _pActiveCamera->GetPos(); }
	return VGet(0.0f, 0.0f, 0.0f);
}

VECTOR CameraManager::GetActiveCameraTarget()
{
	if (_pActiveCamera) { return _pActiveCamera->GetTarget(); }
	return VGet(0.0f, 0.0f, 0.0f);
}

void CameraManager::SetActiveCameraPos(const VECTOR& pos)
{
	if (_pActiveCamera) { _pActiveCamera->SetPos(pos); }
}

void CameraManager::AddAddon(std::shared_ptr<ICameraAddon> addon)
{
	if (addon) 
	{
		_addons.push_back(addon);
	}
}


// カメラとアドオンをリセット
void CameraManager::Reset()
{
	if (_pActiveCamera)
	{
		_pActiveCamera->Reset();
	}

	_addons.clear();
}