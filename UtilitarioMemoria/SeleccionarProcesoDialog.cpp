#include <windows.h>
#include <tlhelp32.h>
#include <vector>
#include <algorithm>
#include <commctrl.h>
#include <psapi.h>
#include <string>
#include "SeleccionarProceso.h"
#include "SeleccionarProcesoDialog.h"

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "psapi.lib")

static HWND g_hLblCurrentProcess = NULL;

struct ProcessItem {
    wchar_t name[MAX_PATH];
    DWORD pid;
    wchar_t wndTitle[256];
};
static std::vector<ProcessItem> g_processList;

// Variable global para el HANDLE del proceso seleccionado
HANDLE g_hSelectedProcess = NULL;

HANDLE GetSelectedProcessHandle() {
    return g_hSelectedProcess;
}

std::wstring GetProcessPathByPID(DWORD pid) {
    std::wstring path;
    HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProc) {
        wchar_t buf[MAX_PATH] = { 0 };
        if (GetModuleFileNameExW(hProc, NULL, buf, MAX_PATH)) {
            path = buf;
        }
        CloseHandle(hProc);
    }
    return path;
}

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    DWORD procId = 0;
    GetWindowThreadProcessId(hwnd, &procId);
    if (procId == 0) return TRUE;
    if (IsWindowVisible(hwnd) && GetWindow(hwnd, GW_OWNER) == NULL) {
        std::vector<std::pair<DWORD, HWND>>* pids = reinterpret_cast<std::vector<std::pair<DWORD, HWND>>*>(lParam);
        pids->emplace_back(procId, hwnd);
    }
    return TRUE;
}

static LRESULT CALLBACK ProcessWndProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

void ShowProcessListDialog(HWND hParent, HWND hLblCurrentProcess) {
    g_hLblCurrentProcess = hLblCurrentProcess;

	// Ventana de dialogo
    const int dlgW = 400, dlgH = 470;
    HWND hDialog = CreateWindowExW(
        WS_EX_DLGMODALFRAME, L"STATIC", L"Lista de Procesos",
        WS_POPUPWINDOW | WS_CAPTION | WS_SYSMENU,
        400, 150, dlgW, dlgH,
        hParent, NULL, GetModuleHandleW(NULL), NULL);

    if (!hDialog) return;

    SetWindowLongPtrW(hDialog, GWLP_WNDPROC, (LONG_PTR)ProcessWndProc);

	// Lista de procesos
    int listviewW = dlgW - 60;
    int margenSup = 20;
    int margenInf = 100;
    int listviewH = dlgH - margenSup - margenInf;
    HWND hListView = CreateWindowW(WC_LISTVIEW, NULL,
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
        20, margenSup, listviewW, listviewH, hDialog, (HMENU)ID_LISTVIEW_PROCESOS, GetModuleHandleW(NULL), NULL);

    // Inicializa columnas
    int col2 = 80;
    int col1 = listviewW - col2;
    LVCOLUMNW lvc = { 0 };
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    lvc.pszText = (LPWSTR)L"Nombre";
    lvc.cx = col1;
    lvc.iSubItem = 0;
    ListView_InsertColumn(hListView, 0, &lvc);
    lvc.pszText = (LPWSTR)L"PID";
    lvc.cx = col2;
    lvc.iSubItem = 1;
    ListView_InsertColumn(hListView, 1, &lvc);

    // Crea el image list para iconos
    HIMAGELIST hImgList = ImageList_Create(24, 24, ILC_COLOR32 | ILC_MASK, 40, 20);
    ListView_SetImageList(hListView, hImgList, LVSIL_SMALL);

    // 1. Obtener lista de ventanas principales y sus PIDs
    std::vector<std::pair<DWORD, HWND>> guiPids;
    EnumWindows(EnumWindowsProc, (LPARAM)&guiPids);

    // 2. Armar lista de procesos con ventana principal visible
    g_processList.clear();
    int idx = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W pe;
        pe.dwSize = sizeof(pe);
        if (Process32FirstW(hSnap, &pe)) {
            do {
                auto it = std::find_if(guiPids.begin(), guiPids.end(),
                    [&pe](const std::pair<DWORD, HWND>& p) { return p.first == pe.th32ProcessID; });
                if (it == guiPids.end()) continue; // No es GUI

                wchar_t wndTitle[256] = L"";
                if (it->second)
                    GetWindowTextW(it->second, wndTitle, 255);

                // Obtener ruta al exe para extraer el icono
                std::wstring exePath = GetProcessPathByPID(pe.th32ProcessID);
                HICON hIcon = NULL;
                if (!exePath.empty()) {
                    hIcon = ExtractIconW(GetModuleHandleW(NULL), exePath.c_str(), 0);
                }
                if (!hIcon) {
                    hIcon = LoadIcon(NULL, IDI_APPLICATION); // Icono por defecto
                }
                int iconIndex = ImageList_AddIcon(hImgList, hIcon);
                if (hIcon)
                    DestroyIcon(hIcon); // ImageList hace copia

                // Prepara los textos
                wchar_t szPID[32];
                _itow_s(pe.th32ProcessID, szPID, 32, 10);

                LVITEMW lvi = { 0 };
                lvi.mask = LVIF_TEXT | LVIF_IMAGE;
                lvi.iItem = idx;
                lvi.pszText = pe.szExeFile;
                lvi.iImage = iconIndex;
                int itemIndex = ListView_InsertItem(hListView, &lvi);

                ListView_SetItemText(hListView, itemIndex, 1, szPID);

                // Guarda la info en la lista para uso posterior
                ProcessItem item;
                wcscpy_s(item.name, pe.szExeFile);
                item.pid = pe.th32ProcessID;
                wcscpy_s(item.wndTitle, wndTitle);
                g_processList.push_back(item);

                ++idx;
            } while (Process32NextW(hSnap, &pe));
        }
        CloseHandle(hSnap);
    }

    // --- Botones ---
    int yBtn = margenSup + listviewH + 20;
    CreateWindowW(L"BUTTON", L"Abrir", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
        80, yBtn, 100, 32, hDialog, (HMENU)ID_BTN_ABRIR, GetModuleHandleW(NULL), NULL);
    CreateWindowW(L"BUTTON", L"Cancelar", WS_CHILD | WS_VISIBLE,
        220, yBtn, 100, 32, hDialog, (HMENU)ID_BTN_CANCELAR, GetModuleHandleW(NULL), NULL);

    // Centrar ventana respecto a padre
    RECT rcParent;
    GetWindowRect(hParent, &rcParent);
    int x = rcParent.left + ((rcParent.right - rcParent.left) - dlgW) / 2;
    int y = rcParent.top + ((rcParent.bottom - rcParent.top) - dlgH) / 2;
    SetWindowPos(hDialog, HWND_TOP, x, y, dlgW, dlgH, SWP_SHOWWINDOW);

    // Modal loop
    EnableWindow(hParent, FALSE);
    MSG msg;
    while (IsWindow(hDialog) && GetMessage(&msg, NULL, 0, 0)) {
        if (!IsDialogMessage(hDialog, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    EnableWindow(hParent, TRUE);
    SetForegroundWindow(hParent);
}

// --- WndProc para manejar seleccion/aceptar ---
static LRESULT CALLBACK ProcessWndProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_COMMAND:
        if (LOWORD(wParam) == ID_BTN_ABRIR) {
            HWND hListView = GetDlgItem(hDlg, ID_LISTVIEW_PROCESOS);
            int sel = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
            if (sel != -1 && sel < (int)g_processList.size()) {
                // Cerrar el anterior si existia
                if (g_hSelectedProcess) {
                    CloseHandle(g_hSelectedProcess);
                    g_hSelectedProcess = NULL;
                }
                // Abrir el handle del proceso seleccionado
                g_hSelectedProcess = OpenProcess(
                    PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION,
                    FALSE,
                    g_processList[sel].pid
                );
                if (!g_hSelectedProcess) {
                    MessageBoxW(NULL, L"No se pudo abrir el proceso.", L"Error", MB_OK | MB_ICONERROR);
                    return 0;
                }
                ActualizarNombreProceso(
                    g_hLblCurrentProcess,
                    g_processList[sel].name,
                    g_processList[sel].pid
                );
            }
            DestroyWindow(hDlg);
            return 0;
        }
        if (LOWORD(wParam) == ID_BTN_CANCELAR) {
            DestroyWindow(hDlg);
            return 0;
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hDlg);
        return 0;
    }
    return DefWindowProcW(hDlg, msg, wParam, lParam);
}
