#ifndef BASE_OPTION_HOOK_H
#define BASE_OPTION_HOOK_H

class BaseOption {
public:
	uintptr_t** vftable;

	OFFSET_FIELD(bool, Enabled, 0x10);
};

BaseOption* GetOption(uintptr_t _settings, unsigned __int16 offset) {
	uintptr_t(__fastcall * v2)(uintptr_t, char**, uintptr_t);
	uintptr_t optionPtr;
	char* v9;

	uintptr_t** funcPtr = reinterpret_cast<uintptr_t**>(*(uintptr_t*)_settings + 0x60);
	optionPtr = CallFunc<uintptr_t, uintptr_t, char**, uintptr_t>(funcPtr[0], _settings, &v9, offset);

	return *(BaseOption**)optionPtr;
}

bool BaseOptionsDetour(uintptr_t _this)
{
	static bool prevZoom = false;
	static uintptr_t hideStart = 0x1C9; // inside the function used to locate this (might just get this from the func addr automatically)
	static uintptr_t hideStart2 = 0x30; // just fucking guessed this using a for loop

	auto hideCursor = GetOption(_this, hideStart);
	auto hideHand = GetOption(_this, hideStart + 1);
	auto hideGui = GetOption(_this, hideStart2);

	// BUG: f1 while in zoom will unhide hand
	if (GameCore::IsZooming && !prevZoom)
	{
		hideCursor->Enabled(true);
		hideHand->Enabled(true);
		hideGui->Enabled(true);
	}

	if (!GameCore::IsZooming && prevZoom)
	{
		hideCursor->Enabled(false);
		hideHand->Enabled(false);
		hideGui->Enabled(false);
	}
	prevZoom = GameCore::IsZooming;

	return CallFunc<bool, uintptr_t>(
		HookComps::__o__Options,
		_this
	);
}

class BaseOptionsHook : public FuncHook {
public://Minecraft.Windows.exe+B45EE0
	//Minecraft.Windows.exe+B426C0

	bool Initialize() override {//48 8B 40 ? 0F 84 ? ? ? ? 41
		uintptr_t offset = NativeCore::FetchOffset("BaseOptions", "40 53 48 83 EC 20 48 8B 01 48 8D");
		if (!NativeCore::HookFunction(offset, &BaseOptionsDetour, &HookComps::__o__Options))
		{
			Console::Log("ZoomMod", "Failed to hook BaseOptions function");
			return false;
		}

		Console::Log("ZoomMod", "Options initialized successfully at offset %x", offset);

		return true;
	}

	static BaseOptionsHook& Get() {
		static BaseOptionsHook instance;
		return instance;
	}
};

#endif // !BASE_OPTION_HOOK_H
