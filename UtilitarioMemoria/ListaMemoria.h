#pragma once
#include <windows.h>
#include <string>
#include <vector>

// Estructura para la lista de memoria
struct FavoritoMemoria {
    LPCVOID direccion;
    std::wstring descripcion;
    std::wstring tipoDato;
    std::wstring valor;
    bool congelado = false;
};

extern std::vector<FavoritoMemoria> listaMemoriaFavoritos;
extern std::mutex mutexFavoritos;

std::wstring InputBoxWin32(HWND hWnd, const std::wstring& title, const std::wstring& prompt, const std::wstring& valorActual);

// Función para crear la lista de favoritos (memoria/congelados)
HWND CrearListViewMemoria(HWND hWndParent, HINSTANCE hInst, int x, int y, int width, int height, int id);

// Inicializa las columnas de la lista de favoritos/memoria
void InicializarColumnasListViewMemoria(HWND hList);

void AgregarAFavoritos(HWND hListMemoria, LPCVOID direccion, const std::wstring& tipoDato, const std::wstring& valor);

void RefrescarValoresMemoria(HWND hList, HANDLE hProc);