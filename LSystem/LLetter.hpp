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

    char GetLetter() const;
    std::string GetLetterByString() const;
    Type GetType() const;

    void SetLetter(const char&);

    bool IsEqual(const LLetter&);
};