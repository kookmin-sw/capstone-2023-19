#pragma once

class LLetter
{
public:
    enum Type
    {
        None,
        Forward = 'F',
        NoDrawForward = 'f',
        NoDrawForward2 = 'G',
        TurnLeft = '+',
        TurnRight = '-',
        PitchUp = '^',
        PitchDown = '&',
        RollLeft = '\\',
        RollRight = '/',
        TurnAround = '|',
        Push = '[',
        Pop = ']',
        StartingPoint = '{',
        EndingPoint = '}',
        MarkingPoint = '.'
    };

private:
    std::string mLetter = 0;
    Type mType = None;

    // Parametic
    bool mIsParam = false;
    std::vector<std::string> mParameters;

public:
    LLetter();
    LLetter(const char&);
    ~LLetter();

    std::string GetLetter() const;
    Type GetType() const;
    std::vector<std::string> GetParameters() const;

    void SetLetter(const char&);
    void SetParameters(std::vector<std::string> param);

    bool IsEmpty() const;
    // LLetter 비교
    // 파라미터가 없는 경우 단순히 string 비교
    // 파라미터가 있는 경우 parameter 개수만 비교 (format만)
    bool IsEqual(const LLetter&) const;
    bool IsParametic();
};