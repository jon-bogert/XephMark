#ifndef XEPHMARK_JSONFORMATTER_H
#define XEPHMARK_JSONFORMATTER_H

#include <XephMark/IOFormatter.h>

namespace XephMark
{
	class JSONFormatter : public IOFormatterText
	{
	public:
		JSONFormatter() = default;
		JSONFormatter(const bool usePrettyFormat) : m_usePrettyFormat(usePrettyFormat) {}

		bool SaveToFile(const Node& node, const std::filesystem::path& path) const override;
		Node LoadFromFile(const std::filesystem::path& path) const override;

		std::string Dump(const Node& node) const override;
		Node Read(const std::string& contents) const override;

		bool GetUsePrettyFormat() const { return m_usePrettyFormat; }
		void SetUsePrettyFormat(const bool usePrettyFormat) { m_usePrettyFormat = usePrettyFormat; }

	private:
		bool m_usePrettyFormat = false;
	};
}

#endif // !XEPHMARK_JSONFORMATTER_H