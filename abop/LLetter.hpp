#pragma once

class LLetter
{
public:
    enum Type
    {
        None,
        Forward = 'F',
        NoDrawForward = 'f',
        TurnLeft    = '+',
        TurnRight   = '-',
        PitchUp = '^',
        PitchDown = '&',
        RollLeft = '\\',
        RollRight = '/',
        TurnAround = '|',
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