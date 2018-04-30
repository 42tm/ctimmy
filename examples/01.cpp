/*
    Timmy example 1: The basics
    Original author: Nguyen Hoang Duong (@NOVAglow).
    C++ Port by Nguyen Tuan Dung (@dungwinux)
    This example is similar to the one demonstrated in the README file.
    This example demonstrates all 2 different ways to add data to a TTimmy
    object's metadata by using the overloaded function add.
*/
#include <iostream>
#include "../src/ctimmy.hpp"

int main()
{
    timmy MyBot;
    MyBot.noUdstdRep = "I gave up.";
    MyBot.add(strSplit("Hello", ' '), strSplit("Greetings!|Hello to you!|Hi!", '|'));
    MyBot.add("How are you", "I am fine!;Never better!;I'm doing great.");
    MyBot.add("What 2 + 2", "The answer is 4", ' ', '@');

    std::cout
        << MyBot.answer("Hello!") << std::endl                        // -> "Greetings!" or "Hello to you!" or "Hi!" (randomly selected)
        << MyBot.answer("How are you?") << std::endl                  // -> "I am fine!" or "Never better!" or "I"m doing great."
        << MyBot.answer("What is 2 + 2?") << std::endl                // -> "The answer is 4"
        << MyBot.answer("What is the meaning of life?") << std::endl; // -> "I gave up"
}