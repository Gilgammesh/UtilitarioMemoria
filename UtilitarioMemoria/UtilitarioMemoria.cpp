// UtilitarioMemoria.cpp : Define el punto de entrada de la aplicacion.
//
#include <windows.h>
#include <windowsx.h>
#include <tlhelp32.h>
#include <commctrl.h>
#include <sstream>
#include <iomanip>
#include <thread>
#include <atomic>
#include <mutex>
#include <iostream>
#include <string>
#include "resource.h"
#include "SeleccionarProceso.h"
#include "SeleccionarProcesoDialog.h"
#include "ListaBusqueda.h"
#include "FiltroBusqueda.h"
#include "ListaMemoria.h"
#include "BusquedaMemoria.h"
#include "DialogoCambiarValor.h"
#include "ConfiguracionFavoritos.h"
#include "BuscarInstruccionesModificanMemoria.h"
#include "Globals.h"
#pragma comment(lib, "comctl32.lib")

#define MAX_LOADSTRING 100

// Declaraciones para refresco de memoria
std::thread refresherThread;
std::atomic<bool> refresherRunning{ false };

// Variables globales:
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING] = L"UtilitarioMemoria";
WCHAR szWindowClass[MAX_LOADSTRING] = L"UtilitarioMemoriaClass";

// Handles globales para controles
HWND hLblBtnIcon, hBtnSelectIcon, hBtnSaveConfig, hBtnLoadConfig, hLblCurrentProcess, hLblTotalRegistros, hLblNroEscaneos, hListMain, hLblValue, hEditValue, hLblValueEnd, hEditValueEnd, hLblComboType, hComboType, hLblComboSearch, hComboSearch, hBtnSearch, hBtnReset, hLblSeparator, hListFreeze;

// Variables globales
extern wchar_t currentProcessName[MAX_PATH];
extern std::vector<FavoritoMemoria> listaMemoriaFavoritos;

// Declaraciones de funciones
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

void IniciarRefresherFavoritos(HWND hList, HANDLE hProc) {
	refresherRunning = true;
	refresherThread = std::thread([hList, hProc]() {
		RefrescarValoresMemoria(hList, hProc);
		});
}

void DetenerRefresherFavoritos() {
	refresherRunning = false;
	if (refresherThread.joinable()) {
		refresherThread.join();
	}
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Inicializar controles comunes
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(icex);
	icex.dwICC = ICC_LISTVIEW_CLASSES | ICC_TAB_CLASSES | ICC_BAR_CLASSES;
	InitCommonControlsEx(&icex);

	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
		return FALSE;

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex = { 0 };

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 900, 600, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
		return FALSE;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

void InitListViewColumns(HWND hList, const wchar_t* const* headers, const int* widths, int count)
{
	LVCOLUMNW lvc = { 0 };
	lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	for (int i = 0; i < count; ++i)
	{
		lvc.pszText = (LPWSTR)headers[i];
		lvc.cx = widths[i];
		lvc.iSubItem = i;
		ListView_InsertColumn(hList, i, &lvc);
	}
}

// -----------------------------------------
// VENTANA PRINCIPAL
// -----------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		// 1ra seccion: Seleccionar proceso
		RECT rc;
		GetClientRect(hWnd, &rc);
		int width = rc.right - rc.left;
		CrearSeleccionarProcesoControles(hWnd, hInst, hLblBtnIcon, hBtnSelectIcon, hBtnSaveConfig, hBtnLoadConfig, hLblCurrentProcess, currentProcessName, width);

		// 2da seccion: Lista de búsqueda (Tabla de búsqueda)
		int widthTotalReg = 200;
		int widthNroScan = 200;
		hLblTotalRegistros = CrearLabelTotalRegistros(hWnd, hInst, 10, 36, widthTotalReg, 22, 2001);
		hLblNroEscaneos = CrearLabelNroEscaneos(hWnd, hInst, widthTotalReg + 20, 36, widthNroScan, 22, 2002);
		hListMain = CrearListViewPrincipal(hWnd, hInst, 10, 60, 600, 220, 1002);
		InicializarColumnasListView(hListMain);

		// 3ra seccion: Panel de búsqueda (Opciones de búsqueda)
		CrearInputBusquedaValores(
			hWnd, hInst,
			hLblValue, hEditValue,
			620, 60, 250);
		CrearFiltroBusquedaControles(
			hWnd, hInst,
			hLblComboType, hComboType,
			hLblComboSearch, hComboSearch,
			hBtnSearch, hBtnReset,
			620, 60, 250);
		InicializarOpcionesFiltroTipoDato(hComboType, 2);
		InicializarOpcionesFiltroTipoBusqueda(hComboSearch);

		// 4ta seccion: Lista de memoria (Tabla para seleccionar y trabajar con memoria)
		hLblSeparator = CreateWindowW(L"STATIC", L"", WS_VISIBLE | WS_CHILD,
			10, 300, 860, 240, hWnd, NULL, hInst, NULL);
		hListFreeze = CrearListViewMemoria(hWnd, hInst, 10, 320, 860, 240, 1008);
		InicializarColumnasListViewMemoria(hListFreeze);

		break;
	}
	case WM_SIZE:
	{
		int width = LOWORD(lParam);
		int height = HIWORD(lParam);

		// Panel superior (boton icono)
		MoveWindow(hLblBtnIcon, 10, 10, 90, 40, TRUE);
		MoveWindow(hBtnSelectIcon, 100, 10, 40, 40, TRUE);
		MoveWindow(hBtnSaveConfig, 140, 10, 40, 40, TRUE);
		MoveWindow(hBtnLoadConfig, 180, 10, 40, 40, TRUE);
		int xCurrent = 240;
		int widthCurrent = width - xCurrent - 20;
		MoveWindow(hLblCurrentProcess, xCurrent, 15, widthCurrent, 30, TRUE);

		// ListView principal
		int widthTotalReg = 200;
		int widthNroScan = 200;
		MoveWindow(hLblTotalRegistros, 10, 60, widthTotalReg, 22, TRUE);
		MoveWindow(hLblNroEscaneos, widthTotalReg + 20, 60, widthNroScan, 22, TRUE);
		MoveWindow(hListMain, 10, 85, static_cast<int>(width * 0.7) - 20, 220, TRUE);

		// Panel derecho (búsqueda)
		int panelX = static_cast<int>(width * 0.7);
		int panelW = width - panelX - 20;
		MoveWindow(hLblValue, panelX, 60, panelW, 20, TRUE);
		MoveWindow(hEditValue, panelX, 80, panelW, 24, TRUE);
		MoveWindow(hLblComboType, panelX, 110, panelW, 20, TRUE);
		MoveWindow(hComboType, panelX, 130, panelW, 24, TRUE);
		MoveWindow(hLblComboSearch, panelX, 160, panelW, 20, TRUE);
		MoveWindow(hComboSearch, panelX, 180, panelW, 24, TRUE);

		int btnW = (panelW - 10) / 2;
		MoveWindow(hBtnSearch, panelX, 220, btnW, 28, TRUE);
		MoveWindow(hBtnReset, panelX + btnW + 10, 220, btnW, 28, TRUE);

		// ListView secundaria (abajo)
		MoveWindow(hLblSeparator, 10, 310, width - 20, 20, TRUE);
		MoveWindow(hListFreeze, 10, 330, width - 20, height - 310, TRUE);
		break;
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		int wmEvent = HIWORD(wParam);

		if ((HWND)lParam == hComboType && wmEvent == CBN_SELCHANGE) {
			int idxType = (int)SendMessageW(hComboType, CB_GETCURSEL, 0, 0);
			int typeId = (int)SendMessageW(hComboType, CB_GETITEMDATA, idxType, 0);
			InicializarOpcionesFiltroTipoDato(hComboType, typeId);
			InicializarOpcionesFiltroTipoBusqueda(hComboSearch, typeId);
		}

		if ((HWND)lParam == hComboSearch && wmEvent == CBN_SELCHANGE) {
			int idxSearch = (int)SendMessageW(hComboSearch, CB_GETCURSEL, 0, 0);
			int searchId = (int)SendMessageW(hComboSearch, CB_GETITEMDATA, idxSearch, 0);			
			CambiarInputBusquedaValores(
				hWnd, hInst,
				hLblValue, hEditValue,
				hLblValueEnd, hEditValueEnd,
				620, 60, 244,
				searchId
			);
			CambiarEstadoInputEditValue(hEditValue, hEditValueEnd, searchId);
		}

		switch (wmId)
		{
		case 1001: // Boton seleccionar proceso
		{
			ShowProcessListDialog(hWnd, hLblCurrentProcess);

			// Detener refresher anterior si lo hay
			DetenerRefresherFavoritos();

			// Obtiene el nuevo handle seleccionado
			HANDLE hProc = GetSelectedProcessHandle();
			if (hProc && hProc != INVALID_HANDLE_VALUE) {
				IniciarRefresherFavoritos(hListFreeze, hProc);
			}
			break;
		}
		case 1010: // Boton guardar configuracion
		{
			GuardarFavoritosEnArchivo();
			break;
		}
		case 1011: // Boton cargar configuracion
		{
			DetenerRefresherFavoritos();

			if (refresherRunning) {
				MessageBoxW(NULL, L"Espere a que termine la actualizacion automatica antes de cargar configuracion.", L"Error", MB_OK | MB_ICONERROR);
				return 0;
			}

			HANDLE hProc = GetSelectedProcessHandle();
			if (!hProc) {
				MessageBoxW(NULL, L"Debes seleccionar un proceso primero.", L"Error", MB_OK | MB_ICONERROR);
				return 0;
			}

			CargarFavoritosDeArchivo(hListFreeze);

			if (hProc && hProc != INVALID_HANDLE_VALUE) {
				IniciarRefresherFavoritos(hListFreeze, hProc);
			}
			break;
		}
		case 1006: // Boton buscar registros en memoria
		{
			HANDLE hProc = GetSelectedProcessHandle();
			if (!hProc) {
				MessageBoxW(NULL, L"Debes seleccionar un proceso primero.", L"Error", MB_OK | MB_ICONERROR);
				return 0;
			}

			int idxType = (int)SendMessageW(hComboType, CB_GETCURSEL, 0, 0);
			int typeId = (int)SendMessageW(hComboType, CB_GETITEMDATA, idxType, 0);

			int idxSearch = (int)SendMessageW(hComboSearch, CB_GETCURSEL, 0, 0);
			int searchId = (int)SendMessageW(hComboSearch, CB_GETITEMDATA, idxSearch, 0);

			wchar_t valor[256];
			GetWindowTextW(hEditValue, valor, 255);

			wchar_t valorEnd[256] = { 0 };
			if (hEditValueEnd) {
				GetWindowTextW(hEditValueEnd, valorEnd, 255);
			}

			BuscarEnMemoria(hProc, valor, valorEnd, typeId, searchId);
			InicializarOpcionesFiltroTipoDato(hComboType, typeId);
			InicializarOpcionesFiltroTipoBusqueda(hComboSearch, typeId, searchId);
			CambiarEstadoInputEditValue(hEditValue, hEditValueEnd, searchId);
			break;
		}
		case 1007: // Boton reiniciar búsqueda
		{
			int idxType = (int)SendMessageW(hComboType, CB_GETCURSEL, 0, 0);
			int typeId = (int)SendMessageW(hComboType, CB_GETITEMDATA, idxType, 0);
			ResetearBusquedaMemoria();
			ListView_DeleteAllItems(hListMain);
			if (hLblTotalRegistros) {
				SetWindowTextW(hLblTotalRegistros, L" Total de registros: 0");
			}
			if (hLblNroEscaneos) {
				SetWindowTextW(hLblNroEscaneos, L" Numero de escaneos: 0");
			}
			InicializarOpcionesFiltroTipoDato(hComboType, typeId);
			InicializarOpcionesFiltroTipoBusqueda(hComboSearch, typeId);
			CambiarEstadoInputEditValue(hEditValue, hEditValueEnd);
			SetWindowTextW(hEditValue, L"");
			SetWindowTextW(hEditValueEnd, L"");
			CambiarInputBusquedaValores(
				hWnd, hInst,
				hLblValue, hEditValue,
				hLblValueEnd, hEditValueEnd,
				620, 60, 244,
				0
			);
			break;
		}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		break;
	}
	case WM_NOTIFY:
	{
		LPNMHDR lpnmh = (LPNMHDR)lParam;

		// Evento para agregar a la lista de memoria el registro de la lista de busqueda
		if (lpnmh->hwndFrom == hListMain && lpnmh->code == NM_DBLCLK)
		{
			LPNMITEMACTIVATE pnmItem = (LPNMITEMACTIVATE)lParam;
			int iItem = pnmItem->iItem;
			if (iItem >= 0)
			{
				wchar_t bufferDir[32] = { 0 };
				wchar_t bufferTipo[32] = { 0 };
				wchar_t bufferValor[128] = { 0 };

				ListView_GetItemText(hListMain, iItem, 0, bufferDir, 31);
				ListView_GetItemText(hListMain, iItem, 1, bufferTipo, 31);
				ListView_GetItemText(hListMain, iItem, 3, bufferValor, 127);

				// Convierte el texto hexadecimal a size_t
				size_t direccionNum = 0;
				std::wistringstream ss(bufferDir);
				ss >> std::hex >> direccionNum;

				// Guarda la direccion en la variable global
				selectedMemory = (LPCVOID)direccionNum;

				// Llama a la funcion y pasa los nuevos parametros
				AgregarAFavoritos(hListFreeze, selectedMemory, bufferTipo, bufferValor);
			}
		}

		// Evento para congelar el valor de un registro de memoria en lista de memoria
		if (lpnmh->hwndFrom == hListFreeze && lpnmh->code == LVN_ITEMCHANGED)
		{
			if (g_actualizandoUI)
				return 0; // Ignora eventos generados por la carga masiva

			NMLISTVIEW* pnmv = (NMLISTVIEW*)lParam;
			if (pnmv->uChanged & LVIF_STATE) {
				BOOL checked = ListView_GetCheckState(hListFreeze, pnmv->iItem);
				{
					std::lock_guard<std::mutex> lock(mutexFavoritos);
					if (pnmv->iItem < (int)listaMemoriaFavoritos.size()) {
						listaMemoriaFavoritos[pnmv->iItem].congelado = checked;
					}
				}
			}
		}

		// Evento para eliminar un registro de memoria en lista de memoria
		if (lpnmh->hwndFrom == hListFreeze && lpnmh->code == LVN_KEYDOWN)
		{
			LPNMLVKEYDOWN plvkd = (LPNMLVKEYDOWN)lParam;
			if (plvkd->wVKey == VK_DELETE) {
				int iSel = ListView_GetNextItem(hListFreeze, -1, LVNI_SELECTED);
				if (iSel >= 0) {
					int ret = MessageBoxW(hWnd, L"Desea eliminar el registro seleccionado?", L"Confirmar eliminacion", MB_YESNO | MB_ICONQUESTION);
					if (ret == IDYES) {
						{
							std::lock_guard<std::mutex> lock(mutexFavoritos);
							if (iSel < (int)listaMemoriaFavoritos.size()) {
								listaMemoriaFavoritos.erase(listaMemoriaFavoritos.begin() + iSel);
							}
						}
						ListView_DeleteItem(hListFreeze, iSel);
					}
				}
			}
		}

		// Evento para modificar un registro de memoria en lista de memoria
		if (lpnmh->hwndFrom == hListFreeze && lpnmh->code == NM_DBLCLK)
		{
			LPNMITEMACTIVATE pnmItem = (LPNMITEMACTIVATE)lParam;
			int iItem = pnmItem->iItem;
			if (iItem >= 0)
			{
				POINT pt = pnmItem->ptAction;

				LVHITTESTINFO lvhti = { 0 };
				lvhti.pt = pt;
				int hit = ListView_SubItemHitTest(hListFreeze, &lvhti);

				// Modifica la Columna 1 - "Descripcion" en Lista de Memoria
				if (lvhti.iItem >= 0 && lvhti.iSubItem == 1)
				{
					// Obtener descripcion actual
					wchar_t bufferDesc[256] = { 0 };
					ListView_GetItemText(hListFreeze, lvhti.iItem, 1, bufferDesc, 255);

					CambiarValorData dlgData;
					dlgData.titulo = L"Cambiar Descripcion";
					dlgData.prompt = L"Introduzca nueva descripcion a cambiar";
					dlgData.valorActual = bufferDesc;

					INT_PTR ret = DialogBoxParam(
						hInst,
						MAKEINTRESOURCE(IDD_CAMBIAR_VALOR),
						hWnd,
						CambiarValorDlgProc,
						(LPARAM)&dlgData
					);

					if (ret == IDOK && dlgData.nuevoValor != dlgData.valorActual) {
						ListView_SetItemText(hListFreeze, lvhti.iItem, 1, (LPWSTR)dlgData.nuevoValor.c_str());
						{
							std::lock_guard<std::mutex> lock(mutexFavoritos);
							if (lvhti.iItem < (int)listaMemoriaFavoritos.size()) {
								listaMemoriaFavoritos[lvhti.iItem].descripcion = dlgData.nuevoValor;
							}
						}
					}
				}

				// Visualizar la Columna 2 - "Direccion Memoria"
				if (lvhti.iItem >= 0 && lvhti.iSubItem == 2)
				{
					wchar_t bufferMemory[128] = { 0 };
					ListView_GetItemText(hListFreeze, lvhti.iItem, 2, bufferMemory, 127);

					VisualizarValorData dlgData;
					dlgData.titulo = L"Direccion de Memoria";
					dlgData.valorActual = bufferMemory;

					INT_PTR ret = DialogBoxParam(
						hInst,
						MAKEINTRESOURCE(IDD_VISUALIZAR_VALOR),
						hWnd,
						VisualizarValorDlgProc,
						(LPARAM)&dlgData
					);
				}

				// Modifica la Columna 4 - "Valor" en Memoria
				if (lvhti.iItem >= 0 && lvhti.iSubItem == 4)
				{
					wchar_t bufferValor[128] = { 0 };
					ListView_GetItemText(hListFreeze, lvhti.iItem, 4, bufferValor, 127);

					CambiarValorData dlgData;
					dlgData.titulo = L"Cambiar Valor";
					dlgData.prompt = L"Introduzca nuevo valor a cambiar";
					dlgData.valorActual = bufferValor;

					INT_PTR ret = DialogBoxParam(
						hInst,
						MAKEINTRESOURCE(IDD_CAMBIAR_VALOR),
						hWnd,
						CambiarValorDlgProc,
						(LPARAM)&dlgData
					);

					if (ret == IDOK && dlgData.nuevoValor != dlgData.valorActual) {
						// Obtener direccion (columna 2)
						wchar_t bufferDir[32] = { 0 };
						ListView_GetItemText(hListFreeze, lvhti.iItem, 2, bufferDir, 31);
						size_t direccionNum = 0;
						std::wistringstream ss(bufferDir);
						ss >> std::hex >> direccionNum;
						LPVOID direccion = (LPVOID)direccionNum;

						// Obtener tipo (columna 3)
						wchar_t bufferTipo[32] = { 0 };
						ListView_GetItemText(hListFreeze, lvhti.iItem, 3, bufferTipo, 31);
						std::wstring tipoDato = bufferTipo;

						// Obtener handle del proceso
						HANDLE hProc = GetSelectedProcessHandle();

						// Guardar nuevo valor según el tipo
						SIZE_T bytesWritten = 0;
						bool exito = false;
						if (tipoDato == L"String") {
							std::string s(dlgData.nuevoValor.begin(), dlgData.nuevoValor.end());
							exito = hProc && WriteProcessMemory(hProc, direccion, s.c_str(), s.size() + 1, &bytesWritten);
						}
						else if (tipoDato == L"Int8") {
							char v = (char)_wtoi(dlgData.nuevoValor.c_str());
							exito = hProc && WriteProcessMemory(hProc, direccion, &v, sizeof(v), &bytesWritten) && bytesWritten == sizeof(v);
						}
						else if (tipoDato == L"Int16") {
							short v = (short)_wtoi(dlgData.nuevoValor.c_str());
							exito = hProc && WriteProcessMemory(hProc, direccion, &v, sizeof(v), &bytesWritten) && bytesWritten == sizeof(v);
						}
						else if (tipoDato == L"Int32") {
							int v = _wtoi(dlgData.nuevoValor.c_str());
							exito = hProc && WriteProcessMemory(hProc, direccion, &v, sizeof(v), &bytesWritten) && bytesWritten == sizeof(v);
						}
						else if (tipoDato == L"Int64") {
							long long v = _wtoll(dlgData.nuevoValor.c_str());
							exito = hProc && WriteProcessMemory(hProc, direccion, &v, sizeof(v), &bytesWritten) && bytesWritten == sizeof(v);
						}
						else if (tipoDato == L"Float") {
							float v = (float)_wtof(dlgData.nuevoValor.c_str());
							exito = hProc && WriteProcessMemory(hProc, direccion, &v, sizeof(v), &bytesWritten) && bytesWritten == sizeof(v);
						}
						else if (tipoDato == L"Double") {
							double v = _wtof(dlgData.nuevoValor.c_str());
							exito = hProc && WriteProcessMemory(hProc, direccion, &v, sizeof(v), &bytesWritten) && bytesWritten == sizeof(v);
						}

						if (exito) {
							ListView_SetItemText(hListFreeze, lvhti.iItem, 4, (LPWSTR)dlgData.nuevoValor.c_str());
							{
								std::lock_guard<std::mutex> lock(mutexFavoritos);
								if (lvhti.iItem < (int)listaMemoriaFavoritos.size()) {
									listaMemoriaFavoritos[lvhti.iItem].valor = dlgData.nuevoValor;
								}
							}
						}
					}
				}				
			}
		}

		break;
	}
	case WM_CONTEXTMENU: {
		HWND hwndFrom = (HWND)wParam;
		// Solo nos interesa el hListFreeze
		if (hwndFrom == hListFreeze) {
			// Obtén posición del cursor
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);

			// Si el click fue por teclado, obten la posición del item seleccionado
			if (x == -1 && y == -1) {
				int iSel = ListView_GetNextItem(hListFreeze, -1, LVNI_SELECTED);
				if (iSel >= 0) {
					RECT rc;
					ListView_GetItemRect(hListFreeze, iSel, &rc, LVIR_BOUNDS);
					POINT pt = { rc.left, rc.bottom };
					ClientToScreen(hListFreeze, &pt);
					x = pt.x;
					y = pt.y;
				}
				else {
					break;
				}
			}

			// Cargar y mostrar el menú
			HMENU hMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU_MEMORIA_POPUP));
			if (hMenu) {
				HMENU hPopup = GetSubMenu(hMenu, 0);
				SetForegroundWindow(hWnd); // Necesario para evitar bugs visuales
				int cmd = TrackPopupMenu(
					hPopup,
					TPM_RETURNCMD | TPM_LEFTALIGN | TPM_TOPALIGN,
					x, y, 0, hWnd, NULL
				);
				if (cmd) {
					// Obtén el ítem seleccionado
					int iSel = ListView_GetNextItem(hListFreeze, -1, LVNI_SELECTED);
					if (iSel >= 0) {
						switch (cmd) {
						case ID_POPUP_FIND_ACCESS:
							MessageBoxW(hWnd, L"Buscar instrucciones que acceden...", L"Acción", MB_OK);
							break;
						case ID_POPUP_FIND_WRITE:
						{
							// Obtener la dirección de memoria seleccionada (columna 2)
							wchar_t bufferDir[32] = { 0 };
							ListView_GetItemText(hListFreeze, iSel, 2, bufferDir, 31);


							// Ejemplo de texto de instrucciones (puedes pasar "" inicialmente o lo que tengas)
							std::wstring instrucciones = 
								L"1000473E8:\r\n"
								L"1000473DE - 48 8B 80 E0070000  - mov rax,[rax+000007E0]\r\n"
								L"1000473E5 - 8B 55 E8  - mov edx,[rbp-18]\r\n"
								L"1000473E8 - 89 10  - mov [rax],edx <<\r\n"
								L"1000473EA - 48 8B 45 F0  - mov rax,[rbp-10]\r\n"
								L"1000473EE - 48 8B 80 E0070000  - mov rax,[rax+000007E0]\r\n"
								L"\r\n"
								L"RAX=000000000010AD70\r\n"
								L"RBX=0000000000000000\r\n"
								L"RCX=000000004E9B0EC7\r\n"
								L"RDX=0000000000000133\r\n"
								L"...";

							// Llama a la ventana personalizada
							MostrarVentanaBuscarInstrucciones(hWnd, bufferDir, instrucciones);
							break;
						}
						}
					}
				}
				DestroyMenu(hMenu);
			}
			return 0;
		}
		break;
	}

	case WM_DESTROY:
		DetenerRefresherFavoritos();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}