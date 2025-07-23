#include <cstdio>
#include "SeleccionarProceso.h"
#include "resource.h"

wchar_t currentProcessName[MAX_PATH] = L"";
DWORD currentProcessPid = 0;

void CrearSeleccionarProcesoControles(HWND hWnd, HINSTANCE hInst, HWND& hLblBtnIcon, HWND& hBtnSelectIcon, HWND& hBtnSaveConfig, HWND& hBtnLoadConfig, HWND& hLblCurrentProcess, wchar_t* currentProcessName, int width)
{
    // Botón GUARDAR configuración
    hBtnSaveConfig = CreateWindowW(L"BUTTON", NULL, WS_VISIBLE | WS_CHILD | BS_ICON | WS_TABSTOP,
        10, 10, 40, 40, hWnd, (HMENU)1010, hInst, NULL);

    // Botón CARGAR configuración
    hBtnLoadConfig = CreateWindowW(L"BUTTON", NULL, WS_VISIBLE | WS_CHILD | BS_ICON | WS_TABSTOP,
        55, 10, 40, 40, hWnd, (HMENU)1011, hInst, NULL);

    // Icono guardar
    HICON hIconSave = (HICON)LoadImageW(hInst, MAKEINTRESOURCEW(IDI_SAVEICO), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
    if (hIconSave)
        SendMessageW(hBtnSaveConfig, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIconSave);

    // Icono cargar
    HICON hIconLoad = (HICON)LoadImageW(hInst, MAKEINTRESOURCEW(IDI_LOADICO), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
    if (hIconLoad)
        SendMessageW(hBtnLoadConfig, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIconLoad);

    // Botón SELECCIONAR proceso (icono y label)
    hLblBtnIcon = CreateWindowW(L"STATIC", L"Seleccionar Proceso: ", WS_VISIBLE | WS_CHILD,
        100, 10, 90, 40, hWnd, NULL, hInst, NULL);

    hBtnSelectIcon = CreateWindowW(L"BUTTON", NULL, WS_VISIBLE | WS_CHILD | BS_ICON | WS_TABSTOP,
        200, 10, 40, 40, hWnd, (HMENU)1001, hInst, NULL);

    int xCurrent = 260;
    int widthCurrent = width - xCurrent - 20;
    hLblCurrentProcess = CreateWindowW(
        L"STATIC", currentProcessName,
        WS_VISIBLE | WS_CHILD | SS_LEFTNOWORDWRAP,
        xCurrent, 15, widthCurrent, 30, hWnd, NULL, hInst, NULL);

    // Icono proceso
    HICON hIcon = (HICON)LoadImageW(hInst, MAKEINTRESOURCEW(IDI_PROCESSICO), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
    if (hIcon)
        SendMessageW(hBtnSelectIcon, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
}

void ActualizarNombreProceso(HWND hLblCurrentProcess, const wchar_t* processName, DWORD processPid)
{
	wcscpy_s(currentProcessName, MAX_PATH, processName);
	currentProcessPid = processPid;

	wchar_t display[MAX_PATH + 32];
	swprintf_s(display, L"%s (PID: %u)", processName, processPid);
	SetWindowTextW(hLblCurrentProcess, display);
}