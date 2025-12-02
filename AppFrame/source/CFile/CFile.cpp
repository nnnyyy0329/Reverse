#include <fstream>
#include <iostream>
#include "CFile.h"
#include "../Application/UtilMacro.h"

// 読み込み
CFile::CFile(const std::string filename) {
	// メンバの初期化
	Init();
	_filename = filename;

	// ファイルを開く
	std::ifstream ifs(filename, std::ios::in | std::ios::binary);	// 入力をバイナリ形式で
	if (!ifs) {
		// 開くの失敗
		return;
	}

	// ファイルサイズの取得
	ifs.seekg(0, std::ios_base::end);
	_size = (int)ifs.tellg();
	ifs.seekg(0, std::ios_base::beg);

	// メモリを確保
	// 文字列として利用できるように、+1byte余計に確保
	_data = new char[_size + 1];

	// ファイルをロード
	ifs.read(_data, _size);

	// ファイルを閉じる
	ifs.close();

	// 文字列として利用できるように、データの末端に\0を付ける
	_data[_size] = '\0';

	// 処理成功
	_success = true;
}

// 書き込み
CFile::CFile(const std::string filename, void* data, int size) {
	// メンバの初期化
	Init();
	_filename = filename;

	// ファイルを開く
	std::ofstream ofs(filename, std::ios::out | std::ios::binary);	// 出力をバイナリ形式で
	if (!ofs) {
		// 開くの失敗
		return;
	}

	// データの書き込み
	ofs.write((char*)data, size);

	// ファイルを閉じる
	ofs.close();

	// 処理成功
	_success = true;
}

// 書き込み


CFile::~CFile() {
	SAFE_DELETE(_data);
}

void CFile::Init() {
	_size = 0;
	_data = NULL;
	_filename = "";
	_success = false;
}
