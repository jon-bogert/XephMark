#include <XephMark/JSONFormatter.h>
#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>
#include <limits>
#include <string>
#include <vector>

using namespace XephMark;
using json = nlohmann::json;

static void Import(const json& in, Node& out)
{
    if (in.is_object())
    {
        for (auto it = in.begin(); it != in.end(); ++it)
        {
            Import(it.value(), out[it.key()]);
        }
        return;
    }

    if (in.is_array())
    {
        for (const auto& child : in)
        {
            Node result;
            Import(child, result);
            out.PushBack(result);
        }
        return;
    }

    if (in.is_boolean())
    {
        out = in.get<bool>();
        return;
    }

    if (in.is_number())
    {
        if (in.is_number_float())
        {
            if (in.is_number_float())
            {
                out = in.get<float>();
                return;
            }
            out = in.get<double>();
            return;
        }

        if (!in.is_number_unsigned())
        {
            if (in.is_number_integer() && in.get<int64_t>() >= std::numeric_limits<int32_t>::min() &&
                in.get<int64_t>() <= std::numeric_limits<int32_t>::max())
            {
                out = in.get<int32_t>();
                return;
            }
            out = in.get<int64_t>();
            return;
        }

        if (in.is_number_unsigned() && in.get<uint64_t>() <= std::numeric_limits<uint32_t>::max())
        {
            out = in.get<uint32_t>();
            return;
        }
        out = in.get<uint64_t>();
        return;
    }

    out = in.get<std::string>();
}

static void Export(json& out, const Node& in)
{
    if (in.IsObject())
    {
        for (const Node& child : in)
        {
            json result;
            Export(result, child);
            out[child.Key()] = result;
        }
        return;
    }

    if (in.IsArray())
    {
        for (const Node& child : in)
        {
            json result;
            Export(result, child);
            out.push_back(result);
        }
        return;
    }

    if (in.IsBool())
    {
        out = in.As<bool>();
        return;
    }

    if (in.IsNumeric())
    {
        if (in.IsDecimal())
        {
            out = in.As<double>();
            return;
        }

        if (!in.IsUnsigned())
        {
            out = in.As<int64_t>();
            return;
        }

        out = in.As<uint64_t>();
        return;
    }

    out = in.As<std::string>();
}

Node XephMark::JSONFormatter::LoadFromFile(const std::filesystem::path& path) const
{
    std::ifstream file(path, std::ios::ate);
    size_t length = file.tellg();

    if (length == 0)
        return Node();

    file.seekg(0, std::ios::beg);
    std::vector<char> content(length + 1);
    file.read(content.data(), length);
    content.back() = '\0';
    return Read(content.data());
}

Node XephMark::JSONFormatter::Read(const std::string& content) const
{
    Node result;
    json in = json::parse(content);
    Import(in, result);
    return result;
}

bool XephMark::JSONFormatter::SaveToFile(const Node& node, const std::filesystem::path& path) const
{
    std::string content = Dump(node);
    std::ofstream file(path);
    if (!file.is_open())
        return false;

    file << content;
    return true;
}

std::string XephMark::JSONFormatter::Dump(const Node& node) const
{
    json out;
    Export(out, node);
    return out.dump((m_usePrettyFormat) ? 4 : -1);
}
