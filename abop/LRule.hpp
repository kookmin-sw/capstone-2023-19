#pragma once

// !!! TEMP
#include <map>
#include <set>

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
        char previous;
        char next;
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
    char mBefore;
    std::map<int, After> mAfter;
    std::map<int, CSAfter> mCSAfter;
    std::vector<CSAfter> mSortedCSAfter;
    std::vector<After> mSortedAfter;

    int mNextAfterID;
    int mRuleCount;

public:
    LRule();
    LRule(const char&, const std::string&);
    LRule(const std::string&, const std::string&);
    LRule(char, char, char, const std::string&);
    ~LRule();

    char GetBefore() const;
    std::vector<LLetter> GetAfter(char previous, std::set<char>& next) const;

    std::vector<RuleInfo> GetRuleInfos();

    void AddAfter(char previous, char next, const std::string&);

    bool DeleteAfter(const int& afterId);

private:
    void InitLRule();
};