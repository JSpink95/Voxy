#pragma once
#define SDL_MAIN_HANDLED
#define GLEW_STATIC

#include <cstdint>
#include <memory>
#include <vector>
#include <array>
#include <string>
#include <format>
#include <Windows.h>

#define GLM_SWIZZLE
#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\norm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

#include <SDL.h>
#include <gl\glew.h>
#include <gl\gl.h>

using s8 = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using f32 = float;
using f64 = double;

#define BEGIN_BITCODE_DEF(Name) namespace Name {
#define BITCODE_VALUE(Name, Index) constexpr u32 Name = (1 << Index)
#define END_BITCODE_DEF() }

template<typename T, size_t size>
using Array = std::array<T, size>;

template<typename T>
using List = std::vector<T>;

template<typename T>
using Reference = std::shared_ptr<T>;

template<typename T>
using Unique = std::unique_ptr<T>;

template<typename T, typename...TArgs>
static inline Reference<T> MakeRef(TArgs&&...args)
{
	return std::make_shared<T>(std::forward<TArgs>(args)...);
}

template<typename To, typename From>
static inline Reference<To> Cast(Reference<From> from)
{
	return (from == nullptr) ? nullptr : std::dynamic_pointer_cast<To>(from);
}

// concepts
template<typename T, typename U>
concept Derived = std::is_base_of<U, T>::value;

template<typename T>
concept VertexType = requires(T) { T::Attributes; };

// console helpers

namespace Console
{
	template<typename...TArgs>
	inline void Log(std::string_view format, TArgs&&...args)
	{
		auto formatted = std::vformat(format, std::make_format_args(std::forward<TArgs>(args)...));
		printf("%s\n", formatted.c_str());
	}

	template<typename...TArgs>
	inline void LogWarning(std::string_view format, TArgs&&...args)
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);

		Log(format, std::forward<TArgs>(args)...);
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}

	template<typename...TArgs>
	inline void LogError(std::string_view format, TArgs&&...args)
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);

		Log(format, std::forward<TArgs>(args)...);
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}
}