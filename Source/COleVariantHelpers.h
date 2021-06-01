#pragma once

#include <afxdisp.h>
#include <afxstr.h>
#include <algorithm>
#include <codecvt>
#include <locale>
#include <string>




namespace COleVariantHelpers
{
	/// <summary>
	/// Convert a std::string to CString.
	/// </summary>
	/// <param name="str">std::string to convert.</param>
	/// <returns>CString with the same data as str.</returns>
	inline CString StringToCString(const std::string str)
	{
		return CString(std::string(str.begin(), str.end()).c_str());
	}

	/// <summary>
	/// Convert a CString to std::string taking into account that CString holds wchar_t.
	/// </summary>
	/// <param name="str">CString to convert.</param>
	/// <returns>std::string with the same value as CString with considerations for different formats.</returns>
	inline std::string CStringToString(const CString str)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
		return std::string(converter.to_bytes(static_cast<LPCTSTR>(str)));
	}

	/// <summary>
	/// Convert a WString to a regular string taking into account the differences in encoding.
	/// </summary>
	/// <param name="wstr">WString to convert.</param>
	/// <returns>The converted string.</returns>
	inline std::string WStringToString(const std::wstring wStr)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
		return std::string(converter.to_bytes(wStr.c_str()));
	}
	
	/// <summary>
	/// Convert COleVariant to a string.
	/// </summary>
	/// <param name="v">COleVariant that holds a string value.</param>
	/// <returns>std::string that contains the value of v.</returns>
	inline std::string COleVariantToString(const COleVariant v)
	{
		ASSERT(v.vt == VT_BSTR);
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
		return std::string(converter.to_bytes(static_cast<LPCTSTR>(v.bstrVal)));
	}

	/// <summary>
	/// Convert C++ bool to VARIANT_BOOL (true and false are represented differently, because of course).
	/// </summary>
	/// <param name="b">bool to convert.</param>
	/// <returns>VARIANT_BOOL equivilant to b.</returns>
	inline VARIANT_BOOL BoolToVARIANTBOOL(const bool b)
	{
		switch (b)
		{
		case true:
			return VARIANT_TRUE;
		case false:
			return VARIANT_FALSE;
		default:
			throw std::exception("Something went wrong - probably memory corruption.");
		}
	}

	/// <summary>
	/// Convert VARIANT_BOOL to bool (true and false are represented differently, because of course).
	/// </summary>
	/// <param name="b">VARIANT_BOOL to convert.</param>
	/// <returns>bool equivilant to b</returns>
	inline bool VARIANTBOOLToBool(const VARIANT_BOOL b)
	{
		switch (b)
		{
		case VARIANT_TRUE:
			return true;
		case VARIANT_FALSE:
			return false;
		default:
			throw std::exception("Something went wrong - probably memory corruption.");
		}
	}

	/// <summary>
	/// Convert an absolute path to a relative path from the working directory (typically where the .exe is located).
	/// </summary>
	/// <param name="absolutePath">The absolute path.</param>
	/// <returns>A relative path, if it is valid; an empty string otherwise.</returns>
	inline std::wstring ToRelativePath(std::wstring absolutePath)
	{
		// LPWSTR/LPWCSTR is defined as wchar_t, so no need to cast.
		
		wchar_t currentDirPath[MAX_PATH];
		if (GetCurrentDirectoryW(MAX_PATH, currentDirPath) == 0)
		{
			MessageBox(NULL, _T("A problem getting the current working directory has occurred. Place the executable file in another folder and try again."), _T("Error"), MB_OK);
			return std::wstring();
		}

		wchar_t outPath[MAX_PATH];
		if (!PathRelativePathToW(outPath, currentDirPath, FILE_ATTRIBUTE_DIRECTORY, CString(absolutePath.c_str()), NULL))
		{
			return std::wstring();
		}

		auto finalPath = std::wstring(outPath);
		std::replace(finalPath.begin(), finalPath.end(), '\\', '/'); // Replace two backward slashes with forward slashes.
		finalPath.erase(finalPath.begin(), std::next(finalPath.begin(), 2)); // Remove the "./" at the start of the path.
		
		return finalPath;
	}
}
