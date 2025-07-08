#include <map>
#include <string>

namespace NativeCore {
	std::map<std::string, NativeOffset> g_Offsets;

	// TODO: link this to g_offsets
	template <typename R, typename... Args>
	R CallFunc(void* func, Args... args) { return ((R(*)(Args...))func)(args...); }
	template <typename R, typename... Args>
	R CallFunc(uintptr_t func, Args... args) { return ((R(*)(Args...))func)(args...); }

	uintptr_t range_start = (uintptr_t)GetModuleHandleA("Minecraft.Windows.exe");
}
