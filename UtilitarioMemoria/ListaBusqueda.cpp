#include "ListaBusqueda.h"
#include <commctrl.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <limits>

// Crea el Label de total de registros
HWND CrearLabelTotalRegistros(HWND hWndParent, HINSTANCE hInst, int x, int y, int width, int height, int id)
{
	HWND hLabel = CreateWindowW(L"STATIC", L" Total de registros: 0", WS_CHILD | WS_VISIBLE,
		x, y, width, height, hWndParent, (HMENU)(UINT_PTR)id, hInst, NULL);
	return hLabel;
}

HWND CrearLabelNroEscaneos(HWND hWndParent, HINSTANCE hInst, int x, int y, int width, int height, int id)
{
    HWND hLabel = CreateWindowW(L"STATIC", L"  Numero de escaneos: 0", WS_CHILD | WS_VISIBLE,
        x, y, width, height, hWndParent, (HMENU)(UINT_PTR)id, hInst, NULL);
    return hLabel;
}

// Crea el ListView principal
HWND CrearListViewPrincipal(HWND hWndParent, HINSTANCE hInst, int x, int y, int width, int height, int id)
{
	HWND hList = CreateWindowW(WC_LISTVIEWW, NULL,
		WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SINGLESEL | WS_BORDER,
		x, y, width, height, hWndParent, (HMENU)(UINT_PTR)id, hInst, NULL);
	return hList;
}

// Inicializa las columnas del ListView principal
void InicializarColumnasListView(HWND hListView)
{
	const wchar_t* headers[] = { L"Direccion", L"Tipo", L"Valor previo", L"Valor actual" };
	int widths[] = { 140, 100, 160, 160 };
	LVCOLUMNW lvc = { 0 };
	lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	for (int i = 0; i < 4; ++i)
	{
		lvc.pszText = (LPWSTR)headers[i];
		lvc.cx = widths[i];
		lvc.iSubItem = i;
		ListView_InsertColumn(hListView, i, &lvc);
	}
}

std::wstring PointerToHexString(LPCVOID addr)
{
	std::wstringstream ss;
	ss << L"0x" << std::hex << std::uppercase << reinterpret_cast<size_t>(addr);
	return ss.str();
}

std::wstring PointerToHexStringNoPrefix(LPCVOID addr) {
    std::wstringstream ss;
    ss << std::uppercase << std::setfill(L'0') << std::setw(sizeof(void*) * 2)
        << std::hex << reinterpret_cast<size_t>(addr);
    return ss.str();
}

// Funcion sobrecargada: actualiza la tabla para valores strings y tambien el label de total
void ActualizarListViewPrincipalString(HWND hListView, const std::vector<ScanResultString>& resultados, HWND hLabelTotal)
{
    for (const auto& res : resultados)
    {
        LVITEMW item = { 0 };
        item.mask = LVIF_TEXT;
        item.iItem = ListView_GetItemCount(hListView);

        std::wstring direccion = PointerToHexStringNoPrefix(res.direccion);
        item.pszText = (LPWSTR)direccion.c_str();
        item.iSubItem = 0;
        ListView_InsertItem(hListView, &item);

        ListView_SetItemText(hListView, item.iItem, 1, (LPWSTR)L"String");

        std::wstring valorPrevioW(res.valorPrevio.begin(), res.valorPrevio.end());
        ListView_SetItemText(hListView, item.iItem, 2, (LPWSTR)valorPrevioW.c_str());

        std::wstring valorActualW(res.valorActual.begin(), res.valorActual.end());
        ListView_SetItemText(hListView, item.iItem, 3, (LPWSTR)valorActualW.c_str());
    }

    // --- ACTUALIZA EL LABEL ---
    if (hLabelTotal) {
        std::wstring txt = L" Total de registros: " + std::to_wstring(resultados.size());
        SetWindowTextW(hLabelTotal, txt.c_str());
    }
}

// Sobrecarga para compatibilidad (por si en algún lugar no tienes label)
void ActualizarListViewPrincipalString(HWND hListView, const std::vector<ScanResultString>& resultados)
{
    ActualizarListViewPrincipalString(hListView, resultados, NULL);
}

// Funcion sobrecargada: actualiza la tabla para valores enteros y tambien el label de total
void ActualizarListViewPrincipalInt(HWND hListView, const std::vector<ScanResultInt>& resultados, HWND hLabelTotal, const wchar_t* tipoDato)
{
    for (const auto& res : resultados)
    {
        LVITEMW item = { 0 };
        item.mask = LVIF_TEXT;
        item.iItem = ListView_GetItemCount(hListView);

        std::wstring direccion = PointerToHexStringNoPrefix(res.direccion);
        item.pszText = (LPWSTR)direccion.c_str();
        item.iSubItem = 0;
        ListView_InsertItem(hListView, &item);

        ListView_SetItemText(hListView, item.iItem, 1, (LPWSTR)tipoDato);

        // Convierte los enteros a string para mostrar
        std::wstring valorPrevioW = std::to_wstring(res.valorPrevio);
        ListView_SetItemText(hListView, item.iItem, 2, (LPWSTR)valorPrevioW.c_str());

        std::wstring valorActualW = std::to_wstring(res.valorActual);
        ListView_SetItemText(hListView, item.iItem, 3, (LPWSTR)valorActualW.c_str());
    }

    // --- ACTUALIZA EL LABEL ---
    if (hLabelTotal) {
        std::wstring txt = L" Total de registros: " + std::to_wstring(resultados.size());
        SetWindowTextW(hLabelTotal, txt.c_str());
    }
}

// Sobrecarga para compatibilidad (por si en algún lugar no tienes label)
void ActualizarListViewPrincipalInt(HWND hListView, const std::vector<ScanResultInt>& resultados, const wchar_t* tipoDato)
{
    ActualizarListViewPrincipalInt(hListView, resultados, NULL, tipoDato);
}

// Funcion sobrecargada: actualiza la tabla para valores enteros grandes (Int64)
void ActualizarListViewPrincipalInt64(HWND hListView, const std::vector<ScanResultInt64>& resultados, HWND hLabelTotal)
{
    for (const auto& res : resultados)
    {
        LVITEMW item = { 0 };
        item.mask = LVIF_TEXT;
        item.iItem = ListView_GetItemCount(hListView);

        // Dirección como string hexadecimal sin prefijo
        std::wstring direccion = PointerToHexStringNoPrefix(res.direccion);
        item.pszText = (LPWSTR)direccion.c_str();
        item.iSubItem = 0;
        ListView_InsertItem(hListView, &item);

        ListView_SetItemText(hListView, item.iItem, 1, (LPWSTR)L"Int64");

        std::wstring valorPrevioW = std::to_wstring(res.valorPrevio);
        ListView_SetItemText(hListView, item.iItem, 2, (LPWSTR)valorPrevioW.c_str());

        std::wstring valorActualW = std::to_wstring(res.valorActual);
        ListView_SetItemText(hListView, item.iItem, 3, (LPWSTR)valorActualW.c_str());
    }

    // --- ACTUALIZA EL LABEL ---
    if (hLabelTotal) {
        std::wstring txt = L" Total de registros: " + std::to_wstring(resultados.size());
        SetWindowTextW(hLabelTotal, txt.c_str());
    }
}

// Sobrecarga para compatibilidad (por si en algún lugar no tienes label)
void ActualizarListViewPrincipalInt64(HWND hListView, const std::vector<ScanResultInt64>& resultados)
{
    ActualizarListViewPrincipalInt64(hListView, resultados, NULL);
}

// Funcion sobrecargada: actualiza la tabla para valores decimales float y tambien el label de total
void ActualizarListViewPrincipalFloat(HWND hListView, const std::vector<ScanResultFloat>& resultados, HWND hLabelTotal)
{
    for (const auto& res : resultados)
    {
        LVITEMW item = { 0 };
        item.mask = LVIF_TEXT;
        item.iItem = ListView_GetItemCount(hListView);

        std::wstring direccion = PointerToHexStringNoPrefix(res.direccion);
        item.pszText = (LPWSTR)direccion.c_str();
        item.iSubItem = 0;
        ListView_InsertItem(hListView, &item);

        ListView_SetItemText(hListView, item.iItem, 1, (LPWSTR)L"Float");

        // Valor previo (con máxima precisión y notación científica si aplica)
        std::wostringstream ossPrev;
        ossPrev << std::setprecision(std::numeric_limits<float>::max_digits10) << res.valorPrevio;
        std::wstring valorPrevioW = ossPrev.str();
        ListView_SetItemText(hListView, item.iItem, 2, (LPWSTR)valorPrevioW.c_str());

        // Valor Actual
        std::wostringstream ossAct;
        ossAct << std::setprecision(std::numeric_limits<float>::max_digits10) << res.valorActual;
        std::wstring valorActualW = ossAct.str();
        ListView_SetItemText(hListView, item.iItem, 3, (LPWSTR)valorActualW.c_str());
    }

    // --- ACTUALIZA EL LABEL ---
    if (hLabelTotal) {
        std::wstring txt = L" Total de registros: " + std::to_wstring(resultados.size());
        SetWindowTextW(hLabelTotal, txt.c_str());
    }
}

// Sobrecarga para compatibilidad (por si en algún lugar no tienes label)
void ActualizarListViewPrincipalFloat(HWND hListView, const std::vector<ScanResultFloat>& resultados)
{
    ActualizarListViewPrincipalFloat(hListView, resultados, NULL);
}

// Funcion sobrecargada: actualiza la tabla para valores decimales double y tambien el label de total
void ActualizarListViewPrincipalDouble(HWND hListView, const std::vector<ScanResultDouble>& resultados, HWND hLabelTotal)
{
    for (const auto& res : resultados)
    {
        LVITEMW item = { 0 };
        item.mask = LVIF_TEXT;
        item.iItem = ListView_GetItemCount(hListView);

        std::wstring direccion = PointerToHexStringNoPrefix(res.direccion);
        item.pszText = (LPWSTR)direccion.c_str();
        item.iSubItem = 0;
        ListView_InsertItem(hListView, &item);

        ListView_SetItemText(hListView, item.iItem, 1, (LPWSTR)L"Double");

        // Valor previo (con máxima precisión y notación científica si aplica)
        std::wostringstream ossPrev;
        ossPrev << std::setprecision(std::numeric_limits<double>::max_digits10) << res.valorPrevio;
        std::wstring valorPrevioW = ossPrev.str();
        ListView_SetItemText(hListView, item.iItem, 2, (LPWSTR)valorPrevioW.c_str());

        // Valor Actual
        std::wostringstream ossAct;
        ossAct << std::setprecision(std::numeric_limits<double>::max_digits10) << res.valorActual;
        std::wstring valorActualW = ossAct.str();
        ListView_SetItemText(hListView, item.iItem, 3, (LPWSTR)valorActualW.c_str());
    }

    // --- ACTUALIZA EL LABEL ---
    if (hLabelTotal) {
        std::wstring txt = L" Total de registros: " + std::to_wstring(resultados.size());
        SetWindowTextW(hLabelTotal, txt.c_str());
    }
}

// Sobrecarga para compatibilidad (por si en algún lugar no tienes label)
void ActualizarListViewPrincipalDouble(HWND hListView, const std::vector<ScanResultDouble>& resultados)
{
    ActualizarListViewPrincipalDouble(hListView, resultados, NULL);
}