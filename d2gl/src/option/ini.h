#pragma once

namespace d2gl::option {

void loadIni();

bool saveBool(LPCSTR section, LPCSTR key, bool val);
bool saveInt(LPCSTR section, LPCSTR key, int val);
bool saveFloat(LPCSTR section, LPCSTR key, float val);
bool saveString(LPCSTR section, LPCSTR key, const std::string& val);

}