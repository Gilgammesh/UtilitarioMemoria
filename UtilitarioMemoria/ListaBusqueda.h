#pragma once
#include <windows.h>
#include <vector>
#include "BusquedaMemoria.h"

HWND CrearLabelTotalRegistros(HWND hWndParent, HINSTANCE hInst, int x, int y, int width, int height, int id);
HWND CrearLabelNroEscaneos(HWND hWndParent, HINSTANCE hInst, int x, int y, int width, int height, int id);
HWND CrearListViewPrincipal(HWND hWndParent, HINSTANCE hInst, int x, int y, int width, int height, int id);

void InicializarColumnasListView(HWND hListView);
void ActualizarListViewPrincipalString(HWND hListView, const std::vector<ScanResultString>& resultados);
void ActualizarListViewPrincipalString(HWND hListView, const std::vector<ScanResultString>& resultados, HWND hLabelTotal);
void ActualizarListViewPrincipalInt(HWND hListView, const std::vector<ScanResultInt>& resultados, const wchar_t* tipoDato);
void ActualizarListViewPrincipalInt(HWND hListView, const std::vector<ScanResultInt>& resultados, HWND hLabelTotal, const wchar_t* tipoDato);
void ActualizarListViewPrincipalInt64(HWND hListView, const std::vector<ScanResultInt64>& resultados);
void ActualizarListViewPrincipalInt64(HWND hListView, const std::vector<ScanResultInt64>& resultados, HWND hLabelTotal);
void ActualizarListViewPrincipalFloat(HWND hListView, const std::vector<ScanResultFloat>& resultados);
void ActualizarListViewPrincipalFloat(HWND hListView, const std::vector<ScanResultFloat>& resultados, HWND hLabelTotal);
void ActualizarListViewPrincipalDouble(HWND hListView, const std::vector<ScanResultDouble>& resultados);
void ActualizarListViewPrincipalDouble(HWND hListView, const std::vector<ScanResultDouble>& resultados, HWND hLabelTotal);
