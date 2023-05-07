#pragma once

//-----------------------------------------------------------------------------
// Simple string class. 
// NOTE: This is *not* optimal! Use in tools, but not runtime code
//-----------------------------------------------------------------------------
class CUtlString
{
public:
	typedef enum
	{
		PATTERN_NONE = 0x00000000,
		PATTERN_DIRECTORY = 0x00000001
	} TUtlStringPattern;

	CUtlString() : m_pString(nullptr) {};

	const char* Get() const
	{
		if (!m_pString)
			return "";

		return m_pString;
	}
	operator const char* () const { return Get(); }

	// for compatibility switching items from UtlSymbol
	const char* String() const { return Get(); }

	// Returns strlen
	size_t		Length() const
	{
		if (m_pString)
			return strlen(m_pString);

		return 0;
	}

	// IsEmpty() is more efficient than Length() == 0
	bool		IsEmpty() const { return !m_pString || m_pString[0] == 0; }

	bool		IsEqual_CaseSensitive(const char* src) const
	{
		if (!src)
			return (Length() == 0);

		return (strcmp(Get(), src) == 0);
	}

	bool		IsEqual_CaseInsensitive(const char* src) const
	{
		if (!src)
			return (Length() == 0);

		return (_stricmp(Get(), src) == 0);
	}


	// Test for equality
	bool operator==(const CUtlString& src) const
	{
		if (IsEmpty())
		{
			if (src.IsEmpty())
				return true;

			return false;
		}
		else
		{
			if (src.IsEmpty())
				return false;

			return (strcmp(m_pString, src.m_pString) == 0);
		}
	}
	bool operator!=(const CUtlString& src) const { return !operator==(src); }

	char operator[](int i) const
	{
		if (!m_pString)
			return '\0';

		if (i >= Length())
			return '\0';

		return m_pString[i];
	}

	// Defining AltArgumentType_t hints that associative container classes should
	// also implement Find/Insert/Remove functions that take const char* params.
	typedef const char* AltArgumentType_t;

	// These can be used for utlvector sorts.
	static int __cdecl SortCaseInsensitive(const CUtlString* pString1, const CUtlString* pString2) { return _stricmp(pString1->String(), pString2->String()); }
	static int __cdecl SortCaseSensitive(const CUtlString* pString1, const CUtlString* pString2) { return strcmp(pString1->String(), pString2->String()); }

	// Empty string for those times when you need to return an empty string and
	// either don't want to pay the construction cost, or are returning a
	// const CUtlString& and cannot just return "".
	static const CUtlString& GetEmptyString()
	{
		static const CUtlString s_emptyString;
		return s_emptyString;
	}

private:
	// If m_pString is not NULL, it points to the start of the string, and the memory allocation.
	char* m_pString;
};