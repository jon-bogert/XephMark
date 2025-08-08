#include <XephMark/YAMLFormatter.h>

#include <yaml-cpp/yaml.h>

using namespace XephMark;

void YAMLFormatter::SaveToFile(const Node& node, const std::string& path) const
{
	std::ofstream file(path);

	if (!file.is_open())
	{
		throw (std::exception("Could not open file for saving."));
	}

	file << Dump(node);
	file.close();
}

Node YAMLFormatter::LoadFromFile(const std::string& path) const
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		throw (std::exception("Could not open file for loading."));
	}
	std::string contents{};

	file.seekg(0, std::ios::end);
	contents.resize(file.tellg());
	file.seekg(0, std::ios::beg);
	file.read(contents.data(), contents.size());
	file.close();

	return Read(contents);
}

std::string YAMLFormatter::Dump(const Node& node) const
{
	YAML::Node root;


	return std::string();
}

Node YAMLFormatter::Read(const std::string& contents) const
{
	return Node();
}
