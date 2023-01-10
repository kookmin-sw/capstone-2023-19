#pragma once

class LRule
{
private:
    std::string before_;
    std::string after_;
public:
    LRule();
    LRule(const std::string&);
    LRule(const std::string&, const std::string&);
    ~LRule();

    std::string get_before() const;
    std::string get_after() const;
};