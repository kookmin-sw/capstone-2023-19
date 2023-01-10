#include <iostream>
#include "LLetter.hpp"

LLetter::LLetter()
{

}

LLetter::LLetter(char letter) : letter_(std::string(1, letter))
{

}

LLetter::LLetter(const std::string& letter) : letter_(letter)
{

}

LLetter::~LLetter()
{

}

std::string LLetter::get_letter() const
{
    return this->letter_;
}

void LLetter::set_letter(const std::string& letter)
{
    this->letter_ = letter;
}