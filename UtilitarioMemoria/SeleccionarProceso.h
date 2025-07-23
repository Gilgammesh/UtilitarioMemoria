#pragma once
#include <windows.h>

extern wchar_t currentProcessName[MAX_PATH];
extern DWORD currentProcessPid;

// Exporta para uso externo
void CrearSeleccionarProcesoControles(HWND hWnd, HINSTANCE hInst, HWND& hLblBtnIcon, HWND& hBtnSelectIcon, HWND& hBtnSaveConfig, HWND& hBtnLoadConfig, HWND& hLblCurrentProcess, wchar_t* currentProcess, int width);
void ActualizarNombreProceso(HWND hLblCurrentProcess, const wchar_t* processName, DWORD processPid);
