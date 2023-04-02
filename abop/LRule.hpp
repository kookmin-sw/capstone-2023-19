#pragma once

class LLetter;

class LRule
{
private:
    // 변환 규칙 key가 하나인 경우만 가정
    LLetter* before_;
    std::vector<LLetter> after_;

    std::string key_;
    std::string value_;

public:
    LRule();
    // Full Text (ex. "A->ABAB")
    LRule(const std::string&);
    // Key & value (ex. 'A', "ABAB")
    LRule(const char&, const std::string&);
    LRule(const std::string&, const std::string&);
    ~LRule();

    LLetter GetBefore() const;
    std::vector<LLetter> GetAfter() const;
    std::string GetRule() const;

    void GetKey(char* out);
    void GetValue(char* out);

private:
    void SetRule(const char&, const std::string&);
};