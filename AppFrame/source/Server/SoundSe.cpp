#include "SoundSe.h"

// コンストラクタ
SoundSe::SoundSe()
{
	SEItem();						// 効果音アイテム用再生
}

// デストラクタ
SoundSe::~SoundSe()
{
	// 何もしない
}

// サウンドのタイプを取得する
SoundSe::TYPE SoundSe::GetType()
{
	return SOUNDTYPE::SE;				// 効果音タイプを返す
}

// サウンドを再生する
void SoundSe::Play()
{
	Stop();							// 再生前に停止
	PlayMem(DX_PLAYTYPE_BACK);		// サウンドをバックグラウンドで再生
}

// 効果音アイテム用再生
void SoundSe::SEItem()
{
	// 何もしない
}