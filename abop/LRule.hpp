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
    // previous, next 조건이 맞는 경우 rule after로 변환함
    // 이 때 compare가 parametic인 경우 이 값을 기준으로 after를 수정
    // condition이 있을 경우 condition이 true인 경우
    // rule: f(t) -> f(t + 1)s(t - 1)
    // compare: f(5), 이 때 compare과 ruleBefore의 format은 동일해야 함
    // result: f(6)s(4)
    std::vector<LLetter> GetAfter(const LLetter& previous,
                                  const LLetter& next,
                                  const LLetter& compare) const;

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
