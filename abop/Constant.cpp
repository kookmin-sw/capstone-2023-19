#include "Constant.hpp"

void AddConstant(std::string& key, std::string& value)
{
	CONSTANT[key] = value;
}

std::string GetConstant(std::string& key)
{
	auto found = CONSTANT.find(key);
	if (found != CONSTANT.end())
		return found->second;
	return "";
}

void DeleteConstant(std::string& key)
{
	auto found = CONSTANT.find(key);
	if (found != CONSTANT.end())
		CONSTANT.erase(found);
}

void ReplaceConstant(std::string& str)
{
	for (auto& [key, value] : CONSTANT)
	{
		if (value[0] == '-')
			value[0] = '~';

		std::string::size_type pos = 0;

		while ((pos = str.find(key)) != std::string::npos)
		{
			str.replace(pos, key.size(), value);
		}
	}
}

bool IsConstant(std::string& key)
{
	auto found = CONSTANT.find(key);
	if (found != CONSTANT.end())
		return true;
	return false;
}