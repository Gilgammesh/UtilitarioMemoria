#include <windows.h>
#include <fstream>
#include <sstream>
#include <commctrl.h>
#include <mutex>
#include <commdlg.h> 
#include "resource.h"
#include "ConfiguracionFavoritos.h"
#include "ListaMemoria.h"
#include "Globals.h"


void GuardarFavoritosEnArchivo() {
    // Buffer para la ruta del archivo (MAX_PATH + extra)
    wchar_t szFileName[MAX_PATH + 100] = L"configuracion_favorita.txt"; // nombre por defecto

    // Estructura para el cuadro de diálogo
    OPENFILENAMEW ofn = { 0 };
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = sizeof(szFileName) / sizeof(wchar_t);
    ofn.lpstrFilter = L"Archivos de texto (*.txt)\0*.txt\0Todos los archivos (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = L"txt";

    if (!GetSaveFileNameW(&ofn)) {
        return;
    }

    std::wofstream archivo(szFileName);
    if (!archivo) {
        MessageBoxW(NULL, L"No se pudo abrir el archivo para guardar.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }
    {
        std::lock_guard<std::mutex> lock(mutexFavoritos);
        for (const auto& fav : listaMemoriaFavoritos) {
            archivo << (size_t)fav.direccion << L"|"
                << fav.descripcion << L"|"
                << fav.tipoDato << L"|"
                << fav.valor << L"|"
                << fav.congelado << std::endl;
        }
    }
    archivo.close();
    MessageBoxW(NULL, L"Configuración guardada correctamente.", L"Guardar", MB_OK | MB_ICONINFORMATION);
}

void CargarFavoritosDeArchivo(HWND hListFreeze) {
    // 1. Abrir archivo
    wchar_t szFileName[MAX_PATH + 100] = L"";
    OPENFILENAMEW ofn = { 0 };
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = sizeof(szFileName) / sizeof(wchar_t);
    ofn.lpstrFilter = L"Archivos de texto (*.txt)\0*.txt\0Todos los archivos (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = L"txt";

    if (!GetOpenFileNameW(&ofn)) return;

    std::wifstream archivo(szFileName);
    if (!archivo) {
        MessageBoxW(NULL, L"No se pudo abrir el archivo de configuración.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    // 2. Leer todo en una copia temporal
    std::vector<FavoritoMemoria> nuevosFavoritos;
    std::wstring linea;
    while (std::getline(archivo, linea)) {
        std::wstringstream ss(linea);
        std::wstring direccionStr, descripcion, tipoDato, valor, congeladoStr;
        std::getline(ss, direccionStr, L'|');
        std::getline(ss, descripcion, L'|');
        std::getline(ss, tipoDato, L'|');
        std::getline(ss, valor, L'|');
        std::getline(ss, congeladoStr, L'|');

        FavoritoMemoria fav;
        fav.direccion = (LPCVOID)std::stoull(direccionStr);
        fav.descripcion = descripcion;
        fav.tipoDato = tipoDato;
        fav.valor = valor;
        fav.congelado = (congeladoStr == L"1" || congeladoStr == L"true" || congeladoStr == L"1\r");

        nuevosFavoritos.push_back(fav);
    }
    archivo.close();

    // 3. Lock para reemplazar el vector
    {
        std::lock_guard<std::mutex> lock(mutexFavoritos);
        listaMemoriaFavoritos.swap(nuevosFavoritos);
    }

    // 4. Actualiza la UI FUERA del lock
    g_actualizandoUI = true;
    ListView_DeleteAllItems(hListFreeze);

    {
        std::lock_guard<std::mutex> lock(mutexFavoritos); // Solo lectura rápida
        for (size_t i = 0; i < listaMemoriaFavoritos.size(); ++i) {
            const auto& fav = listaMemoriaFavoritos[i];

            LVITEMW item = { 0 };
            item.mask = LVIF_TEXT;
            item.iItem = (int)i;
            item.pszText = (LPWSTR)L"";
            item.iSubItem = 0;
            ListView_InsertItem(hListFreeze, &item);

            ListView_SetItemText(hListFreeze, item.iItem, 1, (LPWSTR)fav.descripcion.c_str());
            wchar_t buffer[32];
            swprintf_s(buffer, 32, L"%016llX", (unsigned long long)fav.direccion);
            ListView_SetItemText(hListFreeze, item.iItem, 2, buffer);
            ListView_SetItemText(hListFreeze, item.iItem, 3, (LPWSTR)fav.tipoDato.c_str());
            ListView_SetItemText(hListFreeze, item.iItem, 4, (LPWSTR)fav.valor.c_str());
            ListView_SetCheckState(hListFreeze, item.iItem, fav.congelado);
        }
    }
    g_actualizandoUI = false;
    MessageBoxW(NULL, L"Configuración cargada correctamente.", L"Cargar", MB_OK | MB_ICONINFORMATION);
}
