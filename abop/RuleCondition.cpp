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

    //std::cout << str.find('=') << std::endl;
    //std::cout << (str.find('=') > -1) << std::endl;

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
    for (Condition c : mConds)
    {
        // target가 계산식인 경우 따로 핸들링 - ex) x+y+z
        bool needsParsing = false;
        for (int i = 0; i < c.target.length(); i++)
        {
            if (!isalpha(c.target[i]))
            {
                needsParsing = true;
                break;
            }
        }

        // 파싱
        if (needsParsing)
        {
            for (auto it = valueParams.begin(); it != valueParams.end(); it++)
                ReplaceAll(c.target, it->first, it->second);

            // TODO - Float로 변경 (에러나서 int로 임시 변환)
            c.target = std::to_string((int)CalculateString(c.target));
        }

        // Condition 체크 시작
        bool isNumber = true;
        for (int i = 0; i < c.target.length(); i++)
        {
            if (!isdigit(c.target[i]))
            {
                isNumber = false;
                break;
            }
        }

        // 위에서 파싱한 경우 
        if (isNumber)
            value = std::stoi(c.target);
        else // 일반적인 단일 문자인 경우
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