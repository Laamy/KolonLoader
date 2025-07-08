#include "NativeCore.h"

#include "minhook/minhook.h"

#include "_NativeCore.h"

#define INRANGE(x, a, b) (x >= a && x <= b)
#define GET_BYTE(x) ((GET_BITS(x[0]) << 4) | GET_BITS(x[1]))
#define GET_BITS(x) (INRANGE((x & (~0x20)), 'A', 'F') ? ((x & (~0x20)) - 'A' + 0xa) : (INRANGE(x, '0', '9') ? (x - '0') : 0))

uintptr_t NativeCore::findSig_MS(const char* sig) {
	const char* pattern = sig;
	uintptr_t first_match = 0;
	static MODULEINFO mi_mod_info;
	static bool init = false;

	if (!init)
	{
		init = true;
		GetModuleInformation(GetCurrentProcess(), reinterpret_cast<HMODULE>(range_start), &mi_mod_info, sizeof(MODULEINFO));
	}
	static const uintptr_t range_end = range_start + mi_mod_info.SizeOfImage;

	BYTE pat_byte = GET_BYTE(pattern);
	const char* old_pat = pattern;

	int msEnd = 0;

	for (uintptr_t p_cur = range_start; p_cur < range_end; p_cur++)
	{
		if (!*pattern)
			return first_match;

		while (*(PBYTE)pattern == ' ')
			pattern++;

		if (!*pattern)
			return first_match;

		if (old_pat != pattern)
		{
			old_pat = pattern;
			if (
				*(PBYTE)pattern != '\?' &&
				*(PBYTE)pattern != '\!'
				) {
				pat_byte = GET_BYTE(pattern);
			}
		}

		if (*(PBYTE)pattern == '\!')
		{
			if (!first_match)
				first_match = p_cur;

			if (!pattern[2])
				return first_match;

			bool found = false;
			for (uintptr_t i = 0; i < 0x10 && p_cur + i < range_end; i++) {
				BYTE next_byte = GET_BYTE((pattern + 2));
				if (next_byte == 0 || *(PBYTE)(pattern + 2) == '?' || *(BYTE*)(p_cur + i) == next_byte) {
					pattern += 2;
					p_cur += i - 1;
					found = true;
					break;
				}
			}
			if (!found) {
				pattern = sig;
				first_match = 0;
			}
		}
		else if (*(PBYTE)pattern == '\?' || *(BYTE*)p_cur == pat_byte)
		{
			if (!first_match)
				first_match = p_cur;

			if (!pattern[2])
				return first_match;

			pattern += 2;
		}
		else
		{
			pattern = sig;
			first_match = 0;
		}
	}

	return 0;
}

// Class::Func+Offset(If any)|(overload number)
// Class::Func (no offset or overload)
// Class::Func+1|1 (1 byte into the function & second overload)
NativeOffset NativeCore::FetchOffset(const char* sidId, const char* sig) {
	auto it = g_Offsets.find(sidId);

	if (it != g_Offsets.end())
		return it->second;

	// not found
	uintptr_t sigResult = findSig_MS(sig);
	if (sigResult == NULL)
	{
		Console::Log("NativeCore", "Failed to find signature for %s (It's possible someone didnt name their sig properly so kolan cant find it)", sidId);
		return {};
	}

	NativeOffset offset = {};
	offset.detourPtr = sigResult;
	offset.original = NULL;
	g_Offsets[sidId] = offset;

	return offset;
}

bool NativeCore::HookFunction(void* pTarget, void* pDetour, void* pOriginal) {
	static bool initialized = false;

	if (!initialized) {
		initialized = true;

		MH_Initialize(); // NOTE: kiero also uses minhook so i should embed my own version of that in this dll
	}

	if (MH_CreateHook(pTarget, pDetour, (LPVOID*)pOriginal) != MH_OK) {
		Console::Log("NativeCore", "Failed to create hook for %p", pTarget);
		return false;
	}

	if (MH_EnableHook(pTarget) != MH_OK) {
		Console::Log("NativeCore", "Failed to enable hook for %p", pTarget);
		return false;
	}

	return true;
}
