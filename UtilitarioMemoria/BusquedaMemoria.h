#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <functional>

extern int numEscaneo;

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
	std::function<bool(int, int)> comparador,
	const std::vector<ScanResultInt>& resultadosPrevios = {},
	size_t step = 4
);

std::vector<ScanResultInt> BuscarIntsEnMemoriaEntreValores(
	HANDLE hProcess,
	int valorMin,
	int valorMax,
	const std::vector<ScanResultInt>& resultadosPrevios = {},
	size_t step = 4
);

std::vector<ScanResultInt> BuscarTodosIntsEnMemoria(HANDLE hProcess, size_t step = 4);

std::vector<ScanResultInt> FiltrarIntsPorComparador(
	HANDLE hProcess,
	std::function<bool(int, int)> comparador,
	const std::vector<ScanResultInt>& resultadosPrevios = {},
	size_t step = 4
);

std::vector<ScanResultInt64> BuscarInt64sEnMemoria(
	HANDLE hProcess,
	long long valor,
	std::function<bool(long long, long long)> comparador,
	const std::vector<ScanResultInt64>& resultadosPrevios = {},
	size_t step = 8
);

std::vector<ScanResultInt64> BuscarInt64sEnMemoriaEntreValores(
	HANDLE hProcess,
	long long valorMin,
	long long valorMax,
	const std::vector<ScanResultInt64>& resultadosPrevios = {},
	size_t step = 8
);

std::vector<ScanResultInt64> BuscarTodosInt64sEnMemoria(HANDLE hProcess, size_t step = 8);

std::vector<ScanResultInt64> FiltrarInt64sPorComparador(
	HANDLE hProcess,
	std::function<bool(long long, long long)> comparador,
	const std::vector<ScanResultInt64>& resultadosPrevios = {},
	size_t step = 8
);

std::vector<ScanResultFloat> BuscarFloatEnMemoria(
	HANDLE hProcess,
	float valor,
	std::function<bool(float, float)> comparador,
	const std::vector<ScanResultFloat>& resultadosPrevios = {},
	size_t step = 4
);

std::vector<ScanResultFloat> BuscarFloatEnMemoriaEntreValores(
	HANDLE hProcess,
	float valorMin,
	float valorMax,
	const std::vector<ScanResultFloat>& resultadosPrevios = {},
	size_t step = 4
);

std::vector<ScanResultFloat> BuscarTodosFloatEnMemoria(HANDLE hProcess, size_t step = 4);

std::vector<ScanResultFloat> FiltrarFloatPorComparador(
	HANDLE hProcess,
	std::function<bool(float, float)> comparador,
	const std::vector<ScanResultFloat>& resultadosPrevios = {},
	size_t step = 4
);

std::vector<ScanResultDouble> BuscarDoubleEnMemoria(
	HANDLE hProcess,
	double valor,
	std::function<bool(double, double)> comparador,
	const std::vector<ScanResultDouble>& resultadosPrevios = {},
	size_t step = 8
);

std::vector<ScanResultDouble> BuscarDoubleEnMemoriaEntreValores(
	HANDLE hProcess,
	double valorMin,
	double valorMax,
	const std::vector<ScanResultDouble>& resultadosPrevios = {},
	size_t step = 8
);

std::vector<ScanResultDouble> BuscarTodosDoubleEnMemoria(HANDLE hProcess, size_t step = 8);

std::vector<ScanResultDouble> FiltrarDoubleEnMemoria(
	HANDLE hProcess,
	std::function<bool(double, double)> comparador,
	const std::vector<ScanResultDouble>& resultadosPrevios = {},
	size_t step = 8
);

void BuscarEnMemoria(HANDLE hProc, const wchar_t* valor, const wchar_t* valorEnd, int tipoValor, int tipoBusqueda);
void ResetearBusquedaMemoria();
void IncrementarContadorEscaneos(HWND hLabelNroEscaneos);
