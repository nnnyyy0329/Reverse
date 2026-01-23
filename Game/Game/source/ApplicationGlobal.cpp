#include "ApplicationGlobal.h"

ApplicationGlobal gGlobal;

ApplicationGlobal::ApplicationGlobal() {
}

ApplicationGlobal::~ApplicationGlobal() {
}

bool ApplicationGlobal::Init() {
	_cgCursor = LoadGraph("res/base/cursor.png");

	return true;
}
