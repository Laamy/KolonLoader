#include <TlHelp32.h>
#ifndef KEYMAP_HOOK_H
#define KEYMAP_HOOK_H

std::string KeyToString(int keycode) {
	UINT scanCode = MapVirtualKeyA(keycode, MAPVK_VK_TO_VSC);
	char buffer[64];
	if (GetKeyNameTextA(scanCode << 16, buffer, sizeof(buffer)) != 0)
		return buffer;
	return "Unknown";
}

void KeypressDetour(int key, bool held)
{
	Console::Log("MoreInputs", "Key %s %s", KeyToString(key).c_str(), held ? "held" : "released");

	if (key == 'C')
		GameCore::IsZooming = held;
				
	return CallFunc<void, int, bool>(
		HookComps::__o__Keypress,
		key, held
	);
}

class KeymapHook : public FuncHook {
public:
	bool Initialize() override {
		uintptr_t offset = NativeCore::FetchOffset("Keymap", "48 83 EC 48 ? ? C1 4C 8D");
		if (!NativeCore::HookFunction(offset, &KeypressDetour, &HookComps::__o__Keypress))
		{
			Console::Log("MoreInputs", "Failed to hook keypress function");
			return false;
		}

		Console::Log("MoreInputs", "Keymap hook initialized successfully %x", offset);

		return true;
	}

	static KeymapHook& Get() {
		static KeymapHook instance;
		return instance;
	}
};

#endif // !KEYMAP_HOOK_H
