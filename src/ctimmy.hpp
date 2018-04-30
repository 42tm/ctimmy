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
typedef std::vector<std::string> TStrArray;

/*
    Metadata refers to two arrays holding data:
    MKeywordsList which holds keywords, and
    ReplyList which holds replies

      MKeywordsList [                                 ReplyList [
                     [*keywords for message 1*],                [*possible answers for message 1*],
                     [*keywords for message 2*],                [*possible answers for message 2*],
                                 ...                                             ...
                                                 ]                                                   ]

    Variables (see also the README file):

      Initialized        : State of initialization
      Enabled            : Acts like Initialized but used in fewer number of functions
      NOfEntries         : Number of entries (elements) in MKeywordsList or ReplyList
      DupesCheck         : Check for duplicate or not (might be time-saving if we don't check for duplicate)
      TPercent           : Minimum percentage of the number of keywords over all the words of the message
                           so that the bot object can "understand" and have a reply.
                           (Sorry I don't have a good way to explain it)
      NoUdstdRep : String to assign to TTimmy.Answer in case there's no possible answer to the given message
*/

class TTimmy;

class TTimmy
{

  public:
    bool Enabled = false;
    bool DupesCheck;
    int TPercent;
    std::string NoUdstdRep;

    int Init();
    int Add(TStrArray MKeywords, TStrArray Replies);
    // int Add(std::string KeywordsStr, std::string RepStr);
    int Add(std::string KeywordsStr, std::string RepStr, char KStrDeli, char MStrDeli);
    int Remove(TStrArray MKeywords);
    int Remove(int AIndex);
    std::string Answer(std::string TMessage);


  private:
    bool Initialized = false;
    int NOfEntries;
    std::vector<TStrArray> MKeywordsList;
    std::vector<TStrArray> ReplyList;

    // Deprecated
    // void Update();
};

std::string StrTrim(std::string s);
TStrArray StrSplit(std::string s, char delimiter);
bool CompareStrArrays(TStrArray ArrayA, TStrArray ArrayB);

/*
    Given a string, process it so that the first and the last
    character are not space, and there is no multiple spaces
    character in a row.
*/
std::string StrTrim(std::string s)
{
    bool SpaceOn;
    while (s.front() == ' ')
        s.erase(s.begin());
    while (s.back() == ' ')
        s.erase(std::prev(s.end()));
    std::string FlagStr;
    for (char iter : s)
        if (iter != ' ')
        {
            FlagStr += iter;
            SpaceOn = false;
        }
        else
            switch (SpaceOn)
            {
            case true:
                continue;
            case false:
                FlagStr += ' ';
                SpaceOn = true;
            }
    return (FlagStr);
}
/*
    Given a string, split the string using the delimiter
    and return an array containing the seperated strings.
*/
TStrArray StrSplit(std::string s, char delimiter)
{
    TStrArray splited;
    s += delimiter;
    std::string FlagStr;
    for (char iter : s)
        if (iter != delimiter)
            FlagStr += iter;
        else
        {
            if (FlagStr.empty())
                continue;
            splited.push_back(FlagStr);
            FlagStr.clear();
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
bool CompareStrArrays(TStrArray ArrayA, TStrArray ArrayB)
{
    if (ArrayA.size() != ArrayB.size())
        return false;
    for (size_t iter = 0; iter < ArrayA.size(); ++iter)
        if (ArrayA[iter] != ArrayB[iter])
            return false;
    return true;
}

/*
    Initialize object with some default values set.
    Return 101 if object is initialized, 100 otherwise.
*/
int TTimmy::Init()
{
    if (Initialized)
        return 101;

    DupesCheck = true;
    NoUdstdRep = "Sorry, I didn't get that";
    TPercent = 70;
    NOfEntries = 0;
    Enabled = true;
    Initialized = true;
    return 100;
}

/*
    Add data to bot object's metadata base.
    Data include message's keywords and possible replies to the message.
    Return: 102 if object is not initialized or enabled
            202 if DupesCheck = True and found a match to MKeywords in MKeywordsList
            200 if the adding operation succeed
*/
int TTimmy::Add(TStrArray MKeywords, TStrArray Replies)
{
    if (!Initialized || !Enabled)
        return 102;
    for (std::string &iter : MKeywords)
        std::transform(iter.begin(), iter.end(), iter.begin(), ::tolower);
    if (DupesCheck && (NOfEntries > 0))
        for (TStrArray iter : MKeywordsList)
            if (CompareStrArrays(iter, MKeywords))
                return 202;

    MKeywordsList.push_back(MKeywords);
    ReplyList.push_back(Replies);
    ++NOfEntries;
    return 200;
}

/*
    Add data to bot but this one gets string inputs instead of TStrArray inputs.
    This use StrSplit() to split the string inputs (with a space character as the delimiter
    for the message keywords string input and a semicolon character for the replies string input).
    The main work is done by the primary implementation of TTimmy.Add().

int Add(std::string KeywordsStr, std::string RepStr);
{
    return (Add(StrSplit(KeywordsStr, ' '), StrSplit(RepStr, ';')));
}

    Custom delimiters is accepted through default parameters.

    Return: TTimmy.Add(MKeywords, Replies: TStrArray)
*/

int TTimmy::Add(std::string KeywordsStr, std::string RepStr, char KStrDeli = ' ', char MStrDeli = ';')
{
    return (Add(StrSplit(KeywordsStr, KStrDeli), StrSplit(RepStr, MStrDeli)));
}

/*
    Given a set of keywords, find matches to that set in MKeywordsList,
    remove the matches, and remove the correspondants in ReplyList as well.
    This function simply saves offsets of the matching arrays in MKeywordsList
    and then call TTimmy.RemoveByIndex().

    Return: 102 if object is not initialized or not enabled
            308 if the operation succeed
*/
int TTimmy::Remove(TStrArray MKeywords)
{
    if (!Initialized || !Enabled)
        return 102;

    for (std::string iter : MKeywords)
        std::transform(iter.begin(), iter.end(), iter.begin(), ::tolower);
    std::vector<int> Indexes(MKeywordsList.size());

    // Get offsets of keywords set that match the given MKeywords parameter
    // and later deal with them using TTimmy.RemoveByIndex
    
    for (auto iter = MKeywordsList.begin(); iter != MKeywordsList.end(); ++iter)
        if (CompareStrArrays(*iter, MKeywords))
            Indexes.push_back(std::distance(MKeywordsList.begin(), iter));

    int counter = Indexes.size();
    while (counter > 0)
    {
        TTimmy::Remove(Indexes[Indexes.size() - counter] - Indexes.size() + counter);
        --counter;
    }
    return 308;
}

int TTimmy::Remove(int AIndex)
{
    if (!Initialized || !Enabled)
        return 102;
    if ((AIndex < 0) || (AIndex >= NOfEntries))
        return 305;

    MKeywordsList.erase(std::next(MKeywordsList.begin(), AIndex));
    ReplyList.erase(std::next(ReplyList.begin(), AIndex));

    --NOfEntries;
    return 300;
}

/*
    Answer the given message, using assets in the metadata
*/
std::string TTimmy::Answer(std::string TMessage)
{
    if (!Initialized || !Enabled)
        return ("");

    // Define random engine
    std::default_random_engine generator;

    // Pre-process the message
    std::string FlagM = StrTrim(TMessage);
    std::transform(FlagM.begin(), FlagM.end(), FlagM.begin(), ::tolower);

    // Delete punctuation at the end of the message (like "?" or "!")
    while (!isalnum(FlagM.back()))
        FlagM.erase(prev(FlagM.end()));

    TStrArray FlagWords = StrSplit(FlagM, ' ');
    int counter;
    for (int MetaIter = 0; MetaIter < NOfEntries; ++MetaIter)
    {
        counter = 0;
        for (auto MKIter : MKeywordsList[MetaIter])
            for (auto MWIter : FlagWords)
                counter += (MWIter == MKIter);
        if (((double)counter / MKeywordsList[MetaIter].size()) * 100 >= TPercent)
        {
            auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
            generator.seed(seed);
            std::uniform_int_distribution<int> distribution(0, ReplyList[MetaIter].size() - 1);
            return (ReplyList[MetaIter][distribution(generator)]);
        }
    }
    return (NoUdstdRep);
}