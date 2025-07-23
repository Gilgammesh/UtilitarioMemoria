#pragma once
#include <string>
#include <windows.h>

struct CambiarValorData {
    std::wstring valorActual;
    std::wstring nuevoValor;
    std::wstring titulo;
    std::wstring prompt;
};

INT_PTR CALLBACK CambiarValorDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
