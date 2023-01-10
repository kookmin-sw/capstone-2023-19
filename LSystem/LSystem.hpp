#pragma once

class LRule;
class LLetter;

class LSystem
{
private:
    std::vector<LRule> rules_;
    std::vector<LLetter>* word_;
    std::string wordText_;
    std::string rulesText_;

public:
    LSystem();
    ~LSystem();

    // Get, Set
    std::string get_word() const;       // Word text
    std::string get_rules() const;      // Rules text

    void SetWord(const std::string&);

    // Word
    void ClearWord();

    // Rule
    void AddRule(const std::string&);
    void AddRule(const std::vector<std::string>&);
    void AddRule(const std::string&, const std::string&);
    void ClearRule();

    // Run
    void Iterate();
    void Iterate(int);
};
