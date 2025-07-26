#pragma once
#include <string>
#include <windows.h>

struct CambiarValorData {
    std::wstring valorActual;
    std::wstring nuevoValor;
    std::wstring titulo;
    std::wstring prompt;
};

struct VisualizarValorData {
    std::wstring valorActual;
    std::wstring titulo;
};

INT_PTR CALLBACK CambiarValorDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK VisualizarValorDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
