#pragma once
#include "SoundStatic.h"
class SoundSe :
    public SoundStatic
{

	typedef SoundStatic base;
public:
	SoundSe();																		// 既定のコンストラクタ
	virtual ~SoundSe();																// デストラクタ

	SoundSe(const std::string filename, const std::string name) :base(filename, name) {};	// コンストラクタ
	virtual SOUNDTYPE GetType() override;													// サウンドのタイプを取得する
	virtual void Play() override;														// サウンドを再生する

	void SEItem();
};




																	

