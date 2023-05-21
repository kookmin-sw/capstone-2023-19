#include <vector>
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

std::vector<std::pair<std::string, std::string>> GetConstants()
{
	std::vector<std::pair<std::string, std::string>> ret;
	for (auto it = CONSTANT.begin(); it != CONSTANT.end(); it++)
		ret.push_back({ it->first, it->second });

	return ret;
}

void UpdateConstant(std::string& key, float& value)
{
	std::string strValue = std::to_string(value);

	if (CONSTANT[key] != strValue)
	{
		CONSTANT[key] = strValue;
	}
}

void UpdateConstant(std::string& key, std::string& value)
{
	if (CONSTANT[key] != value)
	{
		CONSTANT[key] = value;
	}
}

void ResetConstant()
{
	CONSTANT = std::map<std::string, std::string>();
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