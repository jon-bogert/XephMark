#ifndef XEPHMARK_IOFORMATTER_H
#define XEPHMARK_IOFORMATTER_H

#include <XephMark/Node.h>

#include <string>
#include <fstream>
#include <filesystem>

namespace XephMark
{
	class IOFormatter
	{
	public:
		virtual bool SaveToFile(const Node& node, const std::filesystem::path& path) const = 0;
		virtual Node LoadFromFile(const std::filesystem::path& path) const = 0;
	};

	class IOFormatterText : public IOFormatter
	{
		virtual std::string Dump(const Node& node) const = 0;
		virtual Node Read(const std::string& contents) const = 0;
	};

	class IOFormatterBinary : public IOFormatter
	{
		virtual std::vector<uint8_t> Dump(const Node& node) const = 0;
		virtual Node Read(const std::vector<uint8_t>& contents) const = 0;
	};
}

#endif //!XEPHMARK_IOFORMATTER_H