#include <vector>
#include <iostream> // FORTEST
#include "RandomSeed.hpp"
#include "Utils.hpp"
#include "Constants.hpp"
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
    // Parametric에서 사용되는 변수
    std::vector<LLetter> result = std::vector<LLetter>();
    std::map<std::string, std::string> valueParams = std::map<std::string, std::string>();
    std::vector<std::string> paramKey = mBefore.GetParameters();
    std::vector<std::string> paramValue = compare.GetParameters();

    // Parametric 이면 우선 valueParams에 compare의 정보 담아두기
    if (compare.IsParametic())
    {
        for (int i = 0; i < paramKey.size(); i++)
        {
            valueParams.insert({ paramKey[i], paramValue[i] });
        }
    }

    // Context info가 있는 경우
    if (!(previous.IsEmpty() && next.IsEmpty()))
    {
        // Context-Sensitive Parametric L-System
        if (compare.IsParametic())
        {
            std::vector<std::string> previousParamKey;
            std::vector<std::string> previousParamValue;
            std::vector<std::string> nextParamKey;
            std::vector<std::string> nextParamValue;

            for (const CSAfter& csAfter : mSortedCSAfter)
            {
                // 1. 계산에 앞서 우선 상황에 따라 변수 할당
                if (!csAfter.previous.IsEmpty())
                {
                    previousParamKey = csAfter.previous.GetParameters();
                    previousParamValue = previous.GetParameters();
                }
                if (!csAfter.next.IsEmpty())
                {
                    nextParamKey = csAfter.next.GetParameters();
                    nextParamValue = next.GetParameters();
                }

                // 2. 규칙 체크
                if (csAfter.next.IsEmpty())
                {
	                // previous 규칙만 존재하는 경우
                    if (csAfter.previous.IsEqual(previous))
                    {
                        // valueParams에 값 삽입
                    	for (int i = 0; i < previousParamValue.size(); i++)
                        {
                            valueParams.insert({ previousParamKey[i], previousParamValue[i] });
                        }

                        // 계산 후 반환
                        if (csAfter.after.condition->CheckCondition(valueParams))
                        {
                            for (LLetter letter : csAfter.after.letters) 
                            {
                                if (letter.IsParametic())
                                {
                                    letter.CalculateParameter(valueParams);
                                }

                                result.push_back(letter);
                            }
                            return result;
                        }
                    }
                }
                else if (csAfter.previous.IsEmpty())
                {
	                // next 규칙만 존재하는 경우
                    if (csAfter.next.IsEqual(next))
                    {
                        // valueParams에 값 삽입
                        for (int i = 0; i < nextParamValue.size(); i++)
                        {
                            valueParams.insert({ nextParamKey[i], nextParamValue[i] });
                        }

                        // 계산 후 반환
                        if (csAfter.after.condition->CheckCondition(valueParams))
                        {
	                        for (LLetter letter : csAfter.after.letters)
	                        {
		                        if (letter.IsParametic())
		                        {
                                    letter.CalculateParameter(valueParams);
		                        }

                                result.push_back(letter);
	                        }
                            return result;
                        }
                    }
                }
                else
                {
	                // previous + next 규칙
                    if (csAfter.previous.IsEqual(previous) && csAfter.next.IsEqual(next))
                    {
                        // valueParams에 값 삽입
                        for (int i = 0; i < previousParamValue.size(); i++)
                            valueParams.insert({ previousParamKey[i], previousParamValue[i] });
                        for (int i = 0; i < nextParamValue.size(); i++)
                            valueParams.insert({ nextParamKey[i], nextParamValue[i] });

                        // 계산 후 반환 - 식 체크할 수 있는 Condition 구현 필요 TODO
                        if (csAfter.after.condition->CheckCondition(valueParams))
                        {
                            for (LLetter letter : csAfter.after.letters)
                            {
                                //std::cout << letter.GetLetter() << "\n";
                                if (letter.IsParametic())
                                {
                                    letter.CalculateParameter(valueParams);
                                }

                                result.push_back(letter);
                            }
                            return result;
                        }
                    }
                }
            }
        }
        else // Context-Sensitive 단일 문자 L-System 
        {
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

    if (compare.IsParametic())
    {
        // compare (beforeLLetter)에 parametic이 있는 경우
        // 변환된 letter의 값에 파라미터 적용
        // ex. 
        // rule: f(t) -> f(t + 1)
        // f(3) to f(4)
        // ruleBefore의 paramSize와 beforeLLetter의 paramSize는 동일

        // condition check
        // !!! t > 0, t < 10 여러 개의 condition이 있을 때 (free의 경우)
        // 우선순위 : 먼저 들어온 Condition true인 규칙 > 나중에 들어온 Condition true인 규칙 > Condition 없는 규칙(얘네끼리는 랜덤 선택)

        // Condition 있는 규칙 먼저 체크
        for (int i = 0; i < mCFConditionCount; i++)
        {
            // Condition 결과가 True 인 경우는 바로 Return
            if (mSortedAfter[i].condition->CheckCondition(valueParams))
            {
                for (LLetter letter : mSortedAfter[i].letters)
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
        }

        // Condition true인 규칙 x, Condition 없는 규칙이 없다면 원래 것 그대로 return
        if (mCFNoConditionCount == 0)
        {
            result.push_back(compare);
            return result;
        }

        // Context-Free, No Condition 규칙은 랜덤 선택
        int index = dist(gen) % mCFNoConditionCount + mCFConditionCount;

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
    int index = dist(gen) % total;
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

        // Condition 존재하는 case가 앞쪽에 오게 sort
    	std::sort(mSortedAfter.begin(), mSortedAfter.end(), CustomSort);

        // 각 Count 업데이트
        if (condition != "")
            mCFConditionCount++;
        else
            mCFNoConditionCount++;
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
        // 삭제 전 Condition 유무에 따라 Count Update
        if (!mAfter.find(afterId)->second.condition->IsEmpty())
            mCFConditionCount--;
        else
            mCFNoConditionCount--;

        mAfter.erase(afterId);
        mRuleCount--;

        // !!! not good
        // insert(or delete)할 때마다 새로운 after vector 생성
        mSortedAfter = std::vector<After>();
        for (auto& [_, csAfter] : mAfter)
        {
            mSortedAfter.push_back(csAfter);
        }

        // Condition 존재하는 case가 앞쪽에 오게 sort
        std::sort(mSortedAfter.begin(), mSortedAfter.end(), CustomSort);
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
    mCFConditionCount = 0;
    mCFNoConditionCount = 0;
}

// Rule AddAfter 시 Condition 이 존재하는 변환 규칙 먼저 앞에 배치하게 sort
bool LRule::CustomSort(After& a, After& b)
{
    if (!a.condition->IsEmpty() && b.condition->IsEmpty())
        return true;
    return false;
}
