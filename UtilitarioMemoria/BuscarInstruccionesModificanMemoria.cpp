#include "BuscarInstruccionesModificanMemoria.h"
#include "resource.h"
#include <commctrl.h>

void MostrarVentanaBuscarInstrucciones(HWND hParent, const std::wstring& direccion, const std::wstring& instrucciones) {
	BuscarInstruccionesData data;
	data.direccionMemoria = direccion;
	data.instrucciones = instrucciones;
	DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_BUSCAR_INSTRUCCIONES), hParent, BuscarInstruccionesDlgProc, (LPARAM)&data);
}

INT_PTR CALLBACK BuscarInstruccionesDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static BuscarInstruccionesData* pData = nullptr;

    switch (message) {
    case WM_INITDIALOG: {
        pData = reinterpret_cast<BuscarInstruccionesData*>(lParam);

        // Título dinámico con dirección
        std::wstring titulo = L"Instrucciones que modifican la memoria " + pData->direccionMemoria;
        SetWindowTextW(hDlg, titulo.c_str());

        // ListView columna "Instrucción"
        HWND hList = GetDlgItem(hDlg, IDC_LIST_INSTRUCCIONES);
        LVCOLUMNW lvc = { 0 };
        lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        lvc.pszText = (LPWSTR)L"Instrucción";
        lvc.cx = 350;
        lvc.iSubItem = 0;
        ListView_InsertColumn(hList, 0, &lvc);

        // Recuadro de texto (multiline)
        HWND hEdit = GetDlgItem(hDlg, IDC_EDIT_LOG);
        SetWindowTextW(hEdit, pData->instrucciones.c_str());
        return TRUE;
    }
    case WM_CLOSE:
        EndDialog(hDlg, IDOK);
        return TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, IDOK);
            return TRUE;
        }
        break;
    }
    return FALSE;
}
