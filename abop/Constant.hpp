#pragma once
#include <map>
#include <string>

// Constant
static std::map<std::string, std::string> CONSTANT;

void AddConstant(std::string&, std::string&);
std::string GetConstant(std::string&);
std::vector<std::pair<std::string, std::string>> GetConstants();
void UpdateConstant(std::string&, float&);
void UpdateConstant(std::string&, std::string&);
void DeleteConstant(std::string&);
void ReplaceConstant(std::string&);
bool IsConstant(std::string&);