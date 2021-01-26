/*
Original author: Tonyx97
Repo: https://github.com/Tonyx97/Tomb-Raider-II-Multiplayer-Mod
*/

#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <iostream>
#include <stdint.h>
#include <cmath>

struct ivec3
{
	int32_t x = 0,
		y = 0,
		z = 0;

	ivec3() {}
	ivec3(int32_t _x, int32_t _y, int32_t _z) : x(_x), y(_y), z(_z) {}

	ivec3 operator + (const ivec3& v) { return ivec3(x + v.x, y + v.y, z + v.z); }

	ivec3 operator - (const ivec3& v) { return ivec3(x - v.x, y - v.y, z - v.z); }

	int32_t distance(const ivec3& v) { return static_cast<int32_t>(std::sqrtf(static_cast<float>((v.x - x) * (v.x - x) + (v.y - y) * (v.y - y) + (v.z - z) * (v.z - z)))); }

	ivec3 interpolate_to(const ivec3& v, float factor)
	{
		x = static_cast<int32_t>(std::lerp(static_cast<float>(x), static_cast<float>(v.x), factor));
		y = static_cast<int32_t>(std::lerp(static_cast<float>(y), static_cast<float>(v.y), factor));
		z = static_cast<int32_t>(std::lerp(static_cast<float>(z), static_cast<float>(v.z), factor));

		return *this;
	}
};


struct small_ivec3
{
	uint16_t x = 0, y = 0, z = 0;

	small_ivec3() {}
	small_ivec3(uint16_t _x, uint16_t _y, uint16_t _z) : x(_x), y(_y), z(_z) {}

	small_ivec3 operator + (const small_ivec3& v)
	{
		return small_ivec3(x + v.x, y + v.y, z + v.z);
	}

	small_ivec3 operator - (const small_ivec3& v)
	{
		return small_ivec3(x - v.x, y - v.y, z - v.z);
	}

	small_ivec3 interpolate_to(const small_ivec3& v, float factor)
	{
		x = static_cast<uint32_t>(std::lerp(static_cast<float>(x), static_cast<float>(v.x), factor));
		y = static_cast<uint16_t>(std::lerp(static_cast<float>(y), static_cast<float>(v.y), factor));
		z = static_cast<uint32_t>(std::lerp(static_cast<float>(z), static_cast<float>(v.z), factor));

		return *this;
	}
};



struct Entity
{
	int32_t floor_y;			// 0x0
	uint32_t _0x4,				// 0x4
		mesh_bits;			// 0x8
	uint16_t model,				// 0xC
		_0xe,				// 0xE
		_0x10,				// 0x10
		_0x12;				// 0x12
	uint16_t anim_id,			// 0x14
		anim_frame,		// 0x16
		room_id,			// 0x18
		room_unk_data,		// 0x1A
		interactive_id;	// 0x1C
	uint16_t xz_velocity,		// 0x1E
		y_velocity,		// 0x20
		health;			// 0x22
	char pad2[0x4];				// 0x24
	uint16_t activation_flags,	// 0x28
		intensity;			// 0x2A
	uint16_t pad3,				// 0x2C
		parent;			// 0x2E
	uintptr_t ai_manager;		// 0x30
	ivec3 position;				// 0x34
	small_ivec3 rotation;		// 0x40
	uint16_t flags;				// 0x46
};
