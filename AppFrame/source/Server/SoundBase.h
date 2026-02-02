#pragma once
#include <DxLib.h>			// DXライブラリのヘッダファイル
#include <string>


class SoundServer;  //前方宣言

class SoundBase
{
public:
	enum class SOUNDTYPE
	{
		_NONE_ = 0,	// 未定義
		BGM,		// BGM
		SE,			// 効果音
		VOICE,		// ボイス
		ONESHOT,	// ワンショット
	};

	SoundBase(std::string filename, std::string name);		// コンストラクタ
	virtual ~SoundBase();									// デストラクタ
	void SetSoundServer(SoundServer* sndServer) { _sndServer = sndServer; }		// サウンドサーバーを設定する

	virtual void Unload();									// サウンドをアンロードする
	virtual void Play() {}									// サウンドを再生する
	virtual bool IsLoad(); 									// サウンドがロードされているか確認する
	virtual bool IsPlay();									// サウンドが再生中か確認する
	virtual void Stop();									// サウンドを停止する

	virtual int  GetVolume(); 								// サウンドの音量を取得する
	virtual void SetVolume(int volume);						// サウンドの音量を設定する
	virtual int	 GetPan();									// サウンドのパン振り(位置によって音が移動する)を取得する
	virtual void SetPan(int pan);							// サウンドのパン振りを設定する
	virtual int  GetFrequency();							// サウンドの周波数を取得する
	virtual void SetFrequency(int frequency);				// サウンドの周波数を設定する
	virtual void ResetFrequency();							// サウンドの周波数をリセットする

	virtual SOUNDTYPE GetType() { return SOUNDTYPE::_NONE_; }			// サウンドのタイプを取得する
	std::string Name() { return _name; } 					// サウンドの名前を取得する
	std::string GetFileName() { return _filename; }			// サウンドのファイル名を取得する
	virtual int GetSoundHandle() { return _snd; }			// サウンドのハンドルを取得する

	virtual void Update() {}								// サウンドの更新処理
	virtual void SetDead(bool dead) { _dead = dead; }		// サウンドを削除するか設定する
	virtual bool IsDead() { return _dead; }					// サウンドが削除されるか確認する

protected:
	virtual void PlayMem(int flg);							//再生前に音量等を設定する


	int _snd;						// サウンドのハンドル
	int _volume;					// サウンドの音量
	int _pan;						// サウンドのパン振り(位置によって音が移動する)
	int _frequency;					// サウンドの周波数
	std::string _filename;			// サウンドのファイル名
	std::string _name;				// サウンドの名前
	SoundServer* _sndServer;		// サウンドサーバーへのポインタ
	bool _dead;						// 削除予約
};

