#pragma once
#include <windows.h>

// Declaraciones de las funciones y handles exportados
void CrearInputBusquedaValores(
    HWND hWndParent, HINSTANCE hInst,
    HWND& hLblValue, HWND& hEditValue,
    int x, int y, int widthPanel);
void CambiarInputBusquedaValores(
    HWND hWndParent, HINSTANCE hInst,
    HWND& hLblValue, HWND& hEditValue,
    HWND& hLblValueEnd, HWND& hEditValueEnd,
    int x, int y, int widthPanel,
    int tipoBusqueda = 0);
void CrearFiltroBusquedaControles(
    HWND hWndParent, HINSTANCE hInst,
    HWND& hLblComboType, HWND& hComboType,
    HWND& hLblComboSearch, HWND& hComboSearch,
    HWND& hBtnSearch, HWND& hBtnReset,
    int x, int y, int widthPanel);

void InicializarOpcionesFiltroTipoDato(HWND hComboType, int tipoDato = 0);
void InicializarOpcionesFiltroTipoBusqueda(HWND hComboSearch, int tipoDato = 0, int tipoBusqueda = 0);
void CambiarEstadoInputEditValue(HWND& hEditValue, HWND& hEditValueEnd, int tipoBusqueda = 0);
void SeleccionarComboPorItemData(HWND hCombo, int itemDataBuscado);
