#pragma once

class LLetter
{
public:
    enum Type
    {
        None,
        Forward = 'F',
        Left    = '+',
        Right   = '-',
        Push    = '[',
        Pop     = ']'
    };

private:
    char letter_ = 0;
    Type type_ = None;

public:
    LLetter();
    LLetter(const char&);
    ~LLetter();

    char get_letter() const;
    std::string get_letter_by_string() const;
    Type get_type() const;

    void set_letter(const char&);

    bool IsEqual(const LLetter&);
};