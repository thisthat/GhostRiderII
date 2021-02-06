#pragma once
#include <cstdint>

namespace Offsets {

	const uintptr_t VERSION_ADDRESS = 0x88;

	class MULTI {
	public:
		static constexpr uintptr_t loadLevelFunc = 0x43C830;
		static constexpr uintptr_t renderLaraFunc = 0x419DD0;
		static constexpr uintptr_t title = 0x11BD90;
		static constexpr byte version[3] = { 0xE3, 0xA8, 0x8E };
	};

	class EPC {
	public:
		static constexpr uintptr_t loadLevelFunc = 0x43C860;
		static constexpr uintptr_t renderLaraFunc = 0x419DD0;
		static constexpr uintptr_t title = 0x11BD90;
		static constexpr byte version[3] = { 0x03, 0xC4, 0x8F };
	};

	class UK {
	public:
		static constexpr uintptr_t loadLevelFunc = 0x43C780;
		static constexpr uintptr_t renderLaraFunc = 0x419DF0;
		static constexpr uintptr_t title = 0x11BDA0;
		static constexpr byte version[3] = { 0x7F, 0x76, 0x6A };
	};
	
	class PATCH {
	public:
		static constexpr uintptr_t loadLevelFunc = 0x43C790;
		static constexpr uintptr_t renderLaraFunc = 0x419DD0;
		static constexpr uintptr_t title = 0x11BD90;
		static constexpr byte version[3] = { 0xBC, 0xAB, 0x82 };
	};

	bool checkVersion(DWORD* baseAddress, const byte version[]) {
		byte *first  = (byte*)((char*)baseAddress + VERSION_ADDRESS + 0);
		byte *second = (byte*)((char*)baseAddress + VERSION_ADDRESS + 1);
		byte *third  = (byte*)((char*)baseAddress + VERSION_ADDRESS + 2);
		return *first == version[0] && *second == version[1] && *third == version[2];
	}

	bool isMulti(DWORD* baseAddress) {
		return checkVersion(baseAddress, MULTI::version);
	}

	bool isEPC(DWORD* baseAddress) {
		return checkVersion(baseAddress, EPC::version);
	}

	bool isUK(DWORD* baseAddress) {
		return checkVersion(baseAddress, UK::version);
	}

	bool isPatched(DWORD* baseAddress) {
		return checkVersion(baseAddress, PATCH::version);
	}
}
