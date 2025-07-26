#pragma once
#include <windows.h>
#include <string>

struct BuscarInstruccionesData {
    std::wstring direccionMemoria;
    std::wstring instrucciones; // para el recuadro de texto grande
};

void MostrarVentanaBuscarInstrucciones(HWND hParent, const std::wstring& direccion, const std::wstring& instrucciones);
INT_PTR CALLBACK BuscarInstruccionesDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
