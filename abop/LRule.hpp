#pragma once

class LLetter;

class LRule
{
public:
    struct After
    {
        std::vector<LLetter> letters;
        std::string text;
    };

    // Context Sensitive
    struct CSAfter
    {
        LLetter previous;
        LLetter next;
        After after;
    };

    // for output
    struct RuleInfo
    {
        int id;
        std::string before;
        std::string after;
    };

private:
    LLetter mBefore;
    std::map<int, After> mAfter;
    std::map<int, CSAfter> mCSAfter;
    std::vector<CSAfter> mSortedCSAfter;
    std::vector<After> mSortedAfter;

    int mNextAfterID;
    int mRuleCount;

public:
    LRule();
    LRule(const LLetter& before, const std::vector<LLetter>& after);
    LRule(const LLetter& previous, 
          const LLetter& before,
          const LLetter& next,
          std::vector<LLetter>& after);
    ~LRule();

    LLetter GetBefore() const;
    std::vector<LLetter> GetAfter(const LLetter& previous,
                                  const LLetter& next) const;

    // Rule의 모든 정보 return
    std::vector<RuleInfo> GetRuleInfos();

    void AddAfter(const LLetter& previous,
                  const LLetter& next,
                  const std::vector<LLetter>& after);

    bool DeleteAfter(const int& afterId);

private:
    void InitLRule();
};