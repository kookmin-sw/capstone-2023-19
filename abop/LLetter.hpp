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
        MakeLeaf = 'J',
        MakeLeaf2 = 'K',
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
    std::string mLetter = "";
    Type mType = None;

    // Parametic
    bool mIsParam = false;
    std::vector<std::string> mParameters;
    std::string mParametersString;
    std::string mFormat;

public:
    LLetter();
    LLetter(const char&);
    // 파라미터 포함 안된 symbol만 등록, 1개씩 read
    LLetter(const std::string&);
    ~LLetter();

    std::string GetLetter(bool isIncludeParam = true) const;
    Type GetType() const;
    std::vector<std::string> GetParameters() const;
    // LLetter에 format 형식 반환
    // F(s, t) -> F(,)
    std::string GetFormat() const;

    void SetLetter(const std::string&);
    // 파라미터 지정, 파라미터 setter는 이 메소드를 통해서만 수행
    void SetParameters(std::vector<std::string> params);

    bool IsEmpty() const;
    // LLetter 비교
    // 파라미터가 없는 경우 단순히 string 비교
    // 파라미터가 있는 경우 parameter 개수만 비교 (format만)
    bool IsEqual(const LLetter&) const;
    bool IsParametic() const;

    void CalculateParameter(std::map<std::string, std::string> valueParams);

private:
    void InitLetter();
};