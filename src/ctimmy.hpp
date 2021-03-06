/*
    ctimmy.hpp
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
#pragma once

#include <cctype>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <random>
#include <chrono>

// Interface
typedef std::vector<std::string> tStrArray;
typedef std::vector<std::string *> pStrArray;

/*
    Metadata refers to two arrays holding data:
    msgKeywordsList which holds keywords,
    replyList which holds replies, and
    pReplyList which also functions like replyList,
    but holds pointers to replies

      msgKeywordsList [                                 replyList [
                     [*keywords for message 1*],                 [*possible answers for message 1*],
                     [*keywords for message 2*],                 [*possible answers for message 2*],
                                 ...                                             ...                ]
                                 ...                  pStrArray [
                     [*keywords for message n*],                 [*pointer points to answer for message n*]
                     [*keywords of message n + 1*]               [*pointer points to answer for message n + 1*]
                                 ...              ]                               ...                         ]

    Variables (see also the README file):

      enabled            :  Bot's state. If `Enabled` is true, the bot
                            is ready to work and vice versa.
      nOfEntries         :  Number of entries (elements) in msgKeywordsList
      dupesCheck         :  Check for duplicate or not
                            (might be time-saving if we don't check for duplicate)
      tPercent           :  Minimum percentage of the number of keywords over
                            all the words of the message so that the bot object
                            can "understand" and have a reply.
                            (Sorry I don't have a good way to explain it)
      noUdstdRep :          String to assign to timmy.answer() in case
                            there's no possible answer to the given message
*/

class timmy;

class timmy
{
  public:
    bool dupesCheck;
    int tPercent;
    std::string noUdstdRep;

    timmy();
    timmy(int, std::string, bool);
    void enable();
    void disable();
    int add(tStrArray, tStrArray);
    int add(std::string, std::string, std::string = " ", std::string = ";");
    int add(tStrArray, std::string *);
    int add(std::string, std::string *, std::string = " ");
    int remove(tStrArray);
    int remove(size_t);
    int remove(std::string, std::string = " ");
    std::string answer(std::string);

  private:
    bool enabled = false;
    size_t nOfEntries = 0;
    std::vector<tStrArray> msgKeywordsList;
    std::vector<tStrArray> replyList;
    pStrArray pReplyList;

    bool isDupe(tStrArray);
};

std::string strTrim(std::string);
tStrArray strSplit(std::string, std::string);
std::string strJoin(tStrArray, std::string);