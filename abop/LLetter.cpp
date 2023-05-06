#include <string>
#include <vector>
#include <map>
#include "Utils.hpp"
#include "LLetter.hpp"

LLetter::LLetter()
{
    InitLetter();
}

LLetter::LLetter(const char& letter)
{
    InitLetter();
    std::string s;
    s += letter;
    this->SetLetter(s);
}

LLetter::LLetter(const std::string& letter)
{
    InitLetter();

    if (letter.size() > 1 && (letter.find('(') != std::string::npos))
    {
        // param이 존재 하는 경우
        this->SetLetter(split(letter, '(')[0]);
        this->SetParameters(split(split(split(letter, '(')[1], ')')[0], ','));
    }
    else
    {
        this->SetLetter(letter);
    }

}

LLetter::~LLetter()
{

}

std::string LLetter::GetLetter(bool isIncludeParam) const
{
    if (!mIsParam)
    {
        return mLetter;
    }

    if (isIncludeParam)
    {
        return mLetter + mParametersString;
    }

    return mLetter;
}

LLetter::Type LLetter::GetType() const
{
    return mType;
}

std::vector<std::string> LLetter::GetParameters() const
{
    return mParameters;
}

std::string LLetter::GetFormat() const
{
    return mFormat;
}

void LLetter::SetLetter(const std::string& letter)
{
    mType = static_cast<LLetter::Type> (letter[0]);
    mLetter = letter;
    mFormat = letter;
}

void LLetter::SetParameters(std::vector<std::string> params)
{
    mParameters = params;
    mParametersString = "";
    mFormat = mLetter;

    if (mParameters.size())
    {
        mIsParam = true;
        mParametersString = "(" + join(mParameters, ", ") + ")";
        
        mFormat += '(';
        for (int i = 0; i < mParameters.size() - 1; i++)
        {
            mFormat += ',';
        }
        mFormat += ')';
    }
}

bool LLetter::IsEmpty() const
{
    return mLetter.empty();
}

bool LLetter::IsEqual(const LLetter& other) const
{
    if (!mIsParam)
    {
        // 파라미터가 없는 경우 mLetter만 비교
        return mLetter == other.GetLetter(false);
    }

    if ((mLetter == other.GetLetter(false))
        && (mParameters.size() == other.GetParameters().size()))
    {
        // letter와 param 개수 모두 같은 경우
        return true;
    }

    return false;
}

bool LLetter::IsParametic() const
{
    return mIsParam;
}

void LLetter::CalculateParameter(std::map<std::string, std::string> valueParams)
{
    if (!IsParametic())
    {
        // 파라미터가 없는 경우
        return;
    }

    std::vector<std::string> newParams = std::vector<std::string>();

    for (const auto& [key, value] : valueParams)
    {
        ReplaceAll(mParametersString, key, value);
    }
    mParametersString = mParametersString.substr(1, mParametersString.size() - 2);    // '(', ')' 제외
    for (const std::string& p : split(mParametersString, ','))
    {
        // int 고정
        newParams.push_back(std::to_string((int)CalculateString(p)));
    }

    SetParameters(newParams);
}

void LLetter::InitLetter()
{
    mLetter = "";
    mType = Type::None;
    mIsParam = false;
    mParameters = std::vector<std::string>();
    mParametersString = "";
}