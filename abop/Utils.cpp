#include <string>
#include <vector>
#include <stack>
#include <iostream>
#include <sstream>
#include <numeric>
#include <algorithm>
#include <iterator>
#include "Utils.hpp"

std::string join(const std::vector<std::string>& strings, std::string delim)
{
    if (strings.empty()) {
        return std::string();
    }

    return std::accumulate(strings.begin() + 1, strings.end(), strings[0],
        [&delim](std::string x, std::string y) {
            return x + delim + y;
        }
    );
}

std::vector<std::string> split(std::string str, char delimiter)
{
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string buffer;

    while (getline(ss, buffer, delimiter))
    {
        result.push_back(buffer);
    }

    return result;
}

void RemoveAll(std::string& s, const char& c)
{
    s.erase(remove(s.begin(), s.end(), ' '), s.end());
}

void ReplaceAll(std::string& s, const std::string& before, const std::string& after)
{
    std::string::size_type pos = 0;

    while ((pos = s.find(before)) != std::string::npos)
    {
        s.replace(pos, before.size(), after);
    }
}

bool IsOperator(const char& c)
{
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')' || c == '^';
}

bool IsOperator(const std::string& s)
{
    return s == "+" || s == "-" || s == "*" || s == "/" || s == "(" || s == ")" || s == "^";
}

std::string charToString(const char& c)
{
    std::string res = "";
    res += c;
    return res;
}

float Calculate(const float& a, const std::string& oper, const float& b)
{
    if (oper == "+")
    {
        return a + b;
    }
    else if (oper == "-")
    {
        return a - b;
    }
    else if (oper == "*")
    {
        return a * b;
    }
    else if (oper == "/")
    {
        return a / b;
    }
    else if (oper == "^")
    //else if (oper == "p")
    {
        // pitch up ^ 이미 있어서 임시로 p 사용
        return pow(a, b);
    }

    return 0.0f;
}

int prec(char op)
{
    switch (op)
    {
        case '(':
        case ')':
            return 0;
        case '+':
        case '-':
            return 1;
        case '*':
        case '/':
            return 2;
    }

    return -1;
}

float CalculateString(std::string s)
{
    // !!! 음수에 대한 연산 예외 처리를 안했음 (ex. 1 + -1)
    RemoveAll(s, ' ');  // 공백 제거

    std::string number = "";
    std::vector<std::string> prior = std::vector<std::string>();
    std::stack<char> operators = std::stack<char>();

    for (const char& c : s)
    {
        if (IsOperator(c))
        {
            if (number.size() > 0)
            {
                prior.push_back(number);
                number = "";
            }

            //if (c == '*' || c == '/')
            while (!operators.empty() && prec(c) <= prec(operators.top()))
            {
                //if (operators.size() > 0 && (operators.top() == '*' || operators.top() == '/'))
                //{
                    prior.push_back(charToString(operators.top()));
                    operators.pop();
                //}
            }

            if (c == ')')
            {
                while (operators.top() != '(')
                {
                    prior.push_back(charToString(operators.top()));
                    operators.pop();
                }
                operators.pop();

                continue;
            }

            operators.push(c);
        }
        else
        {
            number += c;
        }
    }

    if (number.size() > 0)
    {
        prior.push_back(number);
    }

    while (!operators.empty())
    {
        prior.push_back(charToString(operators.top()));
        operators.pop();
    }

    float sum = 0.0f;
    std::stack<float> numbers = std::stack<float>();

    for (std::string ts : prior)
    {
        if (IsOperator(ts))
        {
            float second = numbers.top();
            numbers.pop();
            float first = numbers.top();
            numbers.pop();
            numbers.push(Calculate(first, ts, second));
        }
        else
        {
            if (ts[0] == '~')
            {
                // 음수인 경우
                std::string tts = ts.substr(1, ts.size() - 1);
                numbers.push(std::stof(tts) * -1);
            }
            else
            {
                numbers.push(std::stof(ts));
            }
        }
    }

    return numbers.top();
}

bool IsFloat(const std::string& s) {
    std::istringstream iss(s);
    float dummy;
    iss >> std::noskipws >> dummy;
    return iss && iss.eof();
}