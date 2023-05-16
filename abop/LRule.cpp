#include <vector>
#include <map>
#include <string>
#include <iostream> // FORTEST
#include "RandomSeed.hpp"
#include "LLetter.hpp"
#include "LRule.hpp"

// LRule
LRule::LRule()
{
}

LRule::LRule(const char& before, const std::string& after)
{
    this->InitLRule();
    mBefore = before;
    
    this->AddAfter(NULL, NULL, after);
}

LRule::LRule(const std::string& before, const std::string& after)
{
    this->InitLRule();
    mBefore = before[0];

    this->AddAfter(NULL, NULL, after);
}

LRule::LRule(char privious, char before, char next, const std::string& after)
{
    this->InitLRule();
    mBefore = before;

    this->AddAfter(privious, next, after);
}

LRule::~LRule()
{

}

char LRule::GetBefore() const
{
    return mBefore;
}

std::vector<LLetter> LRule::GetAfter(char previous, std::set<char>& next) const
{
    if (!(previous == NULL && next.size() == 0))
    {
        // Context info가 있는 경우
        for (const CSAfter& csAfter : mSortedCSAfter)
        {
            // !!! 동일한 CS 규칙이 있는 경우 먼저 추가된 규칙 적용
            if (csAfter.next == NULL)
            {
                // previous 규칙
                if (csAfter.previous == previous)
                {
                    return csAfter.after.letters;
                }
            }
            else if (csAfter.previous == NULL)
            {
                // next 규칙
                if (next.find(csAfter.next) != next.end())
                {
                    return csAfter.after.letters;
                }
            }
            else
            {
                // previous + next 규칙
                if (csAfter.previous == previous && next.find(csAfter.next) != next.end())
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
        std::vector<LLetter> temp;
        return temp;
    }
    int index = dist(gen) % total;
    
    return mSortedAfter[index].letters;
}

std::vector<LRule::RuleInfo> LRule::GetRuleInfos()
{
    // this rule의 모든 정보 return
    // Ouput RuleInfo는 LRule.hpp에 정
    std::vector<RuleInfo> v = std::vector<RuleInfo>();

    // Context sensitive
    for (auto& [id, after] : mCSAfter)
    {
        RuleInfo ruleInfo = RuleInfo();
        ruleInfo.id = id;
        // A < B > D
        ruleInfo.before = "";
        if (after.previous != NULL)
        {
            ruleInfo.before += after.previous;
            ruleInfo.before += " < ";
        }
        ruleInfo.before += mBefore;
        if (after.next != NULL)
        {
            ruleInfo.before += " > ";
            ruleInfo.before += after.next;
        }
        
        ruleInfo.after = after.after.text;

        v.push_back(ruleInfo);
    }

    // Context nfree
    for (auto& [id, after] : mAfter)
    {
        RuleInfo ruleInfo = RuleInfo();
        ruleInfo.id = id;
        ruleInfo.before = mBefore;
        ruleInfo.after = after.text;

        v.push_back(ruleInfo);
    }

    return v;
}

void LRule::AddAfter(char previous, char next, const std::string& after)
{
    if (after.size() == 0)
    {
        // error
        return;
    }

    // After info 생성
    After afterInfo = After();
    afterInfo.letters = std::vector<LLetter>();

    afterInfo.text = after;
    for (const char& c : after)
    {
        afterInfo.letters.push_back(LLetter(c));
    }

    if (previous == NULL && next == NULL)
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
            if (csAfter.previous != NULL && csAfter.next != NULL)
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
            if (csAfter.previous != NULL && csAfter.next != NULL)
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