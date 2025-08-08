#ifndef XEPHMARK_BSONFORMATTER_H
#define XEPHMARK_BSONFORMATTER_H

#include <XephMark/IOFormatter.h>

namespace XephMark
{
	class BSONFormatter : public IOFormatterBinary
	{
	public:
		BSONFormatter() = default;
		virtual ~BSONFormatter() = default;

		bool SaveToFile(const Node& node, const std::filesystem::path& path) const override;
		Node LoadFromFile(const std::filesystem::path& path) const override;

		std::vector<uint8_t> Dump(const Node& node) const override;
		Node Read(const std::vector<uint8_t>& contents) const override;
	};
}

#endif // !XEPHMARK_BSONFORMATTER_H