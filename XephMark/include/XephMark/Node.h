#ifndef XEPHMARK_NODE_H
#define XEPHMARK_NODE_H

#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <variant>
#include <vector>
#include <type_traits>

namespace XephMark
{
	class Node final
	{
		enum NodeType : uint8_t
		{
			NodeType_Null = 0x00,
			NodeType_Boolean = 0x01,
			NodeType_Integral = 0x02,
			NodeType_Floating = 0x04,
			NodeType_Unsigned = 0x08,

			NodeType_Scalar = 0x10,
			NodeType_List = 0x20,
			NodeType_Map = 0x40,

			NodeType_Numeric = NodeType_Integral | NodeType_Floating,
		};

		using Data = std::variant<
			std::monostate,
			std::vector<Node>,
			std::string,
			bool,
			uint64_t,
			int64_t,
			double>;
	public:
		Node() = default;
		~Node()
		{
			Clear();
		}

		Node& operator=(const Node& other)
		{
			Clear();
			m_data = other.m_data;
			m_type = other.m_type;
			m_key = other.m_key;
			return *this;
		}

		template <typename T>
		typename std::enable_if<std::is_same<bool, T>::value, Node&>::type
		operator=(const T& other)
		{
			Clear();

			m_type |= NodeType_Scalar;
			m_type |= NodeType_Boolean;
			m_data = static_cast<bool>(other);
			return *this;
		}

		template <typename T>
		typename std::enable_if<std::is_base_of_v<std::string, T>, Node&>::type
		operator=(const T& other)
		{
			Clear();
			m_type |= NodeType_Scalar;
			m_data = static_cast<std::string>(other);
			return *this;
		}

		template <typename T>
		typename std::enable_if<(std::is_integral_v<T> && std::is_unsigned_v<T>)
			&& !std::is_same<bool, T>::value, Node&>::type
			operator=(const T& other)
		{
			Clear();
			m_type |= NodeType_Scalar;
			m_type |= NodeType_Integral;
			m_type |= NodeType_Unsigned;

			m_data = static_cast<uint64_t>(other);
			return *this;
		}

		template <typename T>
		typename std::enable_if<(std::is_integral_v<T> && !std::is_unsigned_v<T>)
			&& !std::is_same<bool, T>::value, Node&>::type
			operator=(const T& other)
		{
			Clear();
			m_type |= NodeType_Scalar;
			m_type |= NodeType_Integral;
			if (other >= 0)
				m_type |= NodeType_Unsigned;

			m_data = static_cast<int64_t>(other);
			return *this;
		}

		template <typename T>
		typename std::enable_if<std::is_floating_point_v<T>, Node&>::type
			operator=(const T& other)
		{
			Clear();
			m_type |= NodeType_Scalar;
			m_type |= NodeType_Floating;
			m_data = static_cast<double>(other);
			return *this;
		}
		
		//Move Assignment
		Node& operator=(Node&& other) noexcept
		{
			if (&other == this)
				return *this;

			m_data = std::move(other.m_data);
			m_type = std::move(other.m_type);
			m_key = std::move(other.m_key);

			return *this;
		}

		Node(const Node& other)
		{
			if (&other == this)
				return;

			m_data = other.m_data;
			m_type = other.m_type;
			m_key = other.m_key;
		}

		Node(Node&& other) noexcept
			: m_key(std::move(other.m_key))
			, m_data(std::move(other.m_data))
			, m_type(std::move(other.m_type)) {}

		class iterator
		{
			std::vector<Node>::iterator iter;

		public:
			iterator(std::vector<Node>::iterator iter) : iter(iter) {}

			Node& operator*()
			{
				return *iter;
			}

			iterator& operator++()
			{
				++iter;
				return *this;
			}

			bool operator!=(const iterator& other) const
			{
				return iter != other.iter;
			}

			bool operator==(const iterator& other) const
			{
				return iter == other.iter;
			}
		};

		class const_iterator
		{
			std::vector<Node>::const_iterator iter;

		public:
			const_iterator(std::vector<Node>::const_iterator iter) : iter(iter) {}

			const Node& operator*() const
			{
				return *iter;
			}

			const_iterator& operator++()
			{
				++iter;
				return *this;
			}

			bool operator!=(const const_iterator& other) const
			{
				return iter != other.iter;
			}

			bool operator==(const const_iterator& other) const
			{
				return iter == other.iter;
			}
		};

		iterator begin()
		{
			if (IsValue())
				throw std::exception("node is not of collection type");

			return iterator(std::get<std::vector<Node>>(m_data).begin());
		}

		iterator end()
		{
			if (IsValue())
				throw std::exception("node is not of collection type");

			return iterator(std::get<std::vector<Node>>(m_data).end());
		}

		const_iterator begin() const
		{
			if (IsValue())
				throw std::exception("node is not of collection type");

			return const_iterator(std::get<std::vector<Node>>(m_data).begin());
		}

		const_iterator end() const
		{
			if (IsValue())
				throw std::exception("node is not of collection type");
			
			return const_iterator(std::get<std::vector<Node>>(m_data).end());
		}

		void Clear()
		{
			m_data = std::monostate();
			m_type = NodeType_Null;
		}

		bool IsDefined() const
		{
			return !(m_type & NodeType_Null);
		}

		bool IsArray() const
		{
			return (m_type & NodeType_List);
		}

		bool IsObject() const
		{
			return (m_type & NodeType_Map);
		}

		bool IsValue() const
		{
			return (m_type & NodeType_Scalar);
		}

		bool HasKey() const
		{
			return !m_key.empty();
		}

		std::string Key() const
		{
			return m_key;
		}

		bool IsNumeric() const
		{
			return m_type & NodeType_Numeric;
		}

		bool IsDecimal() const
		{
			return m_type & NodeType_Floating;
		}

		bool IsIntegral() const
		{
			return m_type & NodeType_Integral;
		}

		bool IsUnsigned() const
		{
			return m_type & NodeType_Unsigned;
		}

		bool IsBool() const
		{
			return m_type & NodeType_Boolean;
		}

		//As bool
		template <typename T>
		typename std::enable_if<std::is_same<bool, T>::value, T>::type
		As() const
		{
			if (!IsValue())
				throw std::exception("Node is not of value type");

			if (!IsBool())
				throw std::exception("Node is not of bool type");
			return std::get<bool>(m_data);
		}

		//As Unsigned Integral
		template <typename T>
		typename std::enable_if<(std::is_integral_v<T> && std::is_unsigned_v<T>) // std::is_floating_point_v<T>
			&& !std::is_same<bool, T>::value, T>::type
		As() const
		{
			if (!IsValue())
				throw std::exception("Node is not of value type");
			if (!IsNumeric)
				throw std::exception("Node is not have Numeric value");
			if (IsUnsigned())
				throw std::exception("Node contains negative value");
			if (IsDecimal())
				throw std::exception("Node contains decimal value");

			return static_cast<T>(std::get<uint64_t>(m_data));
		}

		//As Unsigned Integral
		template <typename T>
		typename std::enable_if<(std::is_integral_v<T> && !std::is_unsigned_v<T>)
			&& !std::is_same<bool, T>::value, T>::type
			As() const
		{
			if (!IsValue())
				throw std::exception("Node is not of value type");
			if (!IsNumeric)
				throw std::exception("Node is not have Numeric value");
			if (IsDecimal())
				throw std::exception("Node contains decimal value");

			return static_cast<T>(std::get<int64_t>(m_data));
		}

		//As Unsigned Integral
		template <typename T>
		typename std::enable_if<std::is_floating_point_v<T>, T>::type
			As() const
		{
			if (!IsValue())
				throw std::exception("Node is not of value type");
			if (!IsNumeric)
				throw std::exception("Node is not have Numeric value");

			if (IsDecimal())
				return static_cast<T>(std::get<double>(m_data));
			if (IsUnsigned())
				return static_cast<T>(std::get<uint64_t>(m_data));

			return static_cast<T>(std::get<int64_t>(m_data));
		}

		//As String
		template <typename T>
		typename std::enable_if<std::is_base_of_v<std::string, T>, T>::type
		As() const
		{
			if (!IsValue())
				throw std::exception("Node is not of value type");
			if (IsBool())
			{
				return std::get<bool>(m_data) ? "true" : "false";
			}
			if (IsIntegral())
			{
				if (IsUnsigned())
				{
					return std::to_string(std::get<uint64_t>(m_data));
				}
				return std::to_string(std::get<int64_t>(m_data));
			}
			if (IsDecimal())
			{
				return std::to_string(std::get<double>(m_data));
			}

			return static_cast<T>(std::get<std::string>(m_data));
		}

		void PushBack(const Node& node)
		{
			if (IsValue() && IsObject())
				throw std::exception("Node is not of type Array");

			if (!IsArray())
			{
				Clear();
				m_data = std::vector<Node>();
				m_type |= NodeType_List;
			}

			std::get<std::vector<Node>>(m_data).push_back(node);
		}

		//Trims all nodes that are not defined
		void Trim()
		{
			if (!IsDefined() || IsValue())
				return;

			std::vector<Node>& data = std::get<std::vector<Node>>(m_data);

			for (auto iter = data.begin(); iter != data.end();)
			{
				if (iter->IsDefined())
				{
					iter->Trim();
					++iter;
				}
				else
				{
					iter = data.erase(iter);
				}
			}
		}

		size_t Size() const
		{
			if (IsValue())
				return 0;

			return std::get<std::vector<Node>>(m_data).size();
		}

		Node& operator[](const std::string& key)
		{
			if (!IsDefined())
			{
				Clear();
				m_data = std::vector<Node>();
				m_type |= NodeType_Map;
			}
			if (!IsObject())
				throw std::exception("node is not of object type");

			Node* node = FindKey(key);
			if (node == nullptr)
			{
				node = &(std::get<std::vector<Node>>(m_data)).emplace_back();
				node->m_key = key;
			}
			return *node;
		}

		Node& operator[](const size_t index)
		{
			if (!IsObject() && !IsArray())
				throw std::exception("node is not an indxed type");
			if (index >= Size())
				throw std::exception("index out of range");

			return (std::get<std::vector<Node>>(m_data))[index];
		}

		const Node& operator[](const size_t index) const
		{
			if (!IsObject() && !IsArray())
				throw std::exception("node is not an indxed type");
			if (index >= Size())
				throw std::exception("index out of range");

			return std::get<std::vector<Node>>(m_data).at(index);
		}

		const Node& At(const std::string key) const
		{
			if (!IsObject())
				throw std::exception("node is not of object type");

			const Node* node = FindKey(key);
			if (node == nullptr)
				throw std::exception("key not found");

			return *node;
		}

		const Node& At(const size_t index) const
		{
			return operator[](index);
		}

	private:
		Node* FindKey(const std::string& key)
		{
			std::vector<Node>& data = std::get<std::vector<Node>>(m_data);

			auto iter = std::find_if(data.begin(), data.end(),
				[&](const Node& n) { return n.m_key == key; });

			if (iter == data.end())
				return nullptr;
			return &(*iter);
		}

		const Node* FindKey(const std::string& key) const
		{
			const std::vector<Node>& data = std::get<std::vector<Node>>(m_data);

			auto iter = std::find_if(data.begin(), data.end(),
				[&](const Node& n) { return n.m_key == key; });

			if (iter == data.end())
				return nullptr;
			return &(*iter);
		}

		std::string m_key = "";

		Data m_data;

		uint8_t m_type = NodeType_Null;
	};
}

#endif //!XEPHMARK_NODE_H