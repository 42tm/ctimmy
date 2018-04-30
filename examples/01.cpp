/*
    Timmy example 1: The basics
    Original author: Nguyen Hoang Duong (@NOVAglow).
    C++ Port by Nguyen Tuan Dung (@dungwinux)
    This example is similar to the one demonstrated in the README file.
    This example demonstrates all 2 different ways to add data to a TTimmy
    object's metadata by using the overloaded function Add.
*/
#include <iostream>
#include "../src/ctimmy.hpp"

int main()
{
    TTimmy MyBot;
    MyBot.Init();
    MyBot.NoUdstdRep = "I gave up.";
    MyBot.Add(StrSplit("Hello", ' '), StrSplit("Greetings!|Hello to you!|Hi!", '|'));
    MyBot.Add("How are you", "I am fine!;Never better!;I'm doing great.");
    MyBot.Add("What 2 + 2", "The answer is 4", ' ', '@');

    std::cout
        << MyBot.Answer("Hello!") << std::endl                        // -> "Greetings!" or "Hello to you!" or "Hi!" (randomly selected)
        << MyBot.Answer("How are you?") << std::endl                  // -> "I am fine!" or "Never better!" or "I"m doing great."
        << MyBot.Answer("What is 2 + 2?") << std::endl                // -> "The answer is 4"
        << MyBot.Answer("What is the meaning of life?") << std::endl; // -> "I gave up"
}