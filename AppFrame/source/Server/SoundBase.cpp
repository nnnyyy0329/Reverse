


#include <DxLib.h>
#include "SoundBase.h"
#include "SoundServer.h"

// コンストラクタ
SoundBase::SoundBase(std::string filename, std::string name)
{
	_filename = filename;		// サウンドのファイル名を設定
	_name = name;				// サウンドの名前を設定
	_snd = -1;					// サウンドのハンドルを初期化
	_volume = 160;				// サウンドの音量を初期化
	_pan = 0;					// サウンドのパン振りを初期化
	_frequency = 0;				// サウンドの周波数を初期化(ファイルによって標準が違う
	_dead = false;				// サウンドの削除予約を初期化
	_sndServer = NULL;			// サウンドサーバーを初期化
}

// デストラクタ
SoundBase::~SoundBase()
{
	Unload();		// サウンドをアンロードする
}

void SoundBase::Unload()
{
	// サウンドがロードされている場合はアンロードする
	if(_snd != -1)
	{
		DeleteSoundMem(_snd);		// サウンドを解放する
		_snd = -1;					// サウンドのハンドルを無効化
	}
}

// サウンドがロードされているか確認する
bool SoundBase::IsLoad()
{
	//非同期ロードが終わっているか？
	if(_snd != -1 && CheckHandleASyncLoad(_snd) == FALSE)
	{
		return true;		// サウンドがロードされている
	}
	return false;			// サウンドがロードされていない
}

// サウンドが再生中か確認する
bool SoundBase::IsPlay()
{
	// サウンドハンドルが有効で、かつサウンドが再生中か確認
	if(_snd != -1 && CheckSoundMem(_snd) == 1)
	{
		return true;		// サウンドが再生中
	}
	return false;			// サウンドが再生中ではない
}

// サウンドを停止する
void SoundBase::Stop()
{
	if(_snd != -1)
	{
		StopSoundMem(_snd);		// サウンドを停止する
	}
}

// サウンドの音量を取得する
int SoundBase::GetVolume()
{
	return _volume;				// サウンドの音量を返す
}

// サウンドの音量を設定する
void SoundBase::SetVolume(int volume)
{
	_volume = volume;							// サウンドの音量を設定
	if(_snd != -1)
	{
		ChangeVolumeSoundMem(_volume, _snd);	// サウンドの音量を変更
	}
}

// サウンドのパン振り(位置によって音が移動する)を取得する
int SoundBase::GetPan()
{
	return _pan;				// サウンドのパン振りを返す
}

// サウンドのパン振りを設定する
void SoundBase::SetPan(int pan)
{
	_pan = pan;								// サウンドのパン振りを設定
	if(_snd != -1)
	{
		ChangePanSoundMem(_pan, _snd);		// サウンドのパン振りを変更
	}
}

// サウンドの周波数を取得する
int SoundBase::GetFrequency()
{
	return _frequency;			// サウンドの周波数を返す
}

// サウンドの周波数を設定する
void SoundBase::SetFrequency(int frequency)
{
	_frequency = frequency;							// サウンドの周波数を設定
	if(_snd != -1)
	{
		SetFrequencySoundMem(_frequency, _snd);		// サウンドの周波数を変更
	}
}

// サウンドの周波数をリセットする
void SoundBase::ResetFrequency()
{
	_frequency = 0;								// サウンドの周波数を0に設定
	if(_snd != -1)
	{
		ResetFrequencySoundMem(_snd);				// サウンドの周波数をリセット
		_frequency = GetFrequencySoundMem(_snd);		// サウンドの周波数を再取得
	}
}

// 再生前に音量等を設定する
void SoundBase::PlayMem(int flg)
{
	// 再生前に音量等を設定
	SetVolume(_volume);					// 音量を設定
	SetPan(_pan);						// パン振りを設定
	SetFrequency(_frequency);			// 周波数を設定
	PlaySoundMem(_snd, flg, TRUE);		// サウンドを再生する
}
