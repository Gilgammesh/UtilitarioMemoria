// Globals.cpp
#include <atomic>
#include "Globals.h"

LPCVOID selectedMemory = nullptr;

std::atomic<bool> g_actualizandoUI{ false };