# XephMark - Readme

## Description

A C++ library for loading and saving object-oriented data to and from various markup languages. Data is stored in native format rather than strings for more optimized reading and writing in binary formats. Supports YAML, JSON, and BSON formats. Multi-format reading and saving may be handy if a debug version of an app require a text-based save format while the released product uses a binary format.

The `Node` type supports values, maps, and lists.

## Language Requirements

C++ 17 or higher is required (for std::filesystem).

## Getting Started

This project uses the [premake5](https://premake.github.io/) build system. For Visual Studio 2022, just run the `generate-vs2022.bat` file in the project's root directory.

### Other Compilers
Other configurations must run commands in your terminal to generate the appropriate files.
- Visual Studio 2019: `./premake5 vs2019`
- Makefile: `./premake5 gmake2`

NOTE: Linux and macOS versions are not currently configured.
More information on how to configure the build system can be found in the [premake5 documentation](https://premake.github.io/docs/).

## Writing

```cpp
#include <XephMark/XephMark.h> // Include base libary
#include <XephMark/YAMLFormatter.h> // Include desired formatter

#include <string>

int main()
{
    // Create a root node;
    XephMark::Node root;

    // Save values similar to std::map or std::unordered_map. New nodes will automatically be added and types will be deduced.
    root["version"] = 1;
    root["title"] = "My Example";

    // Store as map
    root["user"]["name"] = "John Smith";
    root["user"]["age"] = 27;
    root["user"]["has-membership"] = true;

    // Store List
    root["my-numbers"].PushBack(10);
    root["my-numbers"].PushBack(11.32f);
    root["my-numbers"].PushBack(-1000);

    // Initialize Formatter and call Dump or SaveToFile;
    XephMark::YAMLFormatter yaml;
    std::string contents = yaml.Dump(root);
    // or
    yaml.SaveToFile(root, "my-file.yaml");

    return 0;
}
```

## Reading

```cpp
#include <XephMark/XephMark.h> // Include base libary
#include <XephMark/YAMLFormatter.h> // Include desired formatter

// STL headers for example
#include <cstdint>
#include <string>
#include <vector>

struct User // struct for demo
{
    std::string name;
    uint32_t age;
    bool hasMembership;
};

int main()
{
    // Create a root node and formatter
    XephMark::Node root;
    XephMark::YAMLFormatter yaml;

    // Load from Data
    std::string content = GetContentsFromSomewhere();
    root = yaml.Read(content);

    // Load from file
    root = yaml.LoadFromFile("my-file.yaml");

    // Get values
    if (root["version"].IsDefined() && root["version"].IsIntegral()) // Check if a node exists and/or has required properties
    {
        int version = root["version"].As<int>(); // Get value as type;
    }

    std::string title = root["title"].As<std::string>();

    // Get Map values
    User user;
    if (root["user"].IsDefined() && root["user"].IsObject())
    {
        user.name = root["user"]["name"].As<std::string>();
        user.age = root["user"]["age"].As<uint32_t>();
        user.hasMembership = ["user"]["has-membership"].As<bool>();
    }

    // Get list values by index
    std::vector<float> myNumbers;
    myNumbers.resize(root["my-numbers"].Size());
    for (size_t i = 0; i < root["my-numbers"].Size(); ++i)
    {
        myNumbers[i] = root["my-numbers"][i].As<float>();
    }
    myNumbers.clear();
    // Get list values by iterator
    for (const XephMark::Node& num : root["my-numbers"])
    {
        myNumbers.push_back(num.As<float>());
    }
    // or
    myNumbers.clear();
    for (const auto iter = root["my-numbers"].begin(); iter != root["my-numbers"].end(); ++iter)
    {
        myNumbers.push_back(iter->As<float>());
    }

    return 0;
}
```
## External Libraries

 - YAML formatting requires [yaml-cpp](https://github.com/jbeder/yaml-cpp). Source code included in this repository.
 - JSON & BSON formatting requires [nlohmann-json](https://github.com/nlohmann/json). Source code included in this repository.
