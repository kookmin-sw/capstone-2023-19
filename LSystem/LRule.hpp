#pragma once

class LLetter;

class LRule
{
private:
    // 변환 규칙 key가 하나인 경우만 가정
    LLetter* before_;
    std::vector<LLetter> after_;

public:
    LRule();
    // Full Text (ex. "A->ABAB")
    LRule(const std::string&);
    // Key & value (ex. 'A', "ABAB")
    LRule(const char&, const std::string&);
    LRule(const std::string&, const std::string&);
    ~LRule();

    LLetter get_before() const;
    std::vector<LLetter> get_after() const;
    std::string get_rule() const;

private:
    void set_rule(const char&, const std::string&);
};