#include "LineParser.h"
//------------------------------------------------------------------------------
ParserType::~ParserType() {}
int ParserType::get(char*& cstr) { return Err; }
//------------------------------------------------------------------------------
CharType::CharType(CheckChar ptr, CharMode m_mode, char m_ch, int m_min, int m_max)
{
    checkChar = ptr;
    mode      = m_mode;
    ch        = m_ch;
    min       = m_min;
    max       = m_max;
}
CharType::~CharType() {};
int CharType::get(char*& cstr)
{
    curr = 0;
    while((checkChar(*cstr) == ch) ^ mode) {
        char next = *cstr ? checkChar(*++cstr) : 0;
        curr++;
        if(curr > max)
            return Err;
        if(curr >= min) {
            if((next != ch) ^ mode or !next)
                return Succsess;
        }
    }
    return min or curr ? Err : Succsess;
}
//------------------------------------------------------------------------------
int LineParser::run(char*& cstr)
{
    startcstr = cstr;
    for(list<Child>::iterator it = childList.begin(); it != childList.end(); it++) {
        if((*it).ptr->get(cstr) == Err)
            return Err;
    }
    if(outstr)
        *outstr = stdstr(startcstr, cstr - startcstr);
    return Succsess;
}
int LineParser::get(char*& cstr)
{       
    return run(cstr);
}
void LineParser::add(CheckChar ptr, CharMode mode, char ch, int min, int max)
{
    childList.push_back(Child(new CharType(ptr, mode, ch, min, max), true));
}
void LineParser::add(const char *cstr)
{
    char next;
    while(next = *cstr++)
        childList.push_back(Child(new CharType(isAll, Eq, next, 1, 1), true));
}
void LineParser::add(LineParser* ptr)
{
    childList.push_back(Child(ptr, false));
}
int LineParser::start(const char* ptr)
{
    outcstr = (char*)ptr;
    return run(outcstr);
}
int LineParser::start(stdstr* str)
{
    outcstr = (char*)str->c_str();
    return run(outcstr);
}
void LineParser::clear()
{
    for(list<Child>::iterator it = childList.begin(); it != childList.end(); it++) {
        if((*it).isChild)
           (*it).ptr->~ParserType();
    }
}
void LineParser::copy(LineParser* ptr)
{
    clear();
    list<Child> *m_list = ptr->getList();
    for(list<Child>::iterator it = m_list->begin(); it != m_list->end(); it++)
        childList.push_back(Child((*it).ptr, false));
}
LineParser::LineParser(LineParser* ptr, stdstr* str)
{
    copy(ptr);
    outstr = str;
    outcstr = NULL;
    startcstr = NULL;
}
LineParser::LineParser(stdstr* str)
{
    outstr = str;
    outcstr = NULL;
    startcstr = NULL;
}
LineParser::LineParser()
{
    outstr = NULL;
    outcstr = NULL;
    startcstr = NULL;
}
const char *LineParser::getCharPtr()
{
    return outcstr;
}
const char *LineParser::getStartPtr()
{
    return startcstr;
}
void LineParser::setString(stdstr* str)
{
    outstr = str;
}
LineParser::~LineParser()
{
    clear();
}
//------------------------------------------------------------------------------