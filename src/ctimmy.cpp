/*
    ctimmy.cpp
    ctimmy - C++ port of timmy - Pascal unit for creating chat bots
    Version 1.2.0
    
    Copyright (C) 2018 42tm Team <fourtytwotm@gmail.com>
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.
    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "ctimmy.hpp"

// Implementation
/*
    Given a string, process it so that the first and the last
    character are not space, and there is no multiple spaces
    character in a row.
*/
std::string strTrim(std::string str)
{
    bool spaceOn = false;
    while (str.front() == ' ')
        str.erase(str.begin());
    while (str.back() == ' ')
        str.pop_back();
    std::string flagStr;
    for (char iter : str)
        if (iter != ' ')
        {
            flagStr += iter;
            spaceOn = false;
        }
        else
        {
            if (!spaceOn)
            {
                flagStr += ' ';
                spaceOn = true;
            }
        }
    return (flagStr);
}
/*
    Given a string, split the string using the delimiter
    and return an array containing the seperated strings.
*/
tStrArray strSplit(std::string s, char delimiter)
{
    tStrArray splited;
    std::string token;
    std::istringstream iss(s);
    while (std::getline(iss, token, delimiter))
        splited.push_back(token);
    return (splited);
}

/*
    Given two arrays of strings, compare them.
    Return true if they are the same, false otherwise.
*/
bool compareStrArrays(tStrArray arrayA, tStrArray arrayB)
{
    if (arrayA.size() != arrayB.size())
        return false;
    for (size_t iter = 0; iter < arrayA.size(); ++iter)
        if (arrayA[iter] != arrayB[iter])
            return false;
    return true;
}
/*
    Given an array of string, join them using Linker.
    timmy::strJoin({"this", "is", "an", "example"}, "@@");
      -> "this@@is@@an@@example"
*/
std::string strJoin(tStrArray strList, std::string linker)
{
    std::string joined;
    for (std::string iter : strList)
        joined += iter + linker;
    joined.erase(std::prev(joined.end(), linker.size()), joined.end());
    return joined;
}

/*
    Construct class with some default values set.
*/
timmy::timmy()
{
    this->enabled = true;
    this->noUdstdRep = "Sorry, I didn't get that";
    this->dupesCheck = true;
    this->tPercent = 70;
}

timmy::timmy(int newPercent, std::string newRep, bool newDpCheck)
{
    this->enabled = true;
    this->noUdstdRep = newRep;
    this->dupesCheck = newDpCheck;
    this->tPercent = newPercent;
}

/*
    Enable the instance
*/
void timmy::enable()
{
    this->enabled = true;
}

/*
    Disable the instance
*/
void timmy::disable()
{
    this->enabled = false;
}

/*
    Add data to bot object's metadata base.
    Data include message's keywords and possible replies to the message.

    Return: 102 if object is not enabled
            202 if dupesCheck = True and found a match to msgKeywords in msgKeywordsList
            200 if the adding operation succeed
*/
int timmy::add(tStrArray msgKeywords, tStrArray replies)
{
    if (!enabled)
        return 102;
    for (std::string &iter : msgKeywords)
        std::transform(iter.begin(), iter.end(), iter.begin(), ::tolower);
    if (dupesCheck && (nOfEntries > 0))
        for (tStrArray iter : msgKeywordsList)
            if (compareStrArrays(iter, msgKeywords))
                return 202;

    msgKeywordsList.push_back(msgKeywords);
    replyList.push_back(replies);
    ++(this->nOfEntries);
    return 200;
}

/*
    Add data to bot but this one gets string inputs instead of tStrArray inputs.
    This use strSplit() to split the string inputs (with a space character as the delimiter
    for the message keywords string input and a semicolon character for the replies string input).
    The main work is done by the primary implementation of timmy::add().

    Custom delimiters is accepted through default parameters.

    Return: timmy::add(tStrArray, tStrArray);
*/

int timmy::add(std::string keywordsStr, std::string repStr, char kStrDeli, char mStrDeli)
{
    return (timmy::add(strSplit(keywordsStr, kStrDeli), strSplit(repStr, mStrDeli)));
}

/*
    Given a set of keywords, find matches to that set in msgKeywordsList,
    remove the matches, and remove the correspondants in replyList as well.
    This function simply saves offsets of the matching arrays in msgKeywordsList
    and then call timmy::remove(int).

    Return: 102 if object is not enabled
            308 if the operation succeed
*/
int timmy::remove(tStrArray msgKeywords)
{
    if (!enabled)
        return 102;

    for (std::string iter : msgKeywords)
        std::transform(iter.begin(), iter.end(), iter.begin(), ::tolower);
    std::vector<int> indexes(msgKeywordsList.size());

    // Get offsets of keywords set that match the given msgKeywords parameter
    // and later deal with them using timmy::remove(int)

    for (auto iter = msgKeywordsList.begin(); iter != msgKeywordsList.end(); ++iter)
        if (compareStrArrays(*iter, msgKeywords))
            indexes.push_back(std::distance(msgKeywordsList.begin(), iter));

    int counter = indexes.size();
    while (counter > 0)
    {
        timmy::remove(indexes[indexes.size() - counter] - indexes.size() + counter);
        --counter;
    }
    return 308;
}
/*
    Remove data from msgKeywordsList at msgKeywordsList[aIndex].
    Return: 102 if object is not enabled
            305 if the given index is invalid (out of bound)
            300 if operation successful
*/
int timmy::remove(int aIndex)
{
    if (!enabled)
        return 102;
    if ((aIndex < 0) || (aIndex >= nOfEntries))
        return 305;

    msgKeywordsList.erase(std::next(msgKeywordsList.begin(), aIndex));
    replyList.erase(std::next(replyList.begin(), aIndex));

    --(this->nOfEntries);
    return 300;
}

/*
    An implementation of `remove` that uses string as an argument
    instead of a tStrArray. The string is delimited using the space character
    to form a tStrArray, and then pass that tStrArray to the
    common remove function. Default value of kStrDeli is ' '

    Return timmy::remove(tStrArray);
*/
int timmy::remove(std::string keywordsStr, char kStrDeli)
{
    return (timmy::remove(strSplit(keywordsStr, kStrDeli)));
}

/*
    Answer the given message, using assets in the metadata
*/
std::string timmy::answer(std::string tMessage)
{
    if (!enabled)
        return ("");

    // Define random engine
    std::default_random_engine generator;

    // Pre-process the message
    std::string flagM = strTrim(tMessage);
    std::transform(flagM.begin(), flagM.end(), flagM.begin(), ::tolower);

    // Delete punctuation at the end of the message (like "?" or "!")
    while (!std::isalnum(flagM.back()))
        flagM.erase(std::prev(flagM.end()));

    tStrArray flagWords = strSplit(flagM, ' ');
    int counter;
    for (int metaIter = 0; metaIter < nOfEntries; ++metaIter)
    {
        counter = 0;
        for (auto mKIter : msgKeywordsList[metaIter])
            for (auto mWIter : flagWords)
                counter += (mWIter == mKIter);
        if (((double)counter / msgKeywordsList[metaIter].size()) * 100 >= tPercent)
        {
            auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
            generator.seed(seed);
            std::uniform_int_distribution<int> distribution(0, replyList[metaIter].size() - 1);
            return (replyList[metaIter][distribution(generator)]);
        }
    }
    return (noUdstdRep);
}