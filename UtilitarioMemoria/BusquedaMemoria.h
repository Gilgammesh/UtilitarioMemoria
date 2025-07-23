#pragma once
#include <windows.h>
#include <vector>
#include <string>

struct ScanResultString {
	LPCVOID direccion;
	std::string valorPrevio;
	std::string valorActual;
};

struct ScanResultInt {
	LPCVOID direccion;
	int valorPrevio;
	int valorActual;
};

struct ScanResultInt64 {
	LPCVOID direccion;
	long long valorPrevio;
	long long valorActual;
};

struct ScanResultFloat {
	LPCVOID direccion;
	float valorPrevio;
	float valorActual;
};

struct ScanResultDouble {
	LPCVOID direccion;
	double valorPrevio;
	double valorActual;
};

std::vector<ScanResultString> BuscarStringsEnMemoria(
	HANDLE hProcess,
	const std::string& valor,
	const std::vector<ScanResultString>& resultadosPrevios = {},
	size_t step = 1
);

std::vector<ScanResultInt> BuscarIntsEnMemoria(
	HANDLE hProcess,
	int valor,
	const std::vector<ScanResultInt>& resultadosPrevios = {},
	size_t step = 4
);

std::vector<ScanResultFloat> BuscarFloatEnMemoria(
	HANDLE hProcess,
	float valor,
	const std::vector<ScanResultFloat>& resultadosPrevios = {},
	size_t step = 4
);

std::vector<ScanResultDouble> BuscarDoubleEnMemoria(
	HANDLE hProcess,
	double valor,
	const std::vector<ScanResultDouble>& resultadosPrevios = {},
	size_t step = 4
);

void BuscarEnMemoria(HANDLE hProc, const wchar_t* valor, int tipoValor, int tipoBusqueda);
void ResetearBusquedaMemoria();
