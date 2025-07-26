#include "FiltroBusqueda.h"
#include "SeleccionarProcesoDialog.h"
#include "BusquedaMemoria.h"

// Crea los inputs búsqueda
void CrearInputBusquedaValores(
	HWND hWndParent, HINSTANCE hInst,
	HWND& hLblValue, HWND& hEditValue,
	int x, int y, int widthPanel)
{
	hLblValue = CreateWindowW(L"STATIC", L"Valor:", WS_VISIBLE | WS_CHILD,
		x, y, widthPanel, 30, hWndParent, NULL, hInst, NULL);

	hEditValue = CreateWindowW(L"EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER,
		x, y + 20, widthPanel, 24, hWndParent, (HMENU)1003, hInst, NULL);
}

// Cambia los inputs búsqueda de acuerdo al Tipo de Busqueda
void CambiarInputBusquedaValores(
	HWND hWndParent, HINSTANCE hInst,
	HWND& hLblValue, HWND& hEditValue,
	HWND& hLblValueEnd, HWND& hEditValueEnd,
	int x, int y, int widthPanel,
	int tipoBusqueda)
{
	// Destruimos los controles previos si existen
	if (hLblValue)      DestroyWindow(hLblValue);
	if (hEditValue)     DestroyWindow(hEditValue);
	if (hLblValueEnd)   DestroyWindow(hLblValueEnd);
	if (hEditValueEnd)  DestroyWindow(hEditValueEnd);

	hLblValue = nullptr;
	hEditValue = nullptr;
	hLblValueEnd = nullptr;
	hEditValueEnd = nullptr;

	if (tipoBusqueda == 3) { // Entre dos valores
		int widthHalf = widthPanel / 2 - 5;

		hLblValue = CreateWindowW(L"STATIC", L"Valor Inferior:", WS_VISIBLE | WS_CHILD,
			x, y, widthHalf, 30, hWndParent, NULL, hInst, NULL);
		hEditValue = CreateWindowW(L"EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER,
			x, y + 20, widthHalf, 24, hWndParent, (HMENU)1003, hInst, NULL);

		hLblValueEnd = CreateWindowW(L"STATIC", L"Valor Superior:", WS_VISIBLE | WS_CHILD,
			x + widthHalf + 10, y, widthHalf, 30, hWndParent, NULL, hInst, NULL);
		hEditValueEnd = CreateWindowW(L"EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER,
			x + widthHalf + 10, y + 20, widthHalf, 24, hWndParent, (HMENU)1009, hInst, NULL);
	}
	else {
		hLblValue = CreateWindowW(L"STATIC", L"Valor:", WS_VISIBLE | WS_CHILD,
			x, y, widthPanel, 30, hWndParent, NULL, hInst, NULL);
		hEditValue = CreateWindowW(L"EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER,
			x, y + 20, widthPanel, 24, hWndParent, (HMENU)1003, hInst, NULL);
	}
}

// Crea todos los controles del panel de búsqueda
void CrearFiltroBusquedaControles(
	HWND hWndParent, HINSTANCE hInst,
	HWND& hLblComboType, HWND& hComboType,
	HWND& hLblComboSearch, HWND& hComboSearch,
	HWND& hBtnSearch, HWND& hBtnReset,
	int x, int y, int widthPanel)
{
	hLblComboType = CreateWindowW(L"STATIC", L"Tipo dato:", WS_VISIBLE | WS_CHILD,
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

// Inicializa las opciones del ComboBox Filtro Tipo de Dato
void InicializarOpcionesFiltroTipoDato(HWND hComboType, int tipoDato)
{
	SendMessageW(hComboType, CB_RESETCONTENT, 0, 0); // Limpia opciones

	int idx = (int)SendMessageW(hComboType, CB_ADDSTRING, 0, (LPARAM)L"Byte");
	SendMessageW(hComboType, CB_SETITEMDATA, idx, (LPARAM)0); // 0 = Byte
	idx = (int)SendMessageW(hComboType, CB_ADDSTRING, 0, (LPARAM)L"2 Bytes");
	SendMessageW(hComboType, CB_SETITEMDATA, idx, (LPARAM)1); // 1 = 2 Bytes
	idx = (int)SendMessageW(hComboType, CB_ADDSTRING, 0, (LPARAM)L"4 Bytes");
	SendMessageW(hComboType, CB_SETITEMDATA, idx, (LPARAM)2); // 2 = 4 Bytes
	idx = (int)SendMessageW(hComboType, CB_ADDSTRING, 0, (LPARAM)L"8 Bytes");
	SendMessageW(hComboType, CB_SETITEMDATA, idx, (LPARAM)3); // 3 = 8 Bytes
	idx = (int)SendMessageW(hComboType, CB_ADDSTRING, 0, (LPARAM)L"Float");
	SendMessageW(hComboType, CB_SETITEMDATA, idx, (LPARAM)4); // 4 = Float
	idx = (int)SendMessageW(hComboType, CB_ADDSTRING, 0, (LPARAM)L"Double");
	SendMessageW(hComboType, CB_SETITEMDATA, idx, (LPARAM)5); // 5 = Double
	idx = (int)SendMessageW(hComboType, CB_ADDSTRING, 0, (LPARAM)L"String");
	SendMessageW(hComboType, CB_SETITEMDATA, idx, (LPARAM)6); // 6 = String
	idx = (int)SendMessageW(hComboType, CB_ADDSTRING, 0, (LPARAM)L"Hexadecimal");
	SendMessageW(hComboType, CB_SETITEMDATA, idx, (LPARAM)7); // 7 = Hexadecimal
	idx = (int)SendMessageW(hComboType, CB_ADDSTRING, 0, (LPARAM)L"Direccion Memoria");
	SendMessageW(hComboType, CB_SETITEMDATA, idx, (LPARAM)8); // 8 = Direccion Memoria	

	SendMessageW(hComboType, CB_SETCURSEL, tipoDato, 0);

	if (numEscaneo == 0) {
		EnableWindow(hComboType, TRUE);   // Habilitar para el primer escaneo
	}
	else {
		EnableWindow(hComboType, FALSE);  // Bloquear para escaneos sucesivos
	}
}

// Inicializa las opciones del ComboBox Filtro Tipo de Búsqueda
void InicializarOpcionesFiltroTipoBusqueda(HWND hComboSearch, int tipoDato, int tipoBusqueda)
{
	SendMessageW(hComboSearch, CB_RESETCONTENT, 0, 0); // Limpia opciones

	if (tipoDato == 6) { // String
		int idx = (int)SendMessageW(hComboSearch, CB_ADDSTRING, 0, (LPARAM)L"Buscar por texto");
		SendMessageW(hComboSearch, CB_SETITEMDATA, idx, (LPARAM)0); // 0 = Buscar por texto
	}
	else if (tipoDato == 8) { // Dirección Memoria
		int idx = (int)SendMessageW(hComboSearch, CB_ADDSTRING, 0, (LPARAM)L"Direccion exacta");
		SendMessageW(hComboSearch, CB_SETITEMDATA, idx, (LPARAM)0); // 0 = Direccion exacta
	}
	else {
		int idx = (int)SendMessageW(hComboSearch, CB_ADDSTRING, 0, (LPARAM)L"Valor exacto");
		SendMessageW(hComboSearch, CB_SETITEMDATA, idx, (LPARAM)0); // 0 = Valor exacto
		idx = (int)SendMessageW(hComboSearch, CB_ADDSTRING, 0, (LPARAM)L"Mayor que");
		SendMessageW(hComboSearch, CB_SETITEMDATA, idx, (LPARAM)1); // 1 = Mayor que
		idx = (int)SendMessageW(hComboSearch, CB_ADDSTRING, 0, (LPARAM)L"Menor que");
		SendMessageW(hComboSearch, CB_SETITEMDATA, idx, (LPARAM)2); // 2 = Menor que
		idx = (int)SendMessageW(hComboSearch, CB_ADDSTRING, 0, (LPARAM)L"Entre dos valores");
		SendMessageW(hComboSearch, CB_SETITEMDATA, idx, (LPARAM)3); // 3 = Entre dos valores

		// Primer escaneo
		if (numEscaneo == 0) {
			idx = (int)SendMessageW(hComboSearch, CB_ADDSTRING, 0, (LPARAM)L"Valor inicial desconocido");
			SendMessageW(hComboSearch, CB_SETITEMDATA, idx, (LPARAM)4); // 4 = Valor inicial desconocido
		}
		// Segundo escaneo en adelante
		else {
			idx = (int)SendMessageW(hComboSearch, CB_ADDSTRING, 0, (LPARAM)L"Valor incrementado");
			SendMessageW(hComboSearch, CB_SETITEMDATA, idx, (LPARAM)5); // 5 = Valor incrementado
			idx = (int)SendMessageW(hComboSearch, CB_ADDSTRING, 0, (LPARAM)L"Valor decrementado");
			SendMessageW(hComboSearch, CB_SETITEMDATA, idx, (LPARAM)6); // 6 = Valor decrementado
			idx = (int)SendMessageW(hComboSearch, CB_ADDSTRING, 0, (LPARAM)L"Valor cambiado");
			SendMessageW(hComboSearch, CB_SETITEMDATA, idx, (LPARAM)7); // 7 = Valor cambiado
			idx = (int)SendMessageW(hComboSearch, CB_ADDSTRING, 0, (LPARAM)L"Valor sin cambios");
			SendMessageW(hComboSearch, CB_SETITEMDATA, idx, (LPARAM)8); // 8 = Valor sin cambios
		}
	}

	if (tipoBusqueda == 4) {
		SendMessageW(hComboSearch, CB_SETCURSEL, 0, 0);
	}
	else {
		SeleccionarComboPorItemData(hComboSearch, tipoBusqueda);
	}

}

// Crea todos los controles del panel de búsqueda
void CambiarEstadoInputEditValue(HWND& hEditValue, HWND& hEditValueEnd, int tipoBusqueda)
{
	switch (tipoBusqueda) {
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
		EnableWindow(hEditValue, FALSE);     // Bloquear
		EnableWindow(hEditValueEnd, FALSE);  // Bloquear
		break;
	default:
		EnableWindow(hEditValue, TRUE);     // Habilitar
		EnableWindow(hEditValueEnd, TRUE);  // Habilitar
		break;
	}
}

void SeleccionarComboPorItemData(HWND hCombo, int itemDataBuscado) {
	int count = (int)SendMessageW(hCombo, CB_GETCOUNT, 0, 0);
	for (int i = 0; i < count; ++i) {
		int data = (int)SendMessageW(hCombo, CB_GETITEMDATA, i, 0);
		if (data == itemDataBuscado) {
			SendMessageW(hCombo, CB_SETCURSEL, i, 0);
			return;
		}
	}

	// Si no lo encontró, puedes poner CB_SETCURSEL(-1, 0) para deseleccionar
	SendMessageW(hCombo, CB_SETCURSEL, -1, 0);
}