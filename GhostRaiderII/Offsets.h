#pragma once
#include <cstdint>

namespace Offsets {

	class GOG_UK {
	public:
		static constexpr uintptr_t loadLevelFunc = 0x43C790;
		static constexpr uintptr_t renderLaraFunc = 0x419DD0;
		static constexpr uintptr_t title = 0x11BD90;
	};
	
	class Steam_UK {
	public:
		static constexpr uintptr_t loadLevelFunc = 0x43C780;
		static constexpr uintptr_t renderLaraFunc = 0x419DF0;
		static constexpr uintptr_t title = 0x11BD90;
	};
	
	class Steam_EPC {
	public:
		static constexpr uintptr_t loadLevelFunc = 0x43C860;
		static constexpr uintptr_t renderLaraFunc = 0x419DD0;
		static constexpr uintptr_t title = 0x11BD90;
	};
}
