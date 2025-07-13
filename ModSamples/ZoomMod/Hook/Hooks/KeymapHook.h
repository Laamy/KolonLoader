#ifndef KEYMAP_HOOK_H
#define KEYMAP_HOOK_H

std::string KeyToString(int keycode) {
	UINT scanCode = MapVirtualKeyA(keycode, MAPVK_VK_TO_VSC);
	char buffer[64];
	if (GetKeyNameTextA(scanCode << 16, buffer, sizeof(buffer)) != 0)
		return buffer;
	return "Unknown";
}

// SO SKETCHY
int StringToKey(const std::string& keyName) {
	for (int vk = 1; vk < 256; ++vk) {
		UINT sc = MapVirtualKeyA(vk, MAPVK_VK_TO_VSC);
		char buffer[64];
		if (GetKeyNameTextA(sc << 16, buffer, sizeof(buffer)) != 0) {
			if (keyName == buffer)
				return vk;
		}
	}
	return -1;
}

void KeypressDetour(int key, bool held)
{
	//Console::Log("ZoomMod", "Key %s %s", KeyToString(key).c_str(), held ? "held" : "released");

	auto zoomKey = config.GetOrDefault<std::string>("ZoomKey", "C");
	if (key == StringToKey(zoomKey))
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
			Console::Log("ZoomMod", "Failed to hook keypress function");
			return false;
		}

		Console::Log("ZoomMod", "Keymap hook initialized successfully %x", offset);

		return true;
	}

	static KeymapHook& Get() {
		static KeymapHook instance;
		return instance;
	}
};

#endif // !KEYMAP_HOOK_H
