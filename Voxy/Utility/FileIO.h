#pragma once
#include "CoreTypes.h"

#include <filesystem>
#include <fstream>

namespace FileIO
{
	std::string LoadToString(std::string_view path);

	void ParseForFiles(std::string_view root, std::string_view ext, List<std::string>& files);
	void RecursiveParseForFiles(std::string_view root, std::string_view ext, List<std::string>& files);
	void RecursiveParseForFiles(std::string_view root, List<std::string> const& extensions, List<std::string>& files);
	void RecursiveParseForFiles(std::string_view root, List<std::string> const& extensions, List<std::filesystem::path>& files);
}