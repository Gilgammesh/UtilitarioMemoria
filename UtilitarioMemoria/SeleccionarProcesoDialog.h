#pragma once
#include <windows.h>

#define ID_LISTVIEW_PROCESOS  3001
#define ID_BTN_ABRIR          3002
#define ID_BTN_CANCELAR       3003

extern HANDLE g_hSelectedProcess;
HANDLE GetSelectedProcessHandle();

// Llama la ventana modal y la muestra, bloqueando hasta cerrar
void ShowProcessListDialog(HWND hParent, HWND hLblCurrentProcess);
