class RuleCondition
{
public:
    enum Sign
    {
        Same,
        More,
        Less,
        MoreSame,
        LessSame
    };

    struct Condition
    {
        std::string target;
        Sign sign;
        int compare;
    };

public:
    RuleCondition();
    RuleCondition(std::string str);
    ~RuleCondition();

    std::string GetConditionString() const;
    bool IsEmpty();

private:
    std::vector<Condition> mConds;
    std::string mCondsString;
    bool mEmpty;
};