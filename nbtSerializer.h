#pragma once
#include "include/filesystem/iserializer.h"
#include "nbtCompound.h"
#include "include/math/direction.h"
#include "include/math/uuid.h"
#include "include/math/graphics/color/color.h"
#include "include/math/rectangletn.h"
#include "nbtValue.h"
#include "nbtDataArray.h"
#include <concepts>
#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>
#include "constants.h"
#include "GlobalFunctions.h"
#include "math/axis.h"
#include "math/vectn.h"
#include "optimization/debug.h"
#include "nbtData.h"
#include "nbtDataTag.h"


template<typename t>
concept nbtType = std::signed_integral<t> || std::is_same_v<t, float> || std::is_same_v<t, double> || std::is_same_v<t, int8_t*> || std::is_same_v<t, int32_t*> || std::is_same_v<t, int64_t*> || std::is_same_v<t, std::wstring>;

template<nbtType t>
constexpr nbtDataTag getNBTDataTag()
{
	if constexpr (std::is_same<t, int8_t>())
	{
		return nbtDataTag::tagSignedInt8;
	}
	else if constexpr (std::is_same<t, int16_t>())
	{
		return nbtDataTag::tagSignedInt16;
	}
	else if constexpr (std::is_same<t, int32_t>())
	{
		return nbtDataTag::tagSignedInt32;
	}
	else if constexpr (std::is_same<t, int64_t>())
	{
		return nbtDataTag::tagSignedInt64;
	}
	else if constexpr (std::is_same<t, float>())
	{
		return nbtDataTag::tagFP32;
	}
	else if constexpr (std::is_same<t, double>())
	{
		return nbtDataTag::tagFP64;
	}
	else if constexpr (std::is_same<t, int8_t*>())
	{
		return nbtDataTag::tagSignedInt8Array;
	}
	else if constexpr (std::is_same<t, int32_t*>())
	{
		return nbtDataTag::tagSignedInt32Array;
	}
	else if constexpr (std::is_same<t, int64_t*>())
	{
		return nbtDataTag::tagSignedInt64Array;
	}
	else if constexpr (std::is_same<t, std::wstring>())
	{
		return nbtDataTag::tagUTF8String;
	}
	else
	{
		//throw "this tag is not allowed";//TODO: make static assert again
		//static_assert(false, "this tag is not allowed");
		return nbtDataTag::tagEnd;
	}
}

struct idConverter;
struct nbtSerializer :iSerializer
{
	nbtCompound* compound = nullptr;
	idConverter* converter = nullptr;
	std::vector<nbtCompound*> recursiveChildCompounds = std::vector<nbtCompound*>();
	std::vector<int> nextListIndexes = std::vector<int>();
	nbtCompound* getCurrentChildCompound() const;

	nbtSerializer(nbtCompound* compound, cbool& write, cbool& convert = true);

	//IMPORTANT: DO NOT READ AFTER A push<nbtDataTag::tagCompound>() WHICH RETURNED FALSE
	template<nbtDataTag tagType = nbtDataTag::tagCompound>
	bool push(const std::wstring& name = std::wstring(L""))
	{
		nbtCompound* childCompound;
		childCompound = (nbtCompound*)getNBTData<tagType>(name);
		if (childCompound == nullptr)
		{
			return false;
		}
		else
		{
			recursiveChildCompounds.push_back(childCompound);
			nextListIndexes.push_back(0);
			return true;
		}
		return childCompound != nullptr;
	}

	bool push(const nbtData* child);
	void pop();

	const std::vector<nbtData*>& getChildren() const;

	static bool isArrayTag(const nbtDataTag& dataTag);



	template<typename t, typename = std::enable_if_t<getNBTDataTag<t>() != nbtDataTag::tagEnd>>
	inline bool serializeValue(nbtData* data, t& value)
	{
		static_assert(std::is_same<t, std::wstring>::value || std::is_arithmetic<t>::value, "can't convert to this type");

		if (write)
		{
			((nbtValue<t>*)data)->data = value;
		}
		else
		{
			const nbtDataTag& tag = data->dataTag;
			switch (tag)
			{
			case nbtDataTag::tagUTF8String:
				value = convertAnyType<std::wstring, t>(((nbtValue<std::wstring>*)data)->data);
				break;
			case nbtDataTag::tagSignedInt8:
				value = convertAnyType<int8_t, t>(((nbtValue<int8_t>*)data)->data);
				break;
			case nbtDataTag::tagSignedInt16:
				value = convertAnyType<int16_t, t>(((nbtValue<int16_t>*)data)->data);
				break;
			case nbtDataTag::tagSignedInt32:
				value = convertAnyType<int32_t, t>(((nbtValue<int32_t>*)data)->data);
				break;
			case nbtDataTag::tagSignedInt64:
				value = convertAnyType<int64_t, t>(((nbtValue<int64_t>*)data)->data);
				break;
			case nbtDataTag::tagFP32:
				value = convertAnyType<float, t>(((nbtValue<float>*)data)->data);
				break;
			case nbtDataTag::tagFP64:
				value = convertAnyType<double, t>(((nbtValue<double>*)data)->data);
				break;
			default:
				return false;
			}
		}
		return true;
	}

	template<typename t>
	inline bool serializeVariableArray(nbtData* data, t*& value, int& count)
	{
		if (write)
		{
			((nbtDataArray<t>*)data)->data = new t[count];
			((nbtDataArray<t>*)data)->arraySize = count;
			std::copy(value, value + count, ((nbtDataArray<t>*)data)->data);
		}
		else
		{
			if (!value)
			{
				value = new t[count];
			}

			const nbtDataTag tag = data->dataTag;
			switch (tag)
			{
			case nbtDataTag::tagSignedInt8Array:
			{
				int8_t*& ptr = ((nbtDataArray<int8_t>*)data)->data;
				count = (int)((nbtDataArray<int8_t>*)data)->arraySize;
				std::copy(ptr, ptr + count, (int8_t*)value);
			}
			break;
			case nbtDataTag::tagSignedInt32Array:
			{
				int32_t*& ptr = ((nbtDataArray<int32_t>*)data)->data;
				count = (int)((nbtDataArray<int32_t>*)data)->arraySize;
				std::copy(ptr, ptr + count, (int32_t*)value);
			}
			break;
			case nbtDataTag::tagSignedInt64Array:
			{
				int64_t*& ptr = ((nbtDataArray<int64_t>*)data)->data;
				count = (int)((nbtDataArray<int64_t>*)data)->arraySize;
				std::copy(ptr, ptr + count, (int64_t*)value);
			}
			break;
			default:
				return false;
			}
		}
		return true;
	}

	template<typename t>
	inline static bool serializeArray(nbtData* data, t* value, int count = 1)
	{
		return serializeVariableArray<t>(write, data, value, count);
	}
	template<nbtDataTag valueDataTag>
	inline nbtData* getNBTData(const std::wstring& memberName)
	{
		//TODO: get asserts back
		//static_assert(valueDataTag != nbtDataTag::tagEnd, "this type is not allowed");

		nbtCompound* currentChildCompound = getCurrentChildCompound();
		nbtData* currentChild;

		if (currentChildCompound->dataTag == nbtDataTag::tagCompound)
		{
			if (memberName == std::wstring(L""))
			{
				handleError(std::wstring(L"compound tags have names"));
			}
		}
		else
		{
			if (memberName.length())
			{
				handleError(std::wstring(L"list tags don't have names"));
			}
		}

		if (write)
		{
			if (currentChildCompound->dataTag == nbtDataTag::tagCompound)
			{
				//check for multiple children with the same name
				auto it = std::find_if(currentChildCompound->children.begin(), currentChildCompound->children.end(), [memberName](const auto& a) {return a->name == memberName; });
				if (it != currentChildCompound->children.end())
				{
					handleError(std::wstring(L"multiple children with the same name"));
				}
			}
			if (currentChildCompound->dataTag == nbtDataTag::tagList)
			{
				if (currentChildCompound->children.size() && currentChildCompound->children[0]->dataTag != valueDataTag)
				{
					handleError(std::wstring(L"no different tags in a list; maybe you forgot to add a pop()"));
				}
			}
			currentChild = currentChildCompound->createNBTData(memberName, valueDataTag);
			currentChildCompound->children.push_back(currentChild);
			return currentChild;
		}
		else
		{
			if (currentChildCompound->dataTag == nbtDataTag::tagList)
			{
				csize_t& currentCompoundIndex = recursiveChildCompounds.size() - 1;

				//not a reference, make a copy, because the original will be incremented
				csize_t& listIndex = nextListIndexes[currentCompoundIndex];

				if (listIndex < currentChildCompound->children.size())
				{
					nextListIndexes[currentCompoundIndex]++;
					return currentChildCompound->children[listIndex];
				}
				else
				{
					return nullptr;
				}
			}
			else
			{
				if constexpr (isDebugging)
				{
					if (memberName == std::wstring(L""))
					{
						handleError(std::wstring(L"compound tags have names"));
					}
				}
				auto it = std::find_if(currentChildCompound->children.begin(), currentChildCompound->children.end(), [memberName](const auto& a) {return a->name == memberName; });
				if (it == currentChildCompound->children.end())// || (*it)->dataTag != valueDataTag)
				{
					return nullptr;
				}
				else
				{
					return *it;
				}
			}
		}
	}
	
	template<nbtType t>
	inline nbtData* getNBTData(const std::wstring& memberName)
	{
		return getNBTData<getNBTDataTag<t>()>(memberName);
	}

	template<nbtType t>
	inline bool serializeVariableArray(const std::wstring& memberName, t*& value, int& count)
	{
		nbtData* currentChild = getNBTData<t*>(memberName);
		if (currentChild)
		{
			serializeVariableArray(currentChild, value, count);
			return true;
		}
		else
		{
			return false;
		}
	}
	template<nbtType t>
	inline bool serializeValue(const std::wstring& memberName, t& value)
	{
		nbtData* currentChild = getNBTData<t>(memberName);
		if (currentChild)
		{
			serializeValue(currentChild, value);
			return true;
		}
		else
		{
			return false;
		}
	}

	template<typename t>
	inline bool serializeList(const std::wstring& memberName, std::vector<t>& value)
	{
		if (push<nbtDataTag::tagList>(memberName))
		{
			if (!write)
			{
				value = std::vector<t>(getChildren().size());
			}
			for (t& it : value)
			{
				serializeValue(std::wstring(L""), it);
			}
			pop();
			return true;
		}
		return false;
	}

	template<typename t>
	inline bool serializeListOfSerializables(const std::wstring& memberName, std::vector<t>& value)
	{
		if (push<nbtDataTag::tagList>(memberName))
		{
			if (!write)
			{
				value = std::vector<t>(getChildren().size());
			}
			size_t i = 0;
			for (t& it : value)
			{
				if (push()) {
					it.serializeValue(*this);
					pop();
				}
			}
			pop();
			return true;
		}
		return false;
	}

	template<typename t>
	inline bool serializeArray(const std::wstring& memberName, t* value, int count)
	{
		return serializeVariableArray(memberName, value, count);
	}
	//inline bool serializeValue(const std::wstring& memberName, byte& value)
	//{
	//	return serializeValue(memberName, (sbyte&)value);
	//}
	inline bool serializeValue(const std::wstring& memberName, bool& value)
	{
		return serializeValue(memberName, (sbyte&)value);
	}
	template<typename t, typename = std::enable_if_t<std::is_unsigned_v<t>>>
	inline bool serializeValue(const std::wstring& memberName, t& value)
	{
		return serializeValue(memberName, (std::make_signed_t<t>&)value);
	}
	//inline bool serializeValue(const std::wstring& memberName, ushort& value)
	//{
	//	return serializeValue(memberName, (short&)value);
	//}
	//inline bool serializeValue(const std::wstring& memberName, uint& value)
	//{
	//	return serializeValue(memberName, (uint&)value);
	//}
	inline bool serializeValue(const std::wstring& memberName, directionID& direction)
	{
		return serializeValue(memberName, (sbyte&)direction);
	}
	inline bool serializeValue(const std::wstring& memberName, uuid& id)
	{
		if (push<nbtDataTag::tagCompound>(memberName))
		{
			serializeValue(std::wstring(L"a"), (int&)id.idInts[0]);
			serializeValue(std::wstring(L"b"), (int&)id.idInts[1]);
			serializeValue(std::wstring(L"c"), (int&)id.idInts[2]);
			serializeValue(std::wstring(L"d"), (int&)id.idInts[3]);
			pop();
			return true;
		}
		else
		{
			return false;
		}
	}

	template<typename t, size_t axisCount>
	inline bool serializeValue(const std::wstring& memberName, vectn<t, axisCount>& vector)
	{
		if (push<nbtDataTag::tagCompound>(memberName))
		{
			for (size_t i = 0; i < axisCount; i++)
			{
				serializeValue(axisNames[i], vector[i]);
			}

			pop();
			return true;
		}
		else
		{
			return false;
		}
	}


	template<typename t, size_t axisCount>
	inline bool serializeValue(const std::wstring& memberName, colortn<t, axisCount>& vector)
	{
		return serializeValue(memberName, (vectn<t, axisCount>&)vector);
	}

	template<typename t>
	inline bool serializeValue(const std::wstring& memberName, rectanglet2<t>& box)
	{
		if (push<nbtDataTag::tagCompound>(memberName))
		{
			serializeValue(std::wstring(L"position"), box.pos0);
			serializeValue(std::wstring(L"size"), box.size);
			pop();
			return true;
		}
		else
		{
			return false;
		}
	}
};