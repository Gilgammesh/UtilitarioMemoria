#pragma once
#include <windows.h>

// Declaraciones de las funciones y handles exportados
void CrearFiltroBusquedaControles(
    HWND hWndParent, HINSTANCE hInst,
    HWND& hLblValue, HWND& hEditValue,
    HWND& hLblComboType, HWND& hComboType,
    HWND& hLblComboSearch, HWND& hComboSearch,
    HWND& hBtnSearch, HWND& hBtnReset,
    int x, int y, int widthPanel);

void InicializarOpcionesFiltros(HWND hComboType, HWND hComboSearch);
