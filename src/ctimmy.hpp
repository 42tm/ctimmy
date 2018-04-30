/*
    ctimmy - C++ port of Timmy Pascal unit
    
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
#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>

// Interface
typedef std::vector<std::string> tStrArray;

/*
    Metadata refers to two arrays holding data:
    mKeywordsList which holds keywords, and
    replyList which holds replies

      mKeywordsList [                                 replyList [
                     [*keywords for message 1*],                [*possible answers for message 1*],
                     [*keywords for message 2*],                [*possible answers for message 2*],
                                 ...                                             ...
                                                 ]                                                   ]

    Variables (see also the README file):

      enabled            : Acts like initialized but used in fewer number of functions
      nOfEntries         : Number of entries (elements) in mKeywordsList or replyList
      dupesCheck         : Check for duplicate or not (might be time-saving if we don't check for duplicate)
      tPercent           : Minimum percentage of the number of keywords over all the words of the message
                           so that the bot object can "understand" and have a reply.
                           (Sorry I don't have a good way to explain it)
      noUdstdRep : String to assign to tTimmy.answer in case there's no possible answer to the given message
*/

class tTimmy;

class tTimmy
{

  public:
    bool enabled = false;
    bool dupesCheck = true;
    int tPercent = 0;
    std::string noUdstdRep;

    tTimmy();
    int add(tStrArray mKeywords, tStrArray replies);
    // int add(std::string keywordsStr, std::string repStr);
    int add(std::string keywordsStr, std::string repStr, char kStrDeli, char mStrDeli);
    int remove(tStrArray mKeywords);
    int remove(int aIndex);
    std::string answer(std::string tMessage);

  private:
    int nOfEntries = 0;
    std::vector<tStrArray> mKeywordsList;
    std::vector<tStrArray> replyList;

    // Deprecated
    // void Update();
};

std::string strTrim(std::string s);
tStrArray strSplit(std::string s, char delimiter);
bool compareStrArrays(tStrArray arrayA, tStrArray arrayB);

/*
    Given a string, process it so that the first and the last
    character are not space, and there is no multiple spaces
    character in a row.
*/
std::string strTrim(std::string s)
{
    bool spaceOn;
    while (s.front() == ' ')
        s.erase(s.begin());
    while (s.back() == ' ')
        s.erase(std::prev(s.end()));
    std::string flagStr;
    for (char iter : s)
        if (iter != ' ')
        {
            flagStr += iter;
            spaceOn = false;
        }
        else
            switch (spaceOn)
            {
            case true:
                continue;
            case false:
                flagStr += ' ';
                spaceOn = true;
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
    s += delimiter;
    std::string flagStr;
    for (char iter : s)
        if (iter != delimiter)
            flagStr += iter;
        else
        {
            if (flagStr.empty())
                continue;
            splited.push_back(flagStr);
            flagStr.clear();
        }
    if (splited.empty())
    {
        splited.resize(1);
        splited[0] = s;
    }
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
    construct class with some default values set.
*/
tTimmy::tTimmy()
{
    dupesCheck = true;
    noUdstdRep = "Sorry, I didn't get that";
    tPercent = 70;
    nOfEntries = 0;
    enabled = true;
}

/*
    add data to bot object's metadata base.
    Data include message's keywords and possible replies to the message.
    Return: 102 if object is not enabled
            202 if dupesCheck = True and found a match to mKeywords in mKeywordsList
            200 if the adding operation succeed
*/
int tTimmy::add(tStrArray mKeywords, tStrArray replies)
{
    if (!enabled)
        return 102;
    for (std::string &iter : mKeywords)
        std::transform(iter.begin(), iter.end(), iter.begin(), ::tolower);
    if (dupesCheck && (nOfEntries > 0))
        for (tStrArray iter : mKeywordsList)
            if (compareStrArrays(iter, mKeywords))
                return 202;

    mKeywordsList.push_back(mKeywords);
    replyList.push_back(replies);
    ++nOfEntries;
    return 200;
}

/*
    add data to bot but this one gets string inputs instead of tStrArray inputs.
    This use strSplit() to split the string inputs (with a space character as the delimiter
    for the message keywords string input and a semicolon character for the replies string input).
    The main work is done by the primary implementation of tTimmy.add().

int add(std::string keywordsStr, std::string repStr);
{
    return (add(strSplit(keywordsStr, ' '), strSplit(repStr, ';')));
}

    Custom delimiters is accepted through default parameters.

    Return: tTimmy.add(mKeywords, replies: tStrArray)
*/

int tTimmy::add(std::string keywordsStr, std::string repStr, char kStrDeli = ' ', char mStrDeli = ';')
{
    return (add(strSplit(keywordsStr, kStrDeli), strSplit(repStr, mStrDeli)));
}

/*
    Given a set of keywords, find matches to that set in mKeywordsList,
    remove the matches, and remove the correspondants in replyList as well.
    This function simply saves offsets of the matching arrays in mKeywordsList
    and then call tTimmy.removeByIndex().

    Return: 102 if object is not enabled
            308 if the operation succeed
*/
int tTimmy::remove(tStrArray mKeywords)
{
    if (!enabled)
        return 102;

    for (std::string iter : mKeywords)
        std::transform(iter.begin(), iter.end(), iter.begin(), ::tolower);
    std::vector<int> indexes(mKeywordsList.size());

    // Get offsets of keywords set that match the given mKeywords parameter
    // and later deal with them using tTimmy.removeByIndex

    for (auto iter = mKeywordsList.begin(); iter != mKeywordsList.end(); ++iter)
        if (compareStrArrays(*iter, mKeywords))
            indexes.push_back(std::distance(mKeywordsList.begin(), iter));

    int counter = indexes.size();
    while (counter > 0)
    {
        tTimmy::remove(indexes[indexes.size() - counter] - indexes.size() + counter);
        --counter;
    }
    return 308;
}

int tTimmy::remove(int aIndex)
{
    if (!enabled)
        return 102;
    if ((aIndex < 0) || (aIndex >= nOfEntries))
        return 305;

    mKeywordsList.erase(std::next(mKeywordsList.begin(), aIndex));
    replyList.erase(std::next(replyList.begin(), aIndex));

    --nOfEntries;
    return 300;
}

/*
    answer the given message, using assets in the metadata
*/
std::string tTimmy::answer(std::string tMessage)
{
    if (!enabled)
        return ("");

    // Define random engine
    std::default_random_engine generator;

    // Pre-process the message
    std::string flagM = strTrim(tMessage);
    std::transform(flagM.begin(), flagM.end(), flagM.begin(), ::tolower);

    // Delete punctuation at the end of the message (like "?" or "!")
    while (!isalnum(flagM.back()))
        flagM.erase(prev(flagM.end()));

    tStrArray flagWords = strSplit(flagM, ' ');
    int counter;
    for (int metaIter = 0; metaIter < nOfEntries; ++metaIter)
    {
        counter = 0;
        for (auto mKIter : mKeywordsList[metaIter])
            for (auto mWIter : flagWords)
                counter += (mWIter == mKIter);
        if (((double)counter / mKeywordsList[metaIter].size()) * 100 >= tPercent)
        {
            auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
            generator.seed(seed);
            std::uniform_int_distribution<int> distribution(0, replyList[metaIter].size() - 1);
            return (replyList[metaIter][distribution(generator)]);
        }
    }
    return (noUdstdRep);
}