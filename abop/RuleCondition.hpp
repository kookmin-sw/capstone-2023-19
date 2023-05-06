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
    bool IsEmpty() const;
    bool CheckCondition(std::map<std::string, std::string> valueParams) const;

private:
    std::vector<Condition> mConds;
    std::string mCondsString;
    bool mEmpty;
};