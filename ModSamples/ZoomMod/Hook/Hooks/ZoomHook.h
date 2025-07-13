#ifndef ZOOM_HOOK_H
#define ZOOM_HOOK_H

#include <cstdint>

class LevelRendererPlayer {
public:
	uintptr_t** __vftable; // 0x0000

	OFFSET_FIELD(float, Zoom, 0xFD8);
};

void animateTickDetour(LevelRendererPlayer* _this)
{
	CallFunc<void, LevelRendererPlayer*>(
		HookComps::__o__Zoom,
		_this
	);

	if (GameCore::IsZooming)
	{
		auto factor = config.GetOrDefault<float>("FovFactor", 0.3f);
		_this->Zoom(factor);
	}
}

class ZoomHook : public FuncHook {
public:
	bool Initialize() override {
		uintptr_t offset = NativeCore::FetchOffset("LevelRendererPlayer::animateTick", "48 8B C4 48 89 58 10 48 89 70 18 48 89 78 20 55 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? ? ? 70 C8 44");
		if (!NativeCore::HookFunction(offset, &animateTickDetour, &HookComps::__o__Zoom))
		{
			Console::Log("ZoomMod", "Failed to hook animateTick function");
			return false;
		}

		Console::Log("ZoomMod", "Zoom hook initialized successfully at offset %x", offset);

		return true;
	}

	static ZoomHook& Get() {
		static ZoomHook instance;
		return instance;
	}
};


#endif // !ZOOM_HOOK_H
