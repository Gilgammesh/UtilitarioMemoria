#include <vector>
#include <string>
#include <windows.h>
#include <commctrl.h>
#include <algorithm>
#include <thread>
#include <mutex>
#include <chrono>
#include <atomic>
#include <cstdio>
#include "ListaMemoria.h"

// Lista de favoritos (memoria)
std::vector<FavoritoMemoria> listaMemoriaFavoritos;
std::mutex mutexFavoritos;

extern std::atomic<bool> refresherRunning;

// Muestra un InputBox utilizando VBScript en Windows
std::wstring InputBoxWin32(HWND hWnd, const std::wstring& title, const std::wstring& prompt, const std::wstring& valorActual = L"") {
    std::wstring script =
        L"Dim valor\n"
        L"valor=InputBox(\"" + prompt + L"\", \"" + title + L"\", \"" + valorActual + L"\")\n"
        L"Set fso=CreateObject(\"Scripting.FileSystemObject\")\n"
        L"Set archivo=fso.CreateTextFile(WScript.Arguments(0),True)\n"
        L"archivo.Write valor\n"
        L"archivo.Close\n";

    wchar_t tempPath[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);
    wchar_t tempName[MAX_PATH];
    GetTempFileNameW(tempPath, L"inp", 0, tempName);

    wchar_t vbsFile[MAX_PATH], txtFile[MAX_PATH];
    swprintf_s(vbsFile, L"%s.vbs", tempName);
    swprintf_s(txtFile, L"%s.txt", tempName);

    FILE* f = nullptr;
    if (_wfopen_s(&f, vbsFile, L"w, ccs=UNICODE") != 0 || !f) return L"";
    fputws(script.c_str(), f);
    fclose(f);

    wchar_t cmd[MAX_PATH * 4];
    swprintf_s(cmd, L"wscript.exe \"%s\" \"%s\"", vbsFile, txtFile);
    _wsystem(cmd);

    std::wstring inputResult;
    FILE* fr = nullptr;
    if (_wfopen_s(&fr, txtFile, L"r, ccs=UNICODE") == 0 && fr) {
        wchar_t buffer[256];
        if (fgetws(buffer, 256, fr)) {
            inputResult = buffer;
            if (!inputResult.empty() && inputResult.back() == L'\n')
                inputResult.pop_back();
        }
        fclose(fr);
    }

    _wremove(vbsFile);
    _wremove(txtFile);

    return inputResult;
}


// Crea el ListView para favoritos/memoria
HWND CrearListViewMemoria(HWND hWndParent, HINSTANCE hInst, int x, int y, int width, int height, int id)
{
    HWND hList = CreateWindowW(WC_LISTVIEWW, NULL,
        WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SINGLESEL | WS_BORDER,
        x, y, width, height, hWndParent, (HMENU)(UINT_PTR)id, hInst, NULL);

    // Habilita checkboxes
    ListView_SetExtendedListViewStyle(hList, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);

    return hList;
}

// Inicializa las columnas
void InicializarColumnasListViewMemoria(HWND hList)
{
    const wchar_t* headersFreeze[] = { L"Congelar", L"Descripcion", L"Direccion", L"Tipo", L"Valor" };
    int widthsFreeze[] = { 90, 180, 140, 100, 160 };
    for (int i = 0; i < 5; ++i)
    {
        LVCOLUMNW lvc = { 0 };
        lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        lvc.pszText = (LPWSTR)headersFreeze[i];
        lvc.cx = widthsFreeze[i];
        lvc.iSubItem = i;
        ListView_InsertColumn(hList, i, &lvc);
    }
}

void AgregarAFavoritos(HWND hListMemoria, LPCVOID direccion, const std::wstring& tipoDato, const std::wstring& valor) {
    auto it = std::find_if(listaMemoriaFavoritos.begin(), listaMemoriaFavoritos.end(),
        [direccion](const FavoritoMemoria& fav) { return fav.direccion == direccion; });

    if (it == listaMemoriaFavoritos.end()) {
        FavoritoMemoria nuevo{ direccion, L"Sin Descripcion", tipoDato, valor, false };
        listaMemoriaFavoritos.push_back(nuevo);

        // Agrega visualmente al ListView
        LVITEMW item = { 0 };
        item.mask = LVIF_TEXT;
        item.iItem = ListView_GetItemCount(hListMemoria);
        item.pszText = (LPWSTR)L""; // Casilla de congelar (checkbox), columna 0
        item.iSubItem = 0;
        ListView_InsertItem(hListMemoria, &item);

        // Descripcion
        ListView_SetItemText(hListMemoria, item.iItem, 1, (LPWSTR)nuevo.descripcion.c_str());

        // Direccion (hexadecimal, sin prefijo)
        wchar_t buffer[32];
        swprintf_s(buffer, 32, L"%016llX", (unsigned long long)direccion);
        ListView_SetItemText(hListMemoria, item.iItem, 2, buffer);

        // Tipo de dato
        ListView_SetItemText(hListMemoria, item.iItem, 3, (LPWSTR)nuevo.tipoDato.c_str());

        // Valor actual
        ListView_SetItemText(hListMemoria, item.iItem, 4, (LPWSTR)nuevo.valor.c_str());
    }
}

void RefrescarValoresMemoria(HWND hList, HANDLE hProc) {
    if (hProc == NULL || hProc == INVALID_HANDLE_VALUE) {
        OutputDebugString(L"Handle de proceso invalido!\n");
        return;
    }

    while (refresherRunning) {
        {
            std::lock_guard<std::mutex> lock(mutexFavoritos);
            for (size_t i = 0; i < listaMemoriaFavoritos.size(); ++i) {
                auto& fav = listaMemoriaFavoritos[i];
                if (fav.congelado) {
                    // CONGELAR: volver a escribir el valor guardado
                    SIZE_T bytesWritten = 0;
                    bool exito = false;
                    if (fav.tipoDato == L"String") {
                        std::string s(fav.valor.begin(), fav.valor.end());
                        exito = hProc && WriteProcessMemory(hProc, (LPVOID)fav.direccion, s.c_str(), s.size() + 1, &bytesWritten);
                    }
                    else if (fav.tipoDato == L"Int8") {
                        char v = (char)_wtoi(fav.valor.c_str());
                        exito = hProc && WriteProcessMemory(hProc, (LPVOID)fav.direccion, &v, sizeof(v), &bytesWritten);
                    }
                    else if (fav.tipoDato == L"Int16") {
                        short v = (short)_wtoi(fav.valor.c_str());
                        exito = hProc && WriteProcessMemory(hProc, (LPVOID)fav.direccion, &v, sizeof(v), &bytesWritten);
                    }
                    else if (fav.tipoDato == L"Int32") {
                        int v = _wtoi(fav.valor.c_str());
                        exito = hProc && WriteProcessMemory(hProc, (LPVOID)fav.direccion, &v, sizeof(v), &bytesWritten);
                    }
                    else if (fav.tipoDato == L"Int64") {
                        long long v = _wtoll(fav.valor.c_str());
                        exito = hProc && WriteProcessMemory(hProc, (LPVOID)fav.direccion, &v, sizeof(v), &bytesWritten);
                    }
                    else if (fav.tipoDato == L"Float") {
                        float v = (float)_wtof(fav.valor.c_str());
                        exito = hProc && WriteProcessMemory(hProc, (LPVOID)fav.direccion, &v, sizeof(v), &bytesWritten);
                    }
                    else if (fav.tipoDato == L"Double") {
                        double v = _wtof(fav.valor.c_str());
                        exito = hProc && WriteProcessMemory(hProc, (LPVOID)fav.direccion, &v, sizeof(v), &bytesWritten);
                    }
                }
                else {
                    if (fav.tipoDato == L"String") {
                        char buffer[128] = { 0 };
                        SIZE_T bytesRead = 0;
                        if (ReadProcessMemory(hProc, fav.direccion, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
                            buffer[bytesRead] = 0;
                            int wideLen = MultiByteToWideChar(CP_UTF8, 0, buffer, -1, nullptr, 0);
                            std::wstring valorActual;
                            if (wideLen > 1) {
                                valorActual.resize(wideLen - 1);
                                MultiByteToWideChar(CP_UTF8, 0, buffer, -1, &valorActual[0], wideLen);
                            }
                            if (valorActual != fav.valor) {
                                fav.valor = valorActual;
                                ListView_SetItemText(hList, (int)i, 4, (LPWSTR)valorActual.c_str());
                            }
                        }
                    }
                    else if (fav.tipoDato == L"Int8") {
                        char valorLeido = 0;
                        SIZE_T bytesRead = 0;
                        if (ReadProcessMemory(hProc, fav.direccion, &valorLeido, sizeof(char), &bytesRead) && bytesRead == sizeof(char)) {
                            std::wstring valorActual = std::to_wstring((int)valorLeido);
                            if (valorActual != fav.valor) {
                                fav.valor = valorActual;
                                ListView_SetItemText(hList, (int)i, 4, (LPWSTR)valorActual.c_str());
                            }
                        }
                    }
                    else if (fav.tipoDato == L"Int16") {
                        short valorLeido = 0;
                        SIZE_T bytesRead = 0;
                        if (ReadProcessMemory(hProc, fav.direccion, &valorLeido, sizeof(short), &bytesRead) && bytesRead == sizeof(short)) {
                            std::wstring valorActual = std::to_wstring(valorLeido);
                            if (valorActual != fav.valor) {
                                fav.valor = valorActual;
                                ListView_SetItemText(hList, (int)i, 4, (LPWSTR)valorActual.c_str());
                            }
                        }
                    }
                    else if (fav.tipoDato == L"Int32") {
                        int valorLeido = 0;
                        SIZE_T bytesRead = 0;
                        if (ReadProcessMemory(hProc, fav.direccion, &valorLeido, sizeof(int), &bytesRead) && bytesRead == sizeof(int)) {
                            std::wstring valorActual = std::to_wstring(valorLeido);
                            if (valorActual != fav.valor) {
                                fav.valor = valorActual;
                                ListView_SetItemText(hList, (int)i, 4, (LPWSTR)valorActual.c_str());
                            }
                        }
                    }
                    else if (fav.tipoDato == L"Int64") {
                        long long valorLeido = 0;
                        SIZE_T bytesRead = 0;
                        if (ReadProcessMemory(hProc, fav.direccion, &valorLeido, sizeof(long long), &bytesRead) && bytesRead == sizeof(long long)) {
                            std::wstring valorActual = std::to_wstring(valorLeido);
                            if (valorActual != fav.valor) {
                                fav.valor = valorActual;
                                ListView_SetItemText(hList, (int)i, 4, (LPWSTR)valorActual.c_str());
                            }
                        }
                    }
                    else if (fav.tipoDato == L"Float") {
                        float valorLeido = 0;
                        SIZE_T bytesRead = 0;
                        if (ReadProcessMemory(hProc, fav.direccion, &valorLeido, sizeof(float), &bytesRead) && bytesRead == sizeof(float)) {
                            wchar_t buffer[64];
                            swprintf_s(buffer, 64, L"%g", valorLeido);
                            std::wstring valorActual = buffer;
                            if (valorActual != fav.valor) {
                                fav.valor = valorActual;
                                ListView_SetItemText(hList, (int)i, 4, (LPWSTR)valorActual.c_str());
                            }
                        }
                    }
                    else if (fav.tipoDato == L"Double") {
                        double valorLeido = 0;
                        SIZE_T bytesRead = 0;
                        if (ReadProcessMemory(hProc, fav.direccion, &valorLeido, sizeof(double), &bytesRead) && bytesRead == sizeof(double)) {
                            wchar_t buffer[64];
                            swprintf_s(buffer, 64, L"%g", valorLeido);
                            std::wstring valorActual = buffer;
                            if (valorActual != fav.valor) {
                                fav.valor = valorActual;
                                ListView_SetItemText(hList, (int)i, 4, (LPWSTR)valorActual.c_str());
                            }
                        }
                    }
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
