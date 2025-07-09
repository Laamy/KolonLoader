#include <Psapi.h>

#define INRANGE(x, a, b) (x >= a && x <= b)
#define GET_BYTE(x) ((GET_BITS(x[0]) << 4) | GET_BITS(x[1]))
#define GET_BITS(x) (INRANGE((x & (~0x20)), 'A', 'F') ? ((x & (~0x20)) - 'A' + 0xa) : (INRANGE(x, '0', '9') ? (x - '0') : 0))

uintptr_t range_start = reinterpret_cast<uintptr_t>(GetModuleHandleA("Minecraft.Windows.exe"));

uintptr_t findSig_MS(const char* sig) {
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