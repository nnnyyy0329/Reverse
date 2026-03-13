#pragma once
#include "SoundBase.h"
class SoundStatic :
    public SoundBase
{
	typedef SoundBase  base;			// ベースクラスの別名
public:
	SoundStatic() : base("", "") {}	// 既定のコンストラクタを追加
	SoundStatic(std::string filename, std::string name) : base(filename, name)
	{
		SetCreateSoundDataType(DX_SOUNDDATATYPE_MEMNOPRESS);		// メモリ上のデータを使用する
		_snd = LoadSoundMem(_filename.c_str());						// サウンドをロードする
		if(_frequency == 0)
		{
			_frequency = GetFrequencySoundMem(_snd);				// サウンドの周波数を取得する
		}
	}

	virtual ~SoundStatic();									// デストラクタ
};


