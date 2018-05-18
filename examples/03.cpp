/*
    Timmy example 3: Using pointers with Timmy
    Original author: Nguyen Hoang Duong (@NOVAglow).
    C++ Port by Nguyen Tuan Dung (@dungwinux)

    This example demonstrates the use of pointers in Timmy,
    or more specifically , timmy::add().
*/
#include <iostream>
#include <string>
#include "../src/ctimmy.hpp"

int main()
{
    std::string myString = "asdf";
    std::string *pMyStr = &myString;

    timmy myBot(50, "Question not understood :/", false);
    myBot.add("what my string", pMyStr);
    std::cout << myBot.answer("what is my string?") << std::endl;

    myString = "Something else";
    std::cout << myBot.answer("what is my string?") << std::endl;
}