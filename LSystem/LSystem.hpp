#pragma once

class LRule;
class LLetter;

class LSystem
{
private:
    std::vector<LRule> rules_;
    std::vector<LLetter>* word_;

public:
    LSystem();
    ~LSystem();

    // Get, Set
    std::string get_word() const;       // Word text
    std::string get_rules() const;      // Rules text

    void SetWord(const std::string&);
    
    // Rule
    void AddRule(const std::string&);
    void AddRule(const char&, const std::string&);
    void AddRule(const std::string&, const std::string&);

    // Run
    void Iterate();
    void Iterate(int);
};
