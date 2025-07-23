#include "resource.h"
#include "DialogoCambiarValor.h"

INT_PTR CALLBACK CambiarValorDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    CambiarValorData* pData = reinterpret_cast<CambiarValorData*>(GetWindowLongPtr(hDlg, GWLP_USERDATA));

    switch (message)
    {
    case WM_INITDIALOG:
    {
        SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);
        pData = reinterpret_cast<CambiarValorData*>(lParam);

        SetWindowTextW(hDlg, pData->titulo.c_str());
        SetDlgItemTextW(hDlg, IDC_STATIC, pData->prompt.c_str());
        SetDlgItemTextW(hDlg, IDC_EDIT_NUEVO_VALOR, pData->valorActual.c_str());
        SetFocus(GetDlgItem(hDlg, IDC_EDIT_NUEVO_VALOR));
        return FALSE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        {
            wchar_t buffer[256] = { 0 };
            GetDlgItemTextW(hDlg, IDC_EDIT_NUEVO_VALOR, buffer, 255);
            pData->nuevoValor = buffer;
            EndDialog(hDlg, IDOK);
            return TRUE;
        }
        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}
