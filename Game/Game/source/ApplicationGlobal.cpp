#include "ApplicationGlobal.h"

ApplicationGlobal gGlobal;

ApplicationGlobal::ApplicationGlobal() {
}

ApplicationGlobal::~ApplicationGlobal() {
}

bool ApplicationGlobal::Init() {
	_cgCursor = LoadGraph("res/cursor.png");

	return true;
}
