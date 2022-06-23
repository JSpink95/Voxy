#include "Utility/FileIO.h"

namespace FileIO
{
	std::string LoadToString(std::string_view path)
	{
		std::string result = "";
	
		std::ifstream stream(path.data());
		if (stream.is_open())
		{
			result = std::string(
				  std::istreambuf_iterator<char>(stream)
				, std::istreambuf_iterator<char>());

			stream.close();
		}
	
		return result;
	}

	void ParseForFiles(std::string_view root, std::string_view ext, List<std::string>& files)
	{
		auto dir = std::filesystem::path(root);
		for (auto const& entry : std::filesystem::directory_iterator(dir))
		{
			if (entry.is_regular_file())
			{
				auto filepath = entry.path();
				if (filepath.has_extension() && filepath.extension() == ext)
				{
					Console::Log("Found File - {0}", entry.path().string());
					files.push_back(entry.path().string());
				}
			}
		}
	}

	void RecursiveParseForFiles(std::string_view root, std::string_view ext, List<std::string>& files)
	{
		List<std::string> extensions = { ext.data() };
		RecursiveParseForFiles(root, extensions, files);
	}

	void RecursiveParseForFiles(std::string_view root, List<std::string> const& extensions, List<std::string>& files)
	{
		auto dir = std::filesystem::path(root);
		for (auto const& entry : std::filesystem::recursive_directory_iterator(dir))
		{
			if (entry.is_regular_file())
			{
				auto filepath = entry.path();
				if (filepath.has_extension() && std::find(extensions.begin(), extensions.end(), filepath.extension()) != extensions.end())
				{
					files.push_back(entry.path().string());
				}
			}
		}
	}

	void RecursiveParseForFiles(std::string_view root, List<std::string> const& extensions, List<std::filesystem::path>& files)
	{
		auto dir = std::filesystem::path(root);
		for (auto const& entry : std::filesystem::recursive_directory_iterator(dir))
		{
			if (entry.is_regular_file())
			{
				auto filepath = entry.path();
				if (filepath.has_extension() && std::find(extensions.begin(), extensions.end(), filepath.extension()) != extensions.end())
				{
					files.push_back(entry.path());
				}
			}
		}
	}
}
