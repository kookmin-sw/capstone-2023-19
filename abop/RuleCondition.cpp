#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "Utils.hpp"
#include "RuleCondition.hpp"

// 단일식 string을 Condition struct로 변환 (Condition struct는 LRule.hpp 선언)
RuleCondition::Condition ConvertStringToCondition(std::string str)
{
    RuleCondition::Condition cond;
    cond.target = "";

    std::cout << str.find('=') << std::endl;
    std::cout << (str.find('=') > -1) << std::endl;

    if (str.find(">=") != std::string::npos)
    {
        cond.sign = RuleCondition::Sign::MoreSame;
        cond.target = str.substr(0, str.find(">="));
        cond.compare = StringToInt(str.substr(str.find(">=") + 2, str.size() - str.find(">=")));
    }
    else if (str.find("<=") != std::string::npos)
    {
        cond.sign = RuleCondition::Sign::LessSame;
        cond.target = str.substr(0, str.find("<="));
        cond.compare = StringToInt(str.substr(str.find("<=") + 2, str.size() - str.find("<=")));
    }
    else if (str.find(">") != std::string::npos)
    {
        cond.sign = RuleCondition::Sign::More;
        cond.target = str.substr(0, str.find(">"));
        cond.compare = StringToInt(str.substr(str.find(">") + 1, str.size() - str.find(">")));
    }
    else if (str.find("<") != std::string::npos)
    {
        cond.sign = RuleCondition::Sign::Less;
        cond.target = str.substr(0, str.find("<"));
        cond.compare = StringToInt(str.substr(str.find("<") + 1, str.size() - str.find("<")));
    }
    else if (str.find('=') != std::string::npos)
    {
        cond.sign = RuleCondition::Sign::Same;
        cond.target = str.substr(0, str.find("="));
        cond.compare = StringToInt(str.substr(str.find("=") + 1, str.size() - str.find("=")));
    }

    // error
    return cond;
}

RuleCondition::RuleCondition()
{
    mEmpty = true;
}

RuleCondition::~RuleCondition()
{

}

RuleCondition::RuleCondition(std::string str)
{
    mConds = std::vector<Condition>();

    if (str.empty())
    {
        mEmpty = true;
        return;
    }

    mEmpty = false;
    // 계산 + - * /

    // &, | + 관계식만 고려
    // 논리 기호 !, &, |
    // 관계식 < > = >= <=

    // 공백 제거
    RemoveAll(str, ' ');
    mCondsString = str;

    while (str.find("&") != std::string::npos || str.find("|") != std::string::npos)
    {
        std::string pre = str.substr(0, str.find("&"));

        while (pre.find("|") != std::string::npos)
        {
            Condition cond = ConvertStringToCondition(pre.substr(0, pre.find("|")));
            if (cond.target.empty())
            {
                // error
                std::cout << "error" << std::endl;
            }
            else
            {
                mConds.push_back(cond);
            }

            pre = pre.substr(pre.find("|") + 1, pre.size() - pre.find("|"));
        }

        Condition cond = ConvertStringToCondition(pre);
        if (cond.target.empty())
        {
            // error
            std::cout << "error" << std::endl;
        }
        else
        {
            mConds.push_back(cond);
        }

        str = str.substr(str.find("&") + 1, str.size() - str.find("&"));
    }

    Condition cond = ConvertStringToCondition(str);
    if (cond.target.empty())
    {
        // error
        std::cout << "error" << std::endl;
    }
    else
    {
        mConds.push_back(cond);
    }
}

std::string RuleCondition::GetConditionString() const
{
    return mCondsString;
}

bool RuleCondition::IsEmpty() const
{
    return mEmpty;
}

bool RuleCondition::CheckCondition(std::map<std::string, std::string> valueParams) const
{
    float value;
    // Condition의 target과 valueParams key는 매핑 되어야 함
    // !!! &, | 에 대한 처리가 안되어 있음 (기본 &)
    for (const Condition& c : mConds)
    {
        value = std::stoi(valueParams[c.target]);
        
        switch (c.sign)
        {
            case Sign::Same:
            {
                if (!(value == c.compare))
                {
                    return false;
                }
                break;
            }
            case Sign::More:
            {
                if (!(value > c.compare))
                {
                    return false;
                }
                break;
            }
            case Sign::Less:
            {
                if (!(value < c.compare))
                {
                    return false;
                }
                break;
            }
            case Sign::MoreSame:
            {
                if (!(value >= c.compare))
                {
                    return false;
                }
                break;
            }
            case Sign::LessSame:
            {
                if (!(value <= c.compare))
                {
                    return false;
                }
                break;
            }
        }
    }

    return true;
}