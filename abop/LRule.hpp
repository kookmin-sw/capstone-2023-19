#pragma once

#include "LLetter.hpp";     // 전방 선언 이슈

class LLetter;
class RuleCondition;

class LRule
{
public:
    struct After
    {
        std::vector<LLetter> letters;
        std::string text;
        // 정의하지 않은 ~ 에러 발생
        // 임시로 ptr 사용 (메모리 해제 안함)
        RuleCondition* condition;
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
        std::string condition;
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
    LRule(const LLetter& before, const std::vector<LLetter>& after,
          const std::string& condition);
    LRule(const LLetter& previous, 
          const LLetter& before,
          const LLetter& next,
          const std::vector<LLetter>& after,
          const std::string& condition);
    ~LRule();

    LLetter GetBefore() const;
    std::vector<LLetter> GetAfter(const LLetter& previous,
                                  const LLetter& next) const;

    // Rule의 모든 정보 return
    std::vector<RuleInfo> GetRuleInfos();

    void AddAfter(const LLetter& previous,
                  const LLetter& next,
                  const std::vector<LLetter>& after,
                  const std::string& condition);

    bool DeleteAfter(const int& afterId);

private:
    void InitLRule();
};
