#pragma once

class LLetter
{
private:
    std::string letter_ = "";
public:
    LLetter();
    LLetter(char);
    LLetter(const std::string&);
    ~LLetter();

    std::string get_letter() const;

    void set_letter(const std::string&);
};