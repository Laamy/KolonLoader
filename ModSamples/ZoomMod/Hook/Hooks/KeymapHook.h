#ifndef KEYMAP_HOOK_H
#define KEYMAP_HOOK_H

void KeypressDetour(int key, bool held)
{
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
