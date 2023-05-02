#include <string>
#include <vector>
#include "LLetter.hpp"

LLetter::LLetter()
{
    InitLetter();
}

LLetter::LLetter(const std::string& letter)
{
    InitLetter();
    this->SetLetter(letter);
}

LLetter::~LLetter()
{

}

std::string LLetter::GetLetter() const
{
    if (!mIsParam)
    {
        return mLetter;
    }

    return mLetter + mParametersString;
}

LLetter::Type LLetter::GetType() const
{
    return mType;
}

std::vector<std::string> LLetter::GetParameters() const
{
    return mParameters;
}

void LLetter::SetLetter(const std::string& letter)
{
    mType = static_cast<LLetter::Type> (letter[0]);
    mLetter = letter;
}

void LLetter::SetParameters(std::vector<std::string> param)
{
    mParameters = param;

    if (mParameters.size())
    {
        mIsParam = true;
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
        return mLetter == other.GetLetter();
    }

    if (mParameters.size() != other.GetParameters().size())
    {
        // parameter size가 다른 경우
        return false;
    }

    return true;
}

bool LLetter::IsParametic()
{
    return mIsParam;
}

void LLetter::InitLetter()
{
    mLetter = "";
    mType = Type::None;
    mIsParam = false;
    mParameters = std::vector<std::string>();
    mParametersString = "";
}