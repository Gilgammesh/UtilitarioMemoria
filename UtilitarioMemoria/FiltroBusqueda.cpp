#include "FiltroBusqueda.h"
#include "SeleccionarProcesoDialog.h"

// Crea todos los controles del panel de búsqueda
void CrearFiltroBusquedaControles(
    HWND hWndParent, HINSTANCE hInst,
    HWND& hLblValue, HWND& hEditValue,
    HWND& hLblComboType, HWND& hComboType,
    HWND& hLblComboSearch, HWND& hComboSearch,
    HWND& hBtnSearch, HWND& hBtnReset,
    int x, int y, int widthPanel)
{
    hLblValue = CreateWindowW(L"STATIC", L"Valor:", WS_VISIBLE | WS_CHILD,
        x, y, widthPanel, 30, hWndParent, NULL, hInst, NULL);

    hEditValue = CreateWindowW(L"EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER,
        x, y + 20, widthPanel, 24, hWndParent, (HMENU)1003, hInst, NULL);

    hLblComboType = CreateWindowW(L"STATIC", L"Tipo valor:", WS_VISIBLE | WS_CHILD,
        x, y + 50, widthPanel, 20, hWndParent, NULL, hInst, NULL);

    hComboType = CreateWindowW(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,
        x, y + 70, widthPanel, 220, hWndParent, (HMENU)1004, hInst, NULL);

    hLblComboSearch = CreateWindowW(L"STATIC", L"Tipo busqueda:", WS_VISIBLE | WS_CHILD,
        x, y + 100, widthPanel, 20, hWndParent, NULL, hInst, NULL);

    hComboSearch = CreateWindowW(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,
        x, y + 120, widthPanel, 220, hWndParent, (HMENU)1005, hInst, NULL);

    hBtnSearch = CreateWindowW(L"BUTTON", L"Buscar", WS_VISIBLE | WS_CHILD,
        x, y + 160, (widthPanel - 10) / 2, 28, hWndParent, (HMENU)1006, hInst, NULL);

    hBtnReset = CreateWindowW(L"BUTTON", L"Reiniciar", WS_VISIBLE | WS_CHILD,
        x + (widthPanel - 10) / 2 + 10, y + 160, (widthPanel - 10) / 2, 28, hWndParent, (HMENU)1007, hInst, NULL);
}

// Inicializa las opciones de los ComboBox
void InicializarOpcionesFiltros(HWND hComboType, HWND hComboSearch)
{
    // Tipo de dato
    int idx = (int)SendMessageW(hComboType, CB_ADDSTRING, 0, (LPARAM)L"String");
    SendMessageW(hComboType, CB_SETITEMDATA, idx, (LPARAM)0); // 0 = String
    idx = (int)SendMessageW(hComboType, CB_ADDSTRING, 0, (LPARAM)L"Byte");
    SendMessageW(hComboType, CB_SETITEMDATA, idx, (LPARAM)1); // 1 = Byte
    idx = (int)SendMessageW(hComboType, CB_ADDSTRING, 0, (LPARAM)L"2 Bytes");
    SendMessageW(hComboType, CB_SETITEMDATA, idx, (LPARAM)2); // 2 = 2 Bytes
    idx = (int)SendMessageW(hComboType, CB_ADDSTRING, 0, (LPARAM)L"4 Bytes");
    SendMessageW(hComboType, CB_SETITEMDATA, idx, (LPARAM)3); // 3 = 4 Bytes
    idx = (int)SendMessageW(hComboType, CB_ADDSTRING, 0, (LPARAM)L"8 Bytes");
    SendMessageW(hComboType, CB_SETITEMDATA, idx, (LPARAM)4); // 4 = 8 Bytes
    idx = (int)SendMessageW(hComboType, CB_ADDSTRING, 0, (LPARAM)L"Float");
    SendMessageW(hComboType, CB_SETITEMDATA, idx, (LPARAM)5); // 5 = Float
    idx = (int)SendMessageW(hComboType, CB_ADDSTRING, 0, (LPARAM)L"Double");
    SendMessageW(hComboType, CB_SETITEMDATA, idx, (LPARAM)6); // 6 = Double
    idx = (int)SendMessageW(hComboType, CB_ADDSTRING, 0, (LPARAM)L"Dirección Memoria");
    SendMessageW(hComboType, CB_SETITEMDATA, idx, (LPARAM)7); // 7 = Dirección Memoria
    SendMessageW(hComboType, CB_SETCURSEL, 0, 0);

    // Tipo de búsqueda
    idx = (int)SendMessageW(hComboSearch, CB_ADDSTRING, 0, (LPARAM)L"Valor exacto");
    SendMessageW(hComboSearch, CB_SETITEMDATA, idx, (LPARAM)0); // 0 = Exacto
    idx = (int)SendMessageW(hComboSearch, CB_ADDSTRING, 0, (LPARAM)L"Mayor que");
    SendMessageW(hComboSearch, CB_SETITEMDATA, idx, (LPARAM)1); // 1 = Mayor que
    idx = (int)SendMessageW(hComboSearch, CB_ADDSTRING, 0, (LPARAM)L"Menor que");
    SendMessageW(hComboSearch, CB_SETITEMDATA, idx, (LPARAM)2); // 2 = Menor que
    idx = (int)SendMessageW(hComboSearch, CB_ADDSTRING, 0, (LPARAM)L"Entre dos valores");
    SendMessageW(hComboSearch, CB_SETITEMDATA, idx, (LPARAM)3); // 3 = Entre dos valores
    SendMessageW(hComboSearch, CB_SETCURSEL, 0, 0);
}
