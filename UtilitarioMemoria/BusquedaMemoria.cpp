#include <stdio.h>
#include <string>
#include <vector>
#include <windows.h>
#include <cmath>
#include <commctrl.h>
#include <functional>
#include "BusquedaMemoria.h"
#include "ListaBusqueda.h"

extern HWND hListMain;
extern HWND hLblTotalRegistros;
extern HWND hLblNroEscaneos;

// Contador de escaneos
int numEscaneo = 0;

// Define resultados previos
static std::vector<ScanResultString> resultadosPreviosString;
static std::vector<ScanResultInt> resultadosPreviosInt;
static std::vector<ScanResultInt64> resultadosPreviosInt64;
static std::vector<ScanResultFloat> resultadosPreviosFloat;
static std::vector<ScanResultDouble> resultadosPreviosDouble;

// FUNCION UTILITARIA PARA COMPARAR FLOATS CON PRECISION
bool IsFloatRoundedMatch(float a, float b, int precision) {
	float factor = static_cast<float>(std::pow(10, precision));
	return std::round(a * factor) == std::round(b * factor);
}
// FUNCION UTILITARIA PARA COMPARAR FLOATS SIN PRECISION
bool IsFloatDefaultMatch(float a, float b) {
	return a == b;
}

// FUNCION UTILITARIA PARA COMPARAR DOUBLES CON PRECISION
bool IsDoubleRoundedMatch(double a, double b, int precision) {
	double factor = static_cast<double>(std::pow(10, precision));
	return std::round(a * factor) == std::round(b * factor);
}
// FUNCION UTILITARIA PARA COMPARAR DOUBLES SIN PRECISION
bool IsDoubleDefaultMatch(double a, double b) {
	return a == b;
}

// FUNCION UTILITARIA PARA CONVERTIR std::wstring A std::string (UTF-8)
static std::string WideToUtf8(const std::wstring& wstr) {
	if (wstr.empty()) return {};
	int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (len <= 0) return {};
	std::string result(len - 1, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], len, nullptr, nullptr);
	return result;
}

void BuscarEnMemoria(HANDLE hProc, const wchar_t* valor, const wchar_t* valorEnd, int tipoDato, int tipoBusqueda)
{
	switch (tipoDato) {
	case 0: // Byte
	{
		std::wstring ws(valor);
		try {
			std::vector<ScanResultInt> resultados;
			if (tipoBusqueda == 0) { // Valor exacto
				int valorInt = std::stoi(ws);
				resultados = BuscarIntsEnMemoria(hProc, valorInt, [](int a, int b) { return a == b; }, resultadosPreviosInt, 1);
			}
			if (tipoBusqueda == 1) { // Mayor que
				int valorInt = std::stoi(ws);
				resultados = BuscarIntsEnMemoria(hProc, valorInt, [](int a, int b) { return a > b; }, resultadosPreviosInt, 1);
			}
			if (tipoBusqueda == 2) { // Menor que
				int valorInt = std::stoi(ws);
				resultados = BuscarIntsEnMemoria(hProc, valorInt, [](int a, int b) { return a < b; }, resultadosPreviosInt, 1);
			}
			if (tipoBusqueda == 3) { // Entre dos valores
				int valorInt = std::stoi(ws);
				std::wstring wsEnd(valorEnd);
				int valorIntEnd = std::stoi(wsEnd);
				resultados = BuscarIntsEnMemoriaEntreValores(hProc, valorInt, valorIntEnd, resultadosPreviosInt, 1);
			}
			if (tipoBusqueda == 4) { // Valor inicial desconocido
				resultados = BuscarTodosIntsEnMemoria(hProc, 1);
			}
			if (tipoBusqueda == 5) { // Valor incrementado
				resultados = FiltrarIntsPorComparador(hProc, [](int a, int b) { return a > b; }, resultadosPreviosInt, 1);
			}
			if (tipoBusqueda == 6) { // Valor decrementado
				resultados = FiltrarIntsPorComparador(hProc, [](int a, int b) { return a < b; }, resultadosPreviosInt, 1);
			}
			if (tipoBusqueda == 7) { // Valor cambiado
				resultados = FiltrarIntsPorComparador(hProc, [](int a, int b) { return a != b; }, resultadosPreviosInt, 1);
			}
			if (tipoBusqueda == 8) { // Valor sin cambios
				resultados = FiltrarIntsPorComparador(hProc, [](int a, int b) { return a == b; }, resultadosPreviosInt, 1);
			}
			resultadosPreviosInt = resultados;
			ListView_DeleteAllItems(hListMain);
			if (tipoBusqueda == 4) {
				std::wstring txt = L" Total de registros: " + std::to_wstring(resultados.size());
				SetWindowTextW(hLblTotalRegistros, txt.c_str());
			}
			else {
				ActualizarListViewPrincipalInt(hListMain, resultados, hLblTotalRegistros, L"Int8");
			}
			IncrementarContadorEscaneos(hLblNroEscaneos);
		}
		catch (const std::exception&) {
			MessageBoxW(NULL, L"El valor ingresado no es un numero entero valido.", L"Error", MB_OK | MB_ICONERROR);
		}
		break;
	}
	case 1: // 2 Bytes
	{
		std::wstring ws(valor);
		try {
			std::vector<ScanResultInt> resultados;
			if (tipoBusqueda == 0) { // Valor exacto
				int valorInt = std::stoi(ws);
				resultados = BuscarIntsEnMemoria(hProc, valorInt, [](int a, int b) { return a == b; }, resultadosPreviosInt, 2);
			}
			if (tipoBusqueda == 1) { // Mayor que
				int valorInt = std::stoi(ws);
				resultados = BuscarIntsEnMemoria(hProc, valorInt, [](int a, int b) { return a > b; }, resultadosPreviosInt, 2);
			}
			if (tipoBusqueda == 2) { // Menor que
				int valorInt = std::stoi(ws);
				resultados = BuscarIntsEnMemoria(hProc, valorInt, [](int a, int b) { return a < b; }, resultadosPreviosInt, 2);
			}
			if (tipoBusqueda == 3) { // Entre dos valores
				int valorInt = std::stoi(ws);
				std::wstring wsEnd(valorEnd);
				int valorIntEnd = std::stoi(wsEnd);
				resultados = BuscarIntsEnMemoriaEntreValores(hProc, valorInt, valorIntEnd, resultadosPreviosInt, 2);
			}
			if (tipoBusqueda == 4) { // Valor inicial desconocido
				resultados = BuscarTodosIntsEnMemoria(hProc, 2);
			}
			if (tipoBusqueda == 5) { // Valor incrementado
				resultados = FiltrarIntsPorComparador(hProc, [](int a, int b) { return a > b; }, resultadosPreviosInt, 2);
			}
			if (tipoBusqueda == 6) { // Valor decrementado
				resultados = FiltrarIntsPorComparador(hProc, [](int a, int b) { return a < b; }, resultadosPreviosInt, 2);
			}
			if (tipoBusqueda == 7) { // Valor cambiado
				resultados = FiltrarIntsPorComparador(hProc, [](int a, int b) { return a != b; }, resultadosPreviosInt, 2);
			}
			if (tipoBusqueda == 8) { // Valor sin cambios
				resultados = FiltrarIntsPorComparador(hProc, [](int a, int b) { return a == b; }, resultadosPreviosInt, 2);
			}
			resultadosPreviosInt = resultados;
			ListView_DeleteAllItems(hListMain);
			if (tipoBusqueda == 4) {
				std::wstring txt = L" Total de registros: " + std::to_wstring(resultados.size());
				SetWindowTextW(hLblTotalRegistros, txt.c_str());
			}
			else {
				ActualizarListViewPrincipalInt(hListMain, resultados, hLblTotalRegistros, L"Int16");
			}
			IncrementarContadorEscaneos(hLblNroEscaneos);
		}
		catch (const std::exception&) {
			MessageBoxW(NULL, L"El valor ingresado no es un numero entero valido.", L"Error", MB_OK | MB_ICONERROR);
		}
		break;
	}
	case 2: // 4 Bytes
	{
		std::wstring ws(valor);
		try {
			std::vector<ScanResultInt> resultados;
			if (tipoBusqueda == 0) { // Valor exacto
				int valorInt = std::stoi(ws);
				resultados = BuscarIntsEnMemoria(hProc, valorInt, [](int a, int b) { return a == b; }, resultadosPreviosInt, 4);
			}
			if (tipoBusqueda == 1) { // Mayor que
				int valorInt = std::stoi(ws);
				resultados = BuscarIntsEnMemoria(hProc, valorInt, [](int a, int b) { return a > b; }, resultadosPreviosInt, 4);
			}
			if (tipoBusqueda == 2) { // Menor que
				int valorInt = std::stoi(ws);
				resultados = BuscarIntsEnMemoria(hProc, valorInt, [](int a, int b) { return a < b; }, resultadosPreviosInt, 4);
			}
			if (tipoBusqueda == 3) { // Entre dos valores
				int valorInt = std::stoi(ws);
				std::wstring wsEnd(valorEnd);
				int valorIntEnd = std::stoi(wsEnd);
				resultados = BuscarIntsEnMemoriaEntreValores(hProc, valorInt, valorIntEnd, resultadosPreviosInt, 4);
			}
			if (tipoBusqueda == 4) { // Valor inicial desconocido
				resultados = BuscarTodosIntsEnMemoria(hProc, 4);
			}
			if (tipoBusqueda == 5) { // Valor incrementado
				resultados = FiltrarIntsPorComparador(hProc, [](int a, int b) { return a > b; }, resultadosPreviosInt, 4);
			}
			if (tipoBusqueda == 6) { // Valor decrementado
				resultados = FiltrarIntsPorComparador(hProc, [](int a, int b) { return a < b; }, resultadosPreviosInt, 4);
			}
			if (tipoBusqueda == 7) { // Valor cambiado
				resultados = FiltrarIntsPorComparador(hProc, [](int a, int b) { return a != b; }, resultadosPreviosInt, 4);
			}
			if (tipoBusqueda == 8) { // Valor sin cambios
				resultados = FiltrarIntsPorComparador(hProc, [](int a, int b) { return a == b; }, resultadosPreviosInt, 4);
			}
			resultadosPreviosInt = resultados;
			ListView_DeleteAllItems(hListMain);
			if (tipoBusqueda == 4) {
				std::wstring txt = L" Total de registros: " + std::to_wstring(resultados.size());
				SetWindowTextW(hLblTotalRegistros, txt.c_str());
			}
			else {
				ActualizarListViewPrincipalInt(hListMain, resultados, hLblTotalRegistros, L"Int32");
			}
			IncrementarContadorEscaneos(hLblNroEscaneos);
		}
		catch (const std::exception&) {
			MessageBoxW(NULL, L"El valor ingresado no es un numero entero valido.", L"Error", MB_OK | MB_ICONERROR);
		}
		break;
	}
	case 3: // 8 Bytes
	{
		std::wstring ws(valor);
		try {
			std::vector<ScanResultInt64> resultados;
			if (tipoBusqueda == 0) { // Valor exacto
				long long valorInt64 = std::stoll(ws); // Conversión a 64 bits
				resultados = BuscarInt64sEnMemoria(hProc, valorInt64, [](long long a, long long b) { return a == b; }, resultadosPreviosInt64, 8);
			}
			if (tipoBusqueda == 1) { // Mayor que
				long long valorInt64 = std::stoll(ws); // Conversión a 64 bits
				resultados = BuscarInt64sEnMemoria(hProc, valorInt64, [](long long a, long long b) { return a > b; }, resultadosPreviosInt64, 8);
			}
			if (tipoBusqueda == 2) { // Menor que
				long long valorInt64 = std::stoll(ws); // Conversión a 64 bits
				resultados = BuscarInt64sEnMemoria(hProc, valorInt64, [](long long a, long long b) { return a < b; }, resultadosPreviosInt64, 8);
			}
			if (tipoBusqueda == 3) { // Entre dos valores
				long long valorInt64 = std::stoll(ws); // Conversión a 64 bits
				std::wstring wsEnd(valorEnd);
				long long valorInt64End = std::stoll(wsEnd);
				resultados = BuscarInt64sEnMemoriaEntreValores(hProc, valorInt64, valorInt64End, resultadosPreviosInt64, 8);
			}
			if (tipoBusqueda == 4) { // Valor inicial desconocido
				resultados = BuscarTodosInt64sEnMemoria(hProc, 8);
			}
			if (tipoBusqueda == 5) { // Valor incrementado
				resultados = FiltrarInt64sPorComparador(hProc, [](long long a, long long b) { return a > b; }, resultadosPreviosInt64, 8);
			}
			if (tipoBusqueda == 6) { // Valor decrementado
				resultados = FiltrarInt64sPorComparador(hProc, [](long long a, long long b) { return a < b; }, resultadosPreviosInt64, 8);
			}
			if (tipoBusqueda == 7) { // Valor cambiado
				resultados = FiltrarInt64sPorComparador(hProc, [](long long a, long long b) { return a != b; }, resultadosPreviosInt64, 8);
			}
			if (tipoBusqueda == 8) { // Valor sin cambios
				resultados = FiltrarInt64sPorComparador(hProc, [](long long a, long long b) { return a == b; }, resultadosPreviosInt64, 8);
			}
			resultadosPreviosInt64 = resultados;
			ListView_DeleteAllItems(hListMain);
			if (tipoBusqueda == 4) {
				std::wstring txt = L" Total de registros: " + std::to_wstring(resultados.size());
				SetWindowTextW(hLblTotalRegistros, txt.c_str());
			}
			else {
				ActualizarListViewPrincipalInt64(hListMain, resultados, hLblTotalRegistros);
			}
			IncrementarContadorEscaneos(hLblNroEscaneos);
		}
		catch (const std::exception&) {
			MessageBoxW(NULL, L"El valor ingresado no es un numero entero valido (Int64).", L"Error", MB_OK | MB_ICONERROR);
		}
		break;
	}
	case 4: // Float
	{
		std::wstring ws(valor);
		std::string sValor = WideToUtf8(ws);
		try {
			std::vector<ScanResultFloat> resultados;
			if (tipoBusqueda == 0) { // Valor exacto
				float valorFloat = std::stof(sValor);
				resultados = BuscarFloatEnMemoria(hProc, valorFloat, [](float a, float b) { return a == b; }, resultadosPreviosFloat, 4);
			}
			if (tipoBusqueda == 1) { // Mayor que
				float valorFloat = std::stof(sValor);
				resultados = BuscarFloatEnMemoria(hProc, valorFloat, [](float a, float b) { return a > b; }, resultadosPreviosFloat, 4);
			}
			if (tipoBusqueda == 2) { // Menor que
				float valorFloat = std::stof(sValor);
				resultados = BuscarFloatEnMemoria(hProc, valorFloat, [](float a, float b) { return a < b; }, resultadosPreviosFloat, 4);
			}
			if (tipoBusqueda == 3) { // Entre dos valores
				float valorFloat = std::stof(sValor);
				std::wstring wsEnd(valorEnd);
				std::string sValorEnd = WideToUtf8(wsEnd);
				float valorFloatEnd = std::stof(sValorEnd);
				resultados = BuscarFloatEnMemoriaEntreValores(hProc, valorFloat, valorFloatEnd, resultadosPreviosFloat, 4);
			}
			if (tipoBusqueda == 4) { // Valor inicial desconocido
				resultados = BuscarTodosFloatEnMemoria(hProc, 4);
			}
			if (tipoBusqueda == 5) { // Valor incrementado
				resultados = FiltrarFloatPorComparador(hProc, [](float a, float b) { return a > b; }, resultadosPreviosFloat, 4);
			}
			if (tipoBusqueda == 6) { // Valor decrementado
				resultados = FiltrarFloatPorComparador(hProc, [](float a, float b) { return a < b; }, resultadosPreviosFloat, 4);
			}
			if (tipoBusqueda == 7) { // Valor cambiado
				resultados = FiltrarFloatPorComparador(hProc, [](float a, float b) { return a != b; }, resultadosPreviosFloat, 4);
			}
			if (tipoBusqueda == 8) { // Valor sin cambios
				resultados = FiltrarFloatPorComparador(hProc, [](float a, float b) { return a == b; }, resultadosPreviosFloat, 4);
			}
			resultadosPreviosFloat = resultados;
			ListView_DeleteAllItems(hListMain);
			if (tipoBusqueda == 4) {
				std::wstring txt = L" Total de registros: " + std::to_wstring(resultados.size());
				SetWindowTextW(hLblTotalRegistros, txt.c_str());
			}
			else {
				ActualizarListViewPrincipalFloat(hListMain, resultados, hLblTotalRegistros);
			}
			IncrementarContadorEscaneos(hLblNroEscaneos);
		}
		catch (const std::exception&) {
			MessageBoxW(NULL, L"El valor ingresado no es un numero decimal valido.", L"Error", MB_OK | MB_ICONERROR);
		}
		break;
	}
	case 5: // Double
	{
		std::wstring ws(valor);
		std::string sValor = WideToUtf8(ws);
		try {
			std::vector<ScanResultDouble> resultados;
			if (tipoBusqueda == 0) { // Valor exacto
				double valorDouble = std::stod(sValor);
				resultados = BuscarDoubleEnMemoria(hProc, valorDouble, [](double a, double b) { return a == b; }, resultadosPreviosDouble, 8);
			}
			if (tipoBusqueda == 1) { // Mayor que
				double valorDouble = std::stod(sValor);
				resultados = BuscarDoubleEnMemoria(hProc, valorDouble, [](double a, double b) { return a > b; }, resultadosPreviosDouble, 8);
			}
			if (tipoBusqueda == 2) { // Menor que
				double valorDouble = std::stod(sValor);
				resultados = BuscarDoubleEnMemoria(hProc, valorDouble, [](double a, double b) { return a < b; }, resultadosPreviosDouble, 8);
			}
			if (tipoBusqueda == 3) { // Entre dos valores
				double valorDouble = std::stod(sValor);
				std::wstring wsEnd(valorEnd);
				std::string sValorEnd = WideToUtf8(wsEnd);
				double valorDoubleEnd = std::stod(sValorEnd);
				resultados = BuscarDoubleEnMemoriaEntreValores(hProc, valorDouble, valorDoubleEnd, resultadosPreviosDouble, 8);
			}
			if (tipoBusqueda == 4) { // Valor inicial desconocido
				resultados = BuscarTodosDoubleEnMemoria(hProc, 8);
			}
			if (tipoBusqueda == 5) { // Valor incrementado
				resultados = FiltrarDoubleEnMemoria(hProc, [](double a, double b) { return a > b; }, resultadosPreviosDouble, 4);
			}
			if (tipoBusqueda == 6) { // Valor decrementado
				resultados = FiltrarDoubleEnMemoria(hProc, [](double a, double b) { return a < b; }, resultadosPreviosDouble, 4);
			}
			if (tipoBusqueda == 7) { // Valor cambiado
				resultados = FiltrarDoubleEnMemoria(hProc, [](double a, double b) { return a != b; }, resultadosPreviosDouble, 4);
			}
			if (tipoBusqueda == 8) { // Valor sin cambios
				resultados = FiltrarDoubleEnMemoria(hProc, [](double a, double b) { return a == b; }, resultadosPreviosDouble, 4);
			}
			resultadosPreviosDouble = resultados;
			ListView_DeleteAllItems(hListMain);
			if (tipoBusqueda == 4) {
				std::wstring txt = L" Total de registros: " + std::to_wstring(resultados.size());
				SetWindowTextW(hLblTotalRegistros, txt.c_str());
			}
			else {
				ActualizarListViewPrincipalDouble(hListMain, resultados, hLblTotalRegistros);
			}
			IncrementarContadorEscaneos(hLblNroEscaneos);
		}
		catch (const std::exception&) {
			MessageBoxW(NULL, L"El valor ingresado no es un numero decimal valido.", L"Error", MB_OK | MB_ICONERROR);
		}
		break;
	}
	case 6: // String
	{
		std::wstring ws(valor);
		try {
			std::string sValor = WideToUtf8(ws);
			auto resultados = BuscarStringsEnMemoria(hProc, sValor, resultadosPreviosString, 1);
			resultadosPreviosString = resultados;
			ListView_DeleteAllItems(hListMain);
			ActualizarListViewPrincipalString(hListMain, resultados, hLblTotalRegistros);
			IncrementarContadorEscaneos(hLblNroEscaneos);
		}
		catch (const std::exception&) {
			MessageBoxW(NULL, L"El valor ingresado no es una cadena valid.", L"Error", MB_OK | MB_ICONERROR);
		}
		break;
	}
	case 7: // Hexadecimal
	{

	}
	case 8: // Direccion Memoria
	{
		std::wstring ws(valor);
		try {
			// Convierte la direccion de string hexadecimal a número
			size_t direccion = std::stoull(ws, nullptr, 16);

			HANDLE hProcess = hProc;
			unsigned char buffer[16] = { 0 };
			SIZE_T bytesRead = 0;
			BOOL ok = ReadProcessMemory(hProcess, (LPCVOID)direccion, buffer, sizeof(buffer), &bytesRead);

			if (!ok || bytesRead == 0) {
				MessageBoxW(NULL, L"No se pudo leer memoria en esa direccion.", L"Error", MB_OK | MB_ICONERROR);
				break;
			}

			// Llama a las funciones para mostrar cada tipo
			std::vector<ScanResultString> resultStrings;
			std::vector<ScanResultInt> resultInt8;
			std::vector<ScanResultInt> resultInt16;
			std::vector<ScanResultInt> resultInt32;
			std::vector<ScanResultInt64> resultInt64;
			std::vector<ScanResultFloat> resultFloat;
			std::vector<ScanResultDouble> resultDouble;

			// String ASCII
			std::string asciiString;
			for (SIZE_T i = 0; i < bytesRead && i < 15; ++i) {
				char c = (char)buffer[i];
				if (c == 0) break;
				asciiString += c;
			}
			resultStrings.push_back({ (LPCVOID)direccion, asciiString, asciiString });

			// Int8
			char valInt8 = *reinterpret_cast<char*>(buffer);
			resultInt8.push_back({ (LPCVOID)direccion, valInt8, valInt8 });

			// Int16
			short valInt16 = 0;
			if (bytesRead >= 2) memcpy(&valInt16, buffer, 2);
			resultInt16.push_back({ (LPCVOID)direccion, valInt16, valInt16 });

			// Int32
			int valInt32 = 0;
			if (bytesRead >= 4) memcpy(&valInt32, buffer, 4);
			resultInt32.push_back({ (LPCVOID)direccion, valInt32, valInt32 });

			// Int64
			long long valInt64 = 0;
			if (bytesRead >= 8) memcpy(&valInt64, buffer, 8);
			resultInt64.push_back({ (LPCVOID)direccion, valInt64, valInt64 });

			// Float
			float valFloat = 0;
			if (bytesRead >= 4) memcpy(&valFloat, buffer, 4);
			resultFloat.push_back({ (LPCVOID)direccion, valFloat, valFloat });

			// Double
			double valDouble = 0;
			if (bytesRead >= 8) memcpy(&valDouble, buffer, 8);
			resultDouble.push_back({ (LPCVOID)direccion, valDouble, valDouble });

			// Limpia el ListView antes de mostrar
			ListView_DeleteAllItems(hListMain);

			// Actualiza el ListView con los resultados
			ActualizarListViewPrincipalString(hListMain, resultStrings, hLblTotalRegistros);
			ActualizarListViewPrincipalInt(hListMain, resultInt8, hLblTotalRegistros, L"Int8");
			ActualizarListViewPrincipalInt(hListMain, resultInt16, hLblTotalRegistros, L"Int16");
			ActualizarListViewPrincipalInt(hListMain, resultInt32, hLblTotalRegistros, L"Int32");
			ActualizarListViewPrincipalInt64(hListMain, resultInt64, hLblTotalRegistros);
			ActualizarListViewPrincipalFloat(hListMain, resultFloat, hLblTotalRegistros);
			ActualizarListViewPrincipalDouble(hListMain, resultDouble, hLblTotalRegistros);
		}
		catch (const std::exception&) {
			MessageBoxW(NULL, L"Direccion hexadecimal invalida.", L"Error", MB_OK | MB_ICONERROR);
		}
		break;
	}
	default:
		MessageBoxW(NULL, L"Tipo de dato no soportado.", L"Error", MB_OK | MB_ICONERROR);
		break;
	}
}

void ResetearBusquedaMemoria() {
	numEscaneo = 0;
	resultadosPreviosString.clear();
	resultadosPreviosInt.clear();
	resultadosPreviosFloat.clear();
	resultadosPreviosDouble.clear();
}

std::vector<ScanResultString> BuscarStringsEnMemoria(
	HANDLE hProcess,
	const std::string& valor,
	const std::vector<ScanResultString>& resultadosPrevios,
	size_t step
) {
	std::vector<ScanResultString> resultado;
	const size_t strLen = valor.length();
	if (strLen == 0 || !hProcess) return resultado;

	if (resultadosPrevios.empty()) {
		// Escaneo completo de memoria
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		LPCVOID addr = si.lpMinimumApplicationAddress;
		LPCVOID maxAddr = si.lpMaximumApplicationAddress;
		MEMORY_BASIC_INFORMATION mbi;

		while (addr < maxAddr) {
			if (VirtualQueryEx(hProcess, addr, &mbi, sizeof(mbi)) == sizeof(mbi)) {
				if ((mbi.State == MEM_COMMIT) &&
					(
						(mbi.Protect & PAGE_READWRITE) ||
						(mbi.Protect & PAGE_WRITECOPY) ||
						(mbi.Protect & PAGE_EXECUTE_READWRITE) ||
						(mbi.Protect & PAGE_EXECUTE_WRITECOPY)
						) &&
					!(mbi.Protect & PAGE_GUARD) &&
					!(mbi.Protect & PAGE_NOACCESS)) {

					std::vector<char> buffer(mbi.RegionSize);
					SIZE_T bytesRead = 0;
					if (ReadProcessMemory(hProcess, mbi.BaseAddress, buffer.data(), mbi.RegionSize, &bytesRead) && bytesRead > 0) {
						for (SIZE_T offset = 0; offset <= buffer.size() - strLen; offset += step) {
							if (memcmp(&buffer[offset], valor.c_str(), strLen) == 0) {
								LPCVOID foundAddr = (BYTE*)mbi.BaseAddress + offset;
								std::string valorActual(&buffer[offset], strLen);
								resultado.push_back({ foundAddr, valorActual, valorActual });
							}
						}
					}
				}
				addr = (BYTE*)mbi.BaseAddress + mbi.RegionSize;
			}
			else {
				break;
			}
		}
	}
	else {
		// Solo revisa direcciones previas
		for (const auto& prev : resultadosPrevios) {
			char buffer[128] = { 0 };
			SIZE_T bytesRead = 0;
			if (ReadProcessMemory(hProcess, prev.direccion, buffer, strLen, &bytesRead) && bytesRead == strLen) {
				std::string valorActual(buffer, strLen);
				if (memcmp(buffer, valor.c_str(), strLen) == 0) {
					resultado.push_back({ prev.direccion, prev.valorActual, valorActual });
				}
			}
		}
	}
	return resultado;
}

std::vector<ScanResultInt> BuscarIntsEnMemoria(
	HANDLE hProcess,
	int valor,
	std::function<bool(int, int)> comparador,
	const std::vector<ScanResultInt>& resultadosPrevios,
	size_t step
) {
	std::vector<ScanResultInt> resultado;
	if (!hProcess) return resultado;

	if (resultadosPrevios.empty()) {
		// Escaneo completo de memoria
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		LPCVOID addr = si.lpMinimumApplicationAddress;
		LPCVOID maxAddr = si.lpMaximumApplicationAddress;
		MEMORY_BASIC_INFORMATION mbi;

		while (addr < maxAddr) {
			if (VirtualQueryEx(hProcess, addr, &mbi, sizeof(mbi)) == sizeof(mbi)) {
				if ((mbi.State == MEM_COMMIT) &&
					(
						(mbi.Protect & PAGE_READWRITE) ||
						(mbi.Protect & PAGE_WRITECOPY) ||
						(mbi.Protect & PAGE_EXECUTE_READWRITE) ||
						(mbi.Protect & PAGE_EXECUTE_WRITECOPY)
						) &&
					!(mbi.Protect & PAGE_GUARD) &&
					!(mbi.Protect & PAGE_NOACCESS)) {

					std::vector<char> buffer(mbi.RegionSize);
					SIZE_T bytesRead = 0;
					if (ReadProcessMemory(hProcess, mbi.BaseAddress, buffer.data(), mbi.RegionSize, &bytesRead) && bytesRead > 0) {
						for (SIZE_T offset = 0; offset <= buffer.size() - sizeof(int); offset += step) {
							int valorLeido;
							memcpy(&valorLeido, &buffer[offset], sizeof(int));
							if (comparador(valorLeido, valor)) {  // <-- Aquí se usa el comparador flexible
								LPCVOID foundAddr = (BYTE*)mbi.BaseAddress + offset;
								resultado.push_back({ foundAddr, valorLeido, valorLeido });
							}
						}
					}
				}
				addr = (BYTE*)mbi.BaseAddress + mbi.RegionSize;
			}
			else {
				break;
			}
		}
	}
	else {
		// Solo revisa direcciones previas
		for (const auto& prev : resultadosPrevios) {
			int valorLeido = 0;
			SIZE_T bytesRead = 0;
			if (ReadProcessMemory(hProcess, prev.direccion, &valorLeido, sizeof(int), &bytesRead) && bytesRead == sizeof(int)) {
				if (comparador(valorLeido, valor)) {  // <-- Aquí se usa el comparador flexible
					resultado.push_back({ prev.direccion, prev.valorActual, valorLeido });
				}
			}
		}
	}
	return resultado;
}

std::vector<ScanResultInt> BuscarIntsEnMemoriaEntreValores(
	HANDLE hProcess,
	int valorMin,
	int valorMax,
	const std::vector<ScanResultInt>& resultadosPrevios,
	size_t step
) {
	return BuscarIntsEnMemoria(
		hProcess,
		valorMin, // Usaremos valorMin pero el comparador ahora chequea si está entre los dos
		[valorMin, valorMax](int a, int /*b*/) { return a >= valorMin && a <= valorMax; },
		resultadosPrevios,
		step
	);
}

std::vector<ScanResultInt> BuscarTodosIntsEnMemoria(HANDLE hProcess, size_t step) {
	std::vector<ScanResultInt> resultado;
	if (!hProcess) return resultado;

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	LPCVOID addr = si.lpMinimumApplicationAddress;
	LPCVOID maxAddr = si.lpMaximumApplicationAddress;
	MEMORY_BASIC_INFORMATION mbi;

	while (addr < maxAddr) {
		if (VirtualQueryEx(hProcess, addr, &mbi, sizeof(mbi)) == sizeof(mbi)) {
			if ((mbi.State == MEM_COMMIT) &&
				(
					(mbi.Protect & PAGE_READWRITE) ||
					(mbi.Protect & PAGE_WRITECOPY) ||
					(mbi.Protect & PAGE_EXECUTE_READWRITE) ||
					(mbi.Protect & PAGE_EXECUTE_WRITECOPY)
					) &&
				!(mbi.Protect & PAGE_GUARD) &&
				!(mbi.Protect & PAGE_NOACCESS)) {

				std::vector<char> buffer(mbi.RegionSize);
				SIZE_T bytesRead = 0;
				if (ReadProcessMemory(hProcess, mbi.BaseAddress, buffer.data(), mbi.RegionSize, &bytesRead) && bytesRead > 0) {
					for (SIZE_T offset = 0; offset <= buffer.size() - sizeof(int); offset += step) {
						int valorLeido;
						memcpy(&valorLeido, &buffer[offset], sizeof(int));
						LPCVOID foundAddr = (BYTE*)mbi.BaseAddress + offset;
						resultado.push_back({ foundAddr, valorLeido, valorLeido });
					}
				}
			}
			addr = (BYTE*)mbi.BaseAddress + mbi.RegionSize;
		}
		else {
			break;
		}
	}
	return resultado;
}

std::vector<ScanResultInt> FiltrarIntsPorComparador(
	HANDLE hProcess,
	std::function<bool(int actual, int previo)> comparador,
	const std::vector<ScanResultInt>& resultadosPrevios,
	size_t step
) {
	std::vector<ScanResultInt> resultado;
	if (!hProcess) return resultado;

	if (resultadosPrevios.empty()) return resultado;

	for (const auto& prev : resultadosPrevios) {
		int valorLeido = 0;
		SIZE_T bytesRead = 0;
		if (ReadProcessMemory(hProcess, prev.direccion, &valorLeido, sizeof(int), &bytesRead) && bytesRead == sizeof(int)) {
			if (comparador(valorLeido, prev.valorActual)) {
				resultado.push_back({ prev.direccion, prev.valorActual, valorLeido });
			}
		}
	}
	return resultado;
}

std::vector<ScanResultInt64> BuscarInt64sEnMemoria(
	HANDLE hProcess,
	long long valor,
	std::function<bool(long long, long long)> comparador,
	const std::vector<ScanResultInt64>& resultadosPrevios,
	size_t step
) {
	std::vector<ScanResultInt64> resultado;
	if (!hProcess) return resultado;

	if (resultadosPrevios.empty()) {
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		LPCVOID addr = si.lpMinimumApplicationAddress;
		LPCVOID maxAddr = si.lpMaximumApplicationAddress;
		MEMORY_BASIC_INFORMATION mbi;

		while (addr < maxAddr) {
			if (VirtualQueryEx(hProcess, addr, &mbi, sizeof(mbi)) == sizeof(mbi)) {
				if ((mbi.State == MEM_COMMIT) &&
					(
						(mbi.Protect & PAGE_READWRITE) ||
						(mbi.Protect & PAGE_WRITECOPY) ||
						(mbi.Protect & PAGE_EXECUTE_READWRITE) ||
						(mbi.Protect & PAGE_EXECUTE_WRITECOPY)
						) &&
					!(mbi.Protect & PAGE_GUARD) &&
					!(mbi.Protect & PAGE_NOACCESS)) {

					std::vector<char> buffer(mbi.RegionSize);
					SIZE_T bytesRead = 0;
					if (ReadProcessMemory(hProcess, mbi.BaseAddress, buffer.data(), mbi.RegionSize, &bytesRead) && bytesRead > 0) {
						for (SIZE_T offset = 0; offset <= buffer.size() - sizeof(long long); offset += step) {
							long long valorLeido = 0;
							memcpy(&valorLeido, &buffer[offset], sizeof(long long));
							if (comparador(valorLeido, valor)) {
								LPCVOID foundAddr = (BYTE*)mbi.BaseAddress + offset;
								resultado.push_back({ foundAddr, valorLeido, valorLeido });
							}
						}
					}
				}
				addr = (BYTE*)mbi.BaseAddress + mbi.RegionSize;
			}
			else {
				break;
			}
		}
	}
	else {
		for (const auto& prev : resultadosPrevios) {
			long long valorLeido = 0;
			SIZE_T bytesRead = 0;
			if (ReadProcessMemory(hProcess, prev.direccion, &valorLeido, sizeof(long long), &bytesRead) && bytesRead == sizeof(long long)) {
				if (comparador(valorLeido, valor)) {
					resultado.push_back({ prev.direccion, prev.valorActual, valorLeido });
				}
			}
		}
	}
	return resultado;
}

std::vector<ScanResultInt64> BuscarInt64sEnMemoriaEntreValores(
	HANDLE hProcess,
	long long valorMin,
	long long valorMax,
	const std::vector<ScanResultInt64>& resultadosPrevios,
	size_t step
) {
	return BuscarInt64sEnMemoria(
		hProcess,
		valorMin,
		[valorMin, valorMax](long long a, long long /*b*/) { return a >= valorMin && a <= valorMax; },
		resultadosPrevios,
		step
	);
}

std::vector<ScanResultInt64> BuscarTodosInt64sEnMemoria(HANDLE hProcess, size_t step) {
	std::vector<ScanResultInt64> resultado;
	if (!hProcess) return resultado;

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	LPCVOID addr = si.lpMinimumApplicationAddress;
	LPCVOID maxAddr = si.lpMaximumApplicationAddress;
	MEMORY_BASIC_INFORMATION mbi;

	while (addr < maxAddr) {
		if (VirtualQueryEx(hProcess, addr, &mbi, sizeof(mbi)) == sizeof(mbi)) {
			if ((mbi.State == MEM_COMMIT) &&
				(
					(mbi.Protect & PAGE_READWRITE) ||
					(mbi.Protect & PAGE_WRITECOPY) ||
					(mbi.Protect & PAGE_EXECUTE_READWRITE) ||
					(mbi.Protect & PAGE_EXECUTE_WRITECOPY)
					) &&
				!(mbi.Protect & PAGE_GUARD) &&
				!(mbi.Protect & PAGE_NOACCESS)) {

				std::vector<char> buffer(mbi.RegionSize);
				SIZE_T bytesRead = 0;
				if (ReadProcessMemory(hProcess, mbi.BaseAddress, buffer.data(), mbi.RegionSize, &bytesRead) && bytesRead > 0) {
					for (SIZE_T offset = 0; offset <= buffer.size() - sizeof(long long); offset += step) {
						long long valorLeido = 0;
						memcpy(&valorLeido, &buffer[offset], sizeof(long long));
						LPCVOID foundAddr = (BYTE*)mbi.BaseAddress + offset;
						resultado.push_back({ foundAddr, valorLeido, valorLeido });
					}
				}
			}
			addr = (BYTE*)mbi.BaseAddress + mbi.RegionSize;
		}
		else {
			break;
		}
	}
	return resultado;
}

std::vector<ScanResultInt64> FiltrarInt64sPorComparador(
	HANDLE hProcess,
	std::function<bool(long long, long long)> comparador,
	const std::vector<ScanResultInt64>& resultadosPrevios,
	size_t step
) {
	std::vector<ScanResultInt64> resultado;
	if (!hProcess) return resultado;

	if (resultadosPrevios.empty()) return resultado;

	for (const auto& prev : resultadosPrevios) {
		long long valorLeido = 0;
		SIZE_T bytesRead = 0;
		if (ReadProcessMemory(hProcess, prev.direccion, &valorLeido, sizeof(long long), &bytesRead) && bytesRead == sizeof(long long)) {
			if (comparador(valorLeido, prev.valorActual)) {
				resultado.push_back({ prev.direccion, prev.valorActual, valorLeido });
			}
		}
	}
	return resultado;
}

std::vector<ScanResultFloat> BuscarFloatEnMemoria(
	HANDLE hProcess,
	float valor,
	std::function<bool(float, float)> comparador,
	const std::vector<ScanResultFloat>& resultadosPrevios,
	size_t step
) {
	std::vector<ScanResultFloat> resultado;
	if (!hProcess) return resultado;

	if (resultadosPrevios.empty()) {
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		LPCVOID addr = si.lpMinimumApplicationAddress;
		LPCVOID maxAddr = si.lpMaximumApplicationAddress;
		MEMORY_BASIC_INFORMATION mbi;

		while (addr < maxAddr) {
			if (VirtualQueryEx(hProcess, addr, &mbi, sizeof(mbi)) == sizeof(mbi)) {
				if ((mbi.State == MEM_COMMIT) &&
					(
						(mbi.Protect & PAGE_READWRITE) ||
						(mbi.Protect & PAGE_WRITECOPY) ||
						(mbi.Protect & PAGE_EXECUTE_READWRITE) ||
						(mbi.Protect & PAGE_EXECUTE_WRITECOPY)
						) &&
					!(mbi.Protect & PAGE_GUARD) &&
					!(mbi.Protect & PAGE_NOACCESS)) {

					std::vector<char> buffer(mbi.RegionSize);
					SIZE_T bytesRead = 0;
					if (ReadProcessMemory(hProcess, mbi.BaseAddress, buffer.data(), mbi.RegionSize, &bytesRead) && bytesRead > 0) {
						for (SIZE_T offset = 0; offset <= buffer.size() - sizeof(float); offset += step) {
							float valorLeido;
							memcpy(&valorLeido, &buffer[offset], sizeof(float));
							if (comparador(valorLeido, valor)) {
								LPCVOID foundAddr = (BYTE*)mbi.BaseAddress + offset;
								resultado.push_back({ foundAddr, valorLeido, valorLeido });
							}
						}
					}
				}
				addr = (BYTE*)mbi.BaseAddress + mbi.RegionSize;
			}
			else {
				break;
			}
		}
	}
	else {
		for (const auto& prev : resultadosPrevios) {
			float valorLeido = 0;
			SIZE_T bytesRead = 0;
			if (ReadProcessMemory(hProcess, prev.direccion, &valorLeido, sizeof(float), &bytesRead) && bytesRead == sizeof(float)) {
				if (comparador(valorLeido, valor)) {
					resultado.push_back({ prev.direccion, prev.valorActual, valorLeido });
				}
			}
		}
	}
	return resultado;
}

std::vector<ScanResultFloat> BuscarFloatEnMemoriaEntreValores(
	HANDLE hProcess,
	float valorMin,
	float valorMax,
	const std::vector<ScanResultFloat>& resultadosPrevios,
	size_t step
) {
	return BuscarFloatEnMemoria(
		hProcess,
		valorMin,
		[valorMin, valorMax](float a, float /*b*/) { return a >= valorMin && a <= valorMax; },
		resultadosPrevios,
		step
	);
}

std::vector<ScanResultFloat> BuscarTodosFloatEnMemoria(HANDLE hProcess, size_t step) {
	std::vector<ScanResultFloat> resultado;
	if (!hProcess) return resultado;

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	LPCVOID addr = si.lpMinimumApplicationAddress;
	LPCVOID maxAddr = si.lpMaximumApplicationAddress;
	MEMORY_BASIC_INFORMATION mbi;

	while (addr < maxAddr) {
		if (VirtualQueryEx(hProcess, addr, &mbi, sizeof(mbi)) == sizeof(mbi)) {
			if ((mbi.State == MEM_COMMIT) &&
				(
					(mbi.Protect & PAGE_READWRITE) ||
					(mbi.Protect & PAGE_WRITECOPY) ||
					(mbi.Protect & PAGE_EXECUTE_READWRITE) ||
					(mbi.Protect & PAGE_EXECUTE_WRITECOPY)
					) &&
				!(mbi.Protect & PAGE_GUARD) &&
				!(mbi.Protect & PAGE_NOACCESS)) {

				std::vector<char> buffer(mbi.RegionSize);
				SIZE_T bytesRead = 0;
				if (ReadProcessMemory(hProcess, mbi.BaseAddress, buffer.data(), mbi.RegionSize, &bytesRead) && bytesRead > 0) {
					for (SIZE_T offset = 0; offset <= buffer.size() - sizeof(float); offset += step) {
						float valorLeido = 0;
						memcpy(&valorLeido, &buffer[offset], sizeof(float));
						LPCVOID foundAddr = (BYTE*)mbi.BaseAddress + offset;
						resultado.push_back({ foundAddr, valorLeido, valorLeido });
					}
				}
			}
			addr = (BYTE*)mbi.BaseAddress + mbi.RegionSize;
		}
		else {
			break;
		}
	}
	return resultado;
}

std::vector<ScanResultFloat> FiltrarFloatPorComparador(
	HANDLE hProcess,
	std::function<bool(float, float)> comparador,
	const std::vector<ScanResultFloat>& resultadosPrevios,
	size_t step
) {
	std::vector<ScanResultFloat> resultado;
	if (!hProcess) return resultado;

	if (resultadosPrevios.empty()) return resultado;

	for (const auto& prev : resultadosPrevios) {
		float valorLeido = 0;
		SIZE_T bytesRead = 0;
		if (ReadProcessMemory(hProcess, prev.direccion, &valorLeido, sizeof(float), &bytesRead) && bytesRead == sizeof(float)) {
			if (comparador(valorLeido, prev.valorActual)) {
				resultado.push_back({ prev.direccion, prev.valorActual, valorLeido });
			}
		}
	}

	return resultado;
}

std::vector<ScanResultDouble> BuscarDoubleEnMemoria(
	HANDLE hProcess,
	double valor,
	std::function<bool(double, double)> comparador,
	const std::vector<ScanResultDouble>& resultadosPrevios,
	size_t step
) {
	std::vector<ScanResultDouble> resultado;
	if (!hProcess) return resultado;

	if (resultadosPrevios.empty()) {
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		LPCVOID addr = si.lpMinimumApplicationAddress;
		LPCVOID maxAddr = si.lpMaximumApplicationAddress;
		MEMORY_BASIC_INFORMATION mbi;

		while (addr < maxAddr) {
			if (VirtualQueryEx(hProcess, addr, &mbi, sizeof(mbi)) == sizeof(mbi)) {
				if ((mbi.State == MEM_COMMIT) &&
					(
						(mbi.Protect & PAGE_READWRITE) ||
						(mbi.Protect & PAGE_WRITECOPY) ||
						(mbi.Protect & PAGE_EXECUTE_READWRITE) ||
						(mbi.Protect & PAGE_EXECUTE_WRITECOPY)
						) &&
					!(mbi.Protect & PAGE_GUARD) &&
					!(mbi.Protect & PAGE_NOACCESS)) {

					std::vector<char> buffer(mbi.RegionSize);
					SIZE_T bytesRead = 0;
					if (ReadProcessMemory(hProcess, mbi.BaseAddress, buffer.data(), mbi.RegionSize, &bytesRead) && bytesRead > 0) {
						for (SIZE_T offset = 0; offset <= buffer.size() - sizeof(double); offset += step) {
							double valorLeido;
							memcpy(&valorLeido, &buffer[offset], sizeof(double));
							if (comparador(valorLeido, valor)) {
								LPCVOID foundAddr = (BYTE*)mbi.BaseAddress + offset;
								resultado.push_back({ foundAddr, valorLeido, valorLeido });
							}
						}
					}
				}
				addr = (BYTE*)mbi.BaseAddress + mbi.RegionSize;
			}
			else {
				break;
			}
		}
	}
	else {
		for (const auto& prev : resultadosPrevios) {
			double valorLeido = 0;
			SIZE_T bytesRead = 0;
			if (ReadProcessMemory(hProcess, prev.direccion, &valorLeido, sizeof(double), &bytesRead) && bytesRead == sizeof(double)) {
				if (comparador(valorLeido, valor)) {
					resultado.push_back({ prev.direccion, prev.valorActual, valorLeido });
				}
			}
		}
	}
	return resultado;
}

std::vector<ScanResultDouble> BuscarDoubleEnMemoriaEntreValores(
	HANDLE hProcess,
	double valorMin,
	double valorMax,
	const std::vector<ScanResultDouble>& resultadosPrevios,
	size_t step
) {
	return BuscarDoubleEnMemoria(
		hProcess,
		valorMin,
		[valorMin, valorMax](double a, double /*b*/) { return a >= valorMin && a <= valorMax; },
		resultadosPrevios,
		step
	);
}

std::vector<ScanResultDouble> BuscarTodosDoubleEnMemoria(HANDLE hProcess, size_t step) {
	std::vector<ScanResultDouble> resultado;
	if (!hProcess) return resultado;

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	LPCVOID addr = si.lpMinimumApplicationAddress;
	LPCVOID maxAddr = si.lpMaximumApplicationAddress;
	MEMORY_BASIC_INFORMATION mbi;

	while (addr < maxAddr) {
		if (VirtualQueryEx(hProcess, addr, &mbi, sizeof(mbi)) == sizeof(mbi)) {
			if ((mbi.State == MEM_COMMIT) &&
				(
					(mbi.Protect & PAGE_READWRITE) ||
					(mbi.Protect & PAGE_WRITECOPY) ||
					(mbi.Protect & PAGE_EXECUTE_READWRITE) ||
					(mbi.Protect & PAGE_EXECUTE_WRITECOPY)
					) &&
				!(mbi.Protect & PAGE_GUARD) &&
				!(mbi.Protect & PAGE_NOACCESS)) {

				std::vector<char> buffer(mbi.RegionSize);
				SIZE_T bytesRead = 0;
				if (ReadProcessMemory(hProcess, mbi.BaseAddress, buffer.data(), mbi.RegionSize, &bytesRead) && bytesRead > 0) {
					for (SIZE_T offset = 0; offset <= buffer.size() - sizeof(double); offset += step) {
						double valorLeido = 0;
						memcpy(&valorLeido, &buffer[offset], sizeof(double));
						LPCVOID foundAddr = (BYTE*)mbi.BaseAddress + offset;
						resultado.push_back({ foundAddr, valorLeido, valorLeido });
					}
				}
			}
			addr = (BYTE*)mbi.BaseAddress + mbi.RegionSize;
		}
		else {
			break;
		}
	}
	return resultado;
}

std::vector<ScanResultDouble> FiltrarDoubleEnMemoria(
	HANDLE hProcess,
	std::function<bool(double, double)> comparador,
	const std::vector<ScanResultDouble>& resultadosPrevios,
	size_t step
) {
	std::vector<ScanResultDouble> resultado;
	if (!hProcess) return resultado;

	if (resultadosPrevios.empty()) return resultado;

	for (const auto& prev : resultadosPrevios) {
		double valorLeido = 0;
		SIZE_T bytesRead = 0;
		if (ReadProcessMemory(hProcess, prev.direccion, &valorLeido, sizeof(double), &bytesRead) && bytesRead == sizeof(double)) {
			if (comparador(valorLeido, prev.valorActual)) {
				resultado.push_back({ prev.direccion, prev.valorActual, valorLeido });
			}
		}
	}

	return resultado;
}

void IncrementarContadorEscaneos(HWND hLabelNroEscaneos) {
	++numEscaneo;
	if (hLabelNroEscaneos) {
		std::wstring txt = L" Numero de escaneos: " + std::to_wstring(numEscaneo);
		SetWindowTextW(hLabelNroEscaneos, txt.c_str());
	}
}