#include <iostream>
#include "LLetter.hpp"

LLetter::LLetter()
{

}

LLetter::LLetter(const char& letter)
{
    this->set_letter(letter);
}

LLetter::~LLetter()
{

}

char LLetter::get_letter() const
{
    return this->letter_;
}

std::string LLetter::get_letter_by_string() const
{
    return std::string(1, this->letter_);
}

LLetter::Type LLetter::get_type() const
{
    return this->type_;
}

void LLetter::set_letter(const char& letter)
{
    this->type_ = static_cast<LLetter::Type> (letter);
    this->letter_ = letter;
}

bool LLetter::IsEqual(const LLetter& other)
{
    if (this->letter_ == other.get_letter())
    {
        return true;
    }

    return false;
}