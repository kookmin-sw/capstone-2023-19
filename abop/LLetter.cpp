#include <string>
#include "LLetter.hpp"

LLetter::LLetter()
{

}

LLetter::LLetter(const char& letter)
{
    this->SetLetter(letter);
}

LLetter::~LLetter()
{

}

char LLetter::GetLetter() const
{
    return this->letter_;
}

std::string LLetter::GetLetterByString() const
{
    return std::string(1, this->letter_);
}

LLetter::Type LLetter::GetType() const
{
    return this->type_;
}

void LLetter::SetLetter(const char& letter)
{
    this->type_ = static_cast<LLetter::Type> (letter);
    this->letter_ = letter;
}

bool LLetter::IsEqual(const LLetter& other)
{
    if (this->letter_ == other.GetLetter())
    {
        return true;
    }

    return false;
}