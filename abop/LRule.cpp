#include <vector>
#include <map>
#include <string>
#include <iostream> // FORTEST
#include "RandomSeed.hpp"
#include "Utils.hpp"
#include "LLetter.hpp"
#include "RuleCondition.hpp"
#include "LRule.hpp"

// LRule
LRule::LRule()
{
}

LRule::LRule(const LLetter& before, const std::vector<LLetter>& after, const std::string& condition)
{
    this->InitLRule();
    mBefore = before;
    
    this->AddAfter(LLetter(), LLetter(), after, condition);
}

LRule::LRule(const LLetter& previous, const LLetter& before, 
             const LLetter& next, const std::vector<LLetter>& after,
             const std::string& condition)
{
    this->InitLRule();
    mBefore = before;

    this->AddAfter(previous, next, after, condition);
}

LRule::~LRule()
{
}

LLetter LRule::GetBefore() const
{
    return mBefore;
}

std::vector<LLetter> LRule::GetAfter(const LLetter& previous,
                                     const LLetter& next,
                                     const LLetter& compare) const
{
    if (!(previous.IsEmpty() && next.IsEmpty()))
    {
        // Context info가 있는 경우
        for (const CSAfter& csAfter : mSortedCSAfter)
        {
            // !!! 동일한 CS 규칙이 있는 경우 먼저 추가된 규칙 적용
            if (csAfter.next.IsEmpty())
            {
                // previous 규칙
                if (csAfter.previous.IsEqual(previous))
                {
                    return csAfter.after.letters;
                }
            }
            else if (csAfter.previous.IsEmpty())
            {
                // next 규칙
                if (csAfter.next.IsEqual(next))
                {
                    return csAfter.after.letters;
                }
            }
            else
            {
                // previous + next 규칙
                if (csAfter.previous.IsEqual(previous) && csAfter.next.IsEqual(next))
                {
                    return csAfter.after.letters;
                }
            }
        }
    }

    // 확률형 after 적용
    int total = mSortedAfter.size();
    if (total == 0)
    {
        // 일치하는 context sensitive rule이 없고 context free rule이 없는 경우
        // 빈 vector return
        std::vector<LLetter> temp = std::vector<LLetter>();
        return temp;
    }
    int index = dist(gen) % total;

    if (compare.IsParametic())
    {
        // compare (beforeLLetter)에 parametic이 있는 경우
        // 변환된 letter의 값에 파라미터 적용
        // ex. 
        // rule: f(t) -> f(t + 1)
        // f(3) to f(4)
        // ruleBefore의 paramSize와 beforeLLetter의 paramSize는 동일
        std::vector<LLetter> result = std::vector<LLetter>();
        std::map<std::string, std::string> valueParams = std::map<std::string, std::string>();
        std::vector<std::string> paramKey = mBefore.GetParameters();
        std::vector<std::string> paramValue = compare.GetParameters();

        // !!! condition check

        for (int i = 0; i < paramKey.size(); i++)
        {
            valueParams.insert({ paramKey[i], paramValue[i] });
        }

        for (LLetter letter : mSortedAfter[index].letters)
        {
            if (letter.IsParametic())
            {
                // after letter에 param이 있는 경우
                // param key value map을 통해 변환
                letter.CalculateParameter(valueParams);
            }

            result.push_back(letter);
        }

        return result;
    }

    // rule before(key)의 파라미터가 없는 경우
    // 변환된 after 모두 iterated에 추가
    
    return mSortedAfter[index].letters;
}

std::vector<LRule::RuleInfo> LRule::GetRuleInfos()
{
    std::vector<RuleInfo> v = std::vector<RuleInfo>();

    // Context sensitive
    for (auto& [id, after] : mCSAfter)
    {
        RuleInfo ruleInfo = RuleInfo();
        ruleInfo.id = id;
        // A < B > D
        ruleInfo.before = "";
        if (!after.previous.IsEmpty())
        {
            ruleInfo.before += after.previous.GetLetter();
            ruleInfo.before += " < ";
        }
        ruleInfo.before += mBefore.GetLetter();
        if (!after.next.IsEmpty())
        {
            ruleInfo.before += " > ";
            ruleInfo.before += after.next.GetLetter();
        }
        
        ruleInfo.after = after.after.text;

        ruleInfo.condition = "";
        if (!after.after.condition->IsEmpty())
        {
            ruleInfo.condition = after.after.condition->GetConditionString();
        }

        v.push_back(ruleInfo);
    }

    // Context free
    for (auto& [id, after] : mAfter)
    {
        RuleInfo ruleInfo = RuleInfo();
        ruleInfo.id = id;
        ruleInfo.before = mBefore.GetLetter();
        ruleInfo.after = after.text;

        ruleInfo.condition = "";
        if (!after.condition->IsEmpty())
        {
            ruleInfo.condition = after.condition->GetConditionString();
        }

        v.push_back(ruleInfo);
    }

    return v;
}

void LRule::AddAfter(const LLetter& previous,
                     const LLetter& next,
                     const std::vector<LLetter>& after,
                     const std::string& condition)
{
    if (after.size() == 0)
    {
        // error
        return;
    }

    // After info 생성
    After afterInfo = After();
    afterInfo.condition = new RuleCondition(condition);
    afterInfo.letters = after;

    afterInfo.text = "";
    for (const LLetter& c : after)
    {
        afterInfo.text += c.GetLetter();
    }

    if (previous.IsEmpty() && next.IsEmpty())
    {
        // Context free
        mAfter.insert({ mNextAfterID, afterInfo });

        // !!! not good
        // insert(or delete)할 때마다 새로운 after vector 생성
        mSortedAfter = std::vector<After>();
        for (auto& [_, csAfter] : mAfter)
        {
            mSortedAfter.push_back(csAfter);
        }
    }
    else
    {
        // Context sensitive
        CSAfter csAfterInfo = CSAfter();
        csAfterInfo.after = afterInfo;
        csAfterInfo.previous = previous;
        csAfterInfo.next = next;

        mCSAfter.insert({ mNextAfterID, csAfterInfo });

        // !!! not good
        // previous와 next 모두 있는 경우 앞에 배치
        mSortedCSAfter = std::vector<CSAfter>();
        std::vector<CSAfter> tv = std::vector<CSAfter>();
        for (auto& [_, csAfter] : mCSAfter)
        {
            if (!csAfter.previous.IsEmpty() && !csAfter.next.IsEmpty())
            {
                mSortedCSAfter.push_back(csAfter);
            }
            else
            {
                tv.push_back(csAfter);
            }
        }
        mSortedCSAfter.insert(mSortedCSAfter.end(), tv.begin(), tv.end());
    }

    mNextAfterID++;
    mRuleCount++;
}

bool LRule::DeleteAfter(const int& afterId)
{
    if (mAfter.find(afterId) != mAfter.end())
    {
        mAfter.erase(afterId);
        mRuleCount--;

        // !!! not good
        // insert(or delete)할 때마다 새로운 after vector 생성
        mSortedAfter = std::vector<After>();
        for (auto& [_, csAfter] : mAfter)
        {
            mSortedAfter.push_back(csAfter);
        }
    }
    else if (mCSAfter.find(afterId) != mCSAfter.end())
    {
        mCSAfter.erase(afterId);
        mRuleCount--;

        // !!! not good
        // insert(or delete)할 때마다 새로운 after vector 생성
        mSortedCSAfter = std::vector<CSAfter>();
        std::vector<CSAfter> tv = std::vector<CSAfter>();
        for (auto& [_, csAfter] : mCSAfter)
        {
            if (!csAfter.previous.IsEmpty() && !csAfter.next.IsEmpty())
            {
                mSortedCSAfter.push_back(csAfter);
            }
            else
            {
                tv.push_back(csAfter);
            }
        }
        mSortedCSAfter.insert(mSortedCSAfter.end(), tv.begin(), tv.end());
    }

    if (mRuleCount == 0)
    {
        // after 삭제 후 남은 after가 없는 경우 true return
        return true;
    }

    return false;
}

// private
void LRule::InitLRule()
{
    mAfter = std::map<int, After>();
    mCSAfter = std::map<int, CSAfter>();

    mNextAfterID = 0;
    mRuleCount = 0;
}
