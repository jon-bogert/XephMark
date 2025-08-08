#ifndef XEPHMARK_YAMLFORMATTER_H
#define XEPHMARK_YAMLFORMATTER_H

#include <XephMark/IOFormatter.h>

namespace XephMark
{
	class YAMLFormatter : public IOFormatterText
	{
	public:
		YAMLFormatter() = default;
		virtual ~YAMLFormatter() = default;

		void SaveToFile(const Node& node, const std::string& path) const override;
		Node LoadFromFile(const std::string& path) const override;

		std::string Dump(const Node& node) const override;
		Node Read(const std::string& contents) const override;
	};
}

#endif //!XEPHMARK_YAMLFORMATTER_H
