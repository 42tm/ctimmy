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
// Interface
#include "ctimmy.hpp"

// Implementation
/*
    Given a string, process it so that the first and the last
    character are not space, and there is no multiple spaces
    character in a row.
*/
std::string strTrim(std::string str)
{
    while (str.back() == ' ')
        str.pop_back();
    while (str.front() == ' ')
        str.erase(str.begin());
    auto iter = str.find("  ");
    while ((iter = str.find("  ")) != std::string::npos)
        str.erase(iter);
    return str;
}

/*
    Given a string, split the string using the delimiter
    and return an array containing the separated strings.
    If no delimiter delimiter is found in string s,
    a tStrArray of only one value is returned, and that
    only one value is the original string s.
*/
tStrArray strSplit(std::string s, std::string delimiter)
{
    tStrArray splited;
    std::string token;
    if (s.size() < delimiter.size())
    {
        splited.push_back(s);
        return (splited);
    }
    size_t pos, last = 0;
    s += delimiter;
    while ((pos = s.find(delimiter, last)) != std::string::npos)
    {
        token = s.substr(last, pos - last);
        if (!token.empty())
            splited.push_back(token);
        last = pos + 1;
    }
    return (splited);
}

/*
    Given an array of string, join them using Linker.
    timmy::strJoin({"this", "is", "an", "example"}, "@@");
      -> "this@@is@@an@@example"
*/
std::string strJoin(tStrArray strList, std::string linker)
{
    for (std::string &iter : strList)
        iter += linker;
    std::string joined = std::accumulate(strList.begin(), strList.end(), std::string(""));
    joined.erase(std::prev(joined.end(), linker.size()), joined.end());
    return joined;
}

/*
    Class implementation
*/
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

/*
    Construct class with custom values set.
*/
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
    Check if given keywords clue is a duplicate of one
    that is already presented in MsgKeywordsList.
    Return true if duplication check is enabled and
    a duplicate is found, false otherwise.
*/
bool timmy::isDupe(tStrArray checkMsgKeywords)
{
    if (!dupesCheck || nOfEntries == 0)
        return false;

    for (tStrArray &iter : msgKeywordsList)
    {
        if (iter == checkMsgKeywords)
            return true;
    }
    return false;
}

/*
    Add data to bot object's metadata base.
    Data include message's keywords and possible replies to the message.
    *** PRIMARY ADD FUNCTION ***

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
            if (iter == msgKeywords)
                return 202;

    msgKeywordsList.push_back(msgKeywords);
    replyList.push_back(replies);
    ++(this->nOfEntries);
    return 200;
}

/*
    Add data, but this one takes strings instead of tStrArray.
    The strings are delimited using delimiters to create tStrArray,
    and these tStrArray are then passed to the primary timmy::add().

    Return: timmy::add(tStrArray, tStrArray);
*/
int timmy::add(std::string keywordsStr, std::string repStr, std::string kStrDeli, std::string mStrDeli)
{
    return (this->add(strSplit(keywordsStr, kStrDeli), strSplit(repStr, mStrDeli)));
}

/*
    Add data, takes tStrArray for keywords clue and a pointer which
    points to the possible answer for the messages that contain the keywords.

    Return: 102 if the bot is not enabled
            202 if dupes check is enabled and a duplication is found
            203 if the operation is successful
*/
int timmy::add(tStrArray msgKeywords, std::string *pAnswer)
{
    if (!this->enabled)
        return 102;
    if (isDupe(msgKeywords))
        return 202;

    ++(this->nOfEntries);
    msgKeywordsList.push_back(msgKeywords);
    pReplyList.push_back(pAnswer);

    return 203;
}

/*
    Functions like the above one but takes string instead of tStrArray.
    The string is delimited using a delimiter to create a tStrArray,
    and the rest is for timmy::add(tStrArray, pStr)

    Return: timmy::add(tStrArray, pStr)
*/
int timmy::add(std::string keywordsStr, std::string *pAnswer, std::string kStrDeli)
{
    return (this->add(strSplit(keywordsStr, kStrDeli), pAnswer));
}

/*
    Given a set of keywords, find matches to that set in msgKeywordsList,
    remove the matches, and remove the correspondants in replyList as well.
    This function simply saves offsets of the matching arrays in msgKeywordsList
    and then call timmy::remove(size_t).

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
    // and later deal with them using timmy::remove(size_t)

    for (auto iter = msgKeywordsList.begin(); iter != msgKeywordsList.end(); ++iter)
        if (*iter == msgKeywords)
            indexes.push_back(std::distance(msgKeywordsList.begin(), iter));

    int counter = indexes.size();
    while (counter > 0)
    {
        this->remove(indexes[indexes.size() - counter] - indexes.size() + counter);
        --counter;
    }
    return 308;
}

/*
    The same as the above implementation of `timmy::remove()`, but allows
    use of custom string delimiter.

    Return timmy::remove(tStrArray);
*/
int timmy::remove(std::string keywordsStr, std::string kStrDeli)
{
    return (this->remove(strSplit(keywordsStr, kStrDeli)));
}

/*
    Remove data from msgKeywordsList at msgKeywordsList[aIndex]
    and answer(s) corresponding to the keywords at that offset.

    Return: 102 if object is not enabled
            305 if the given index is invalid (out of bound)
            300 if operation successful
*/
int timmy::remove(size_t aIndex)
{
    if (!enabled)
        return 102;
    if (aIndex >= nOfEntries)
        return 305;

    msgKeywordsList.erase(std::next(msgKeywordsList.begin(), aIndex));
    if (aIndex < replyList.size())
    {
        replyList.erase(std::next(replyList.begin(), aIndex));
    }
    else
    {
        pReplyList.erase(std::next(pReplyList.begin(), aIndex - replyList.size()));
    }

    --(this->nOfEntries);
    return 300;
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
        flagM.pop_back();

    tStrArray flagWords = strSplit(flagM, " ");
    size_t counter, maxMatch = 0;
    bool isMatch = false;
    for (size_t metaIter = 0; metaIter < nOfEntries; ++metaIter)
    {
        counter = 0;

        // Iterate over each keyword in each array in msgKeywordsList
        for (auto mKIter : msgKeywordsList[metaIter])
            for (auto mWIter : flagWords)
                counter += (mWIter == mKIter);

        // Compare to tPercent
        if ((1.0 * counter / msgKeywordsList[metaIter].size()) * 100 >= tPercent)
        {
            maxMatch = metaIter;
            isMatch = true;
        }
    }
    // Case: Not understood
    if (!isMatch)
        return (noUdstdRep);

    // Case: Understood
    if (maxMatch < replyList.size())
    {
        auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        generator.seed(seed);
        std::uniform_int_distribution<int> distribution(0, replyList[maxMatch].size() - 1);
        return (replyList[maxMatch][distribution(generator)]);
    }
    else
        return (*pReplyList[maxMatch - replyList.size()]);
    return (noUdstdRep);
}