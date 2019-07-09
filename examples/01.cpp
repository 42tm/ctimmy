/*
    Timmy example 1: The basics
    Original author: Nguyen Hoang Duong (@you-create).
    C++ Port by Nguyen Tuan Dung (@dungwinux)
    
    This example is similar to the one demonstrated in the README file.
    This example demonstrates all 2 different ways to add data to a TTimmy
    object's metadata by using the overloaded function add.
*/
#include <iostream>
#include "../src/ctimmy.hpp"

int main()
{
    timmy myBot(70, "I gave up", false);
    myBot.add(strSplit("Hello", " "), strSplit("Greetings!|Hello to you!|Hi!", "|"));
    myBot.add("How are you", "I am fine!;Never better!;I'm doing great.");
    myBot.add("What 2 + 2", "The answer is 4", " ", "@");

    std::cout
        << myBot.answer("Hello!") << std::endl                        // -> "Greetings!" or "Hello to you!" or "Hi!" (randomly selected)
        << myBot.answer("How are you?") << std::endl                  // -> "I am fine!" or "Never better!" or "I"m doing great."
        << myBot.answer("What is 2 + 2?") << std::endl                // -> "The answer is 4"
        << myBot.answer("What is the meaning of life?") << std::endl; // -> "I gave up"
}
