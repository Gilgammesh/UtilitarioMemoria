#include <stdio.h>
#include <string>
#include <vector>
#include <windows.h>
#include <cmath>
#include <commctrl.h>
#include "BusquedaMemoria.h"
#include "ListaBusqueda.h"

extern HWND hListMain;
extern HWND hLblTotalRegistros;

// Define resultados previos
static std::vector<ScanResultString> resultadosPreviosString;
static std::vector<ScanResultInt> resultadosPreviosInt;
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

void BuscarEnMemoria(HANDLE hProc, const wchar_t* valor, int tipoValor, int tipoBusqueda)
{
	switch (tipoValor) {
	case 0: // String
	{
		std::wstring ws(valor);
		std::string sValor = WideToUtf8(ws);
		auto resultados = BuscarStringsEnMemoria(hProc, sValor, resultadosPreviosString, 1);
		resultadosPreviosString = resultados;
		ListView_DeleteAllItems(hListMain);
		ActualizarListViewPrincipalString(hListMain, resultados, hLblTotalRegistros);

		break;
	}
	case 1: // Byte
	{
		std::wstring ws(valor);
		try {
			int valorInt = std::stoi(ws);
			auto resultados = BuscarIntsEnMemoria(hProc, valorInt, resultadosPreviosInt, 1);
			resultadosPreviosInt = resultados;
			ListView_DeleteAllItems(hListMain);
			ActualizarListViewPrincipalInt(hListMain, resultados, hLblTotalRegistros, L"Int8");
		}
		catch (const std::exception&) {
			MessageBoxW(NULL, L"El valor ingresado no es un numero entero valido.", L"Error", MB_OK | MB_ICONERROR);
		}
		break;
	}
	case 2: // 2 Bytes
	{
		std::wstring ws(valor);
		try {
			int valorInt = std::stoi(ws);
			auto resultados = BuscarIntsEnMemoria(hProc, valorInt, resultadosPreviosInt, 2);
			resultadosPreviosInt = resultados;
			ListView_DeleteAllItems(hListMain);
			ActualizarListViewPrincipalInt(hListMain, resultados, hLblTotalRegistros, L"Int16");
		}
		catch (const std::exception&) {
			MessageBoxW(NULL, L"El valor ingresado no es un numero entero valido.", L"Error", MB_OK | MB_ICONERROR);
		}
		break;
	}
	case 3: // 4 Bytes
	{
		std::wstring ws(valor);
		try {
			int valorInt = std::stoi(ws);
			auto resultados = BuscarIntsEnMemoria(hProc, valorInt, resultadosPreviosInt, 4);
			resultadosPreviosInt = resultados;
			ListView_DeleteAllItems(hListMain);
			ActualizarListViewPrincipalInt(hListMain, resultados, hLblTotalRegistros, L"Int32");
		}
		catch (const std::exception&) {
			MessageBoxW(NULL, L"El valor ingresado no es un numero entero valido.", L"Error", MB_OK | MB_ICONERROR);
		}
		break;
	}
	case 4: // 8 Bytes
	{
		std::wstring ws(valor);
		try {
			int valorInt = std::stoi(ws);
			auto resultados = BuscarIntsEnMemoria(hProc, valorInt, resultadosPreviosInt, 4);
			resultadosPreviosInt = resultados;
			ListView_DeleteAllItems(hListMain);
			ActualizarListViewPrincipalInt(hListMain, resultados, hLblTotalRegistros, L"Int64");
		}
		catch (const std::exception&) {
			MessageBoxW(NULL, L"El valor ingresado no es un numero entero valido.", L"Error", MB_OK | MB_ICONERROR);
		}
		break;
	}
	case 5: // Float
	{
		std::wstring ws(valor);
		std::string sValor = WideToUtf8(ws);
		try {
			float valorFloat = std::stof(sValor);
			auto resultados = BuscarFloatEnMemoria(hProc, valorFloat, resultadosPreviosFloat, 4);
			resultadosPreviosFloat = resultados;
			ListView_DeleteAllItems(hListMain);
			ActualizarListViewPrincipalFloat(hListMain, resultados, hLblTotalRegistros);
		}
		catch (const std::exception&) {
			MessageBoxW(NULL, L"El valor ingresado no es un numero decimal valido.", L"Error", MB_OK | MB_ICONERROR);
		}
		break;
	}
	case 6: // Double
	{
		std::wstring ws(valor);
		std::string sValor = WideToUtf8(ws);
		try {
			double valorDouble = std::stod(sValor);
			auto resultados = BuscarDoubleEnMemoria(hProc, valorDouble, resultadosPreviosDouble, 4);
			resultadosPreviosDouble = resultados;
			ListView_DeleteAllItems(hListMain);
			ActualizarListViewPrincipalDouble(hListMain, resultados, hLblTotalRegistros);
		}
		catch (const std::exception&) {
			MessageBoxW(NULL, L"El valor ingresado no es un numero decimal valido.", L"Error", MB_OK | MB_ICONERROR);
		}
		break;
	}
	case 7: // Direccion Memoria
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

			// Mostrar logs
			char msg[1024];
			sprintf_s(msg, sizeof(msg), "String: %hs\n",
				asciiString.c_str());
			OutputDebugStringA(msg);
			sprintf_s(msg, sizeof(msg), "Int8: %d\nInt16: %d\nInt32: %d\nInt64: %lld\n",
				valInt8, valInt16, valInt32, valInt64);
			OutputDebugStringA(msg);
			sprintf_s(msg, sizeof(msg), "Float: %g\nDouble: %g\n", valFloat, valDouble);
			OutputDebugStringA(msg);

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
							if (valorLeido == valor) {
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
				if (valorLeido == valor) {
					resultado.push_back({ prev.direccion, prev.valorActual, valorLeido });
				}
			}
		}
	}
	return resultado;
}

std::vector<ScanResultFloat> BuscarFloatEnMemoria(
	HANDLE hProcess,
	float valor,
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
							if (valorLeido == valor) {  // Comparacion exacta
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
				if (valorLeido == valor) {
					resultado.push_back({ prev.direccion, prev.valorActual, valorLeido });
				}
			}
		}
	}
	return resultado;
}


std::vector<ScanResultDouble> BuscarDoubleEnMemoria(
	HANDLE hProcess,
	double valor,
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
							if (valorLeido == valor) {
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
				if (valorLeido == valor) {
					resultado.push_back({ prev.direccion, prev.valorActual, valorLeido });
				}
			}
		}
	}
	return resultado;
}