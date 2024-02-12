#ifndef LINEPARSER_H
#define	LINEPARSER_H

typedef basic_string<char> stdstr;
//------------------------------------------------------------------------------
char isAll(unsigned char ch)
{
    return ch;
}
char isRegular(unsigned char ch)
{
    return isspace(ch) ? 0 : ch;
}
char isShmTag(unsigned char ch)
{
    return isspace(ch) or ch == '!' ? 0 : ch;
}
char isDigit(unsigned char ch)
{
    return ch >= '0' and ch <= '9' ? ch : 0;
}
char isHex(unsigned char ch)
{
    return (ch >= '0' and ch <= '9') or (ch >= 'A' and ch <= 'F') ? ch : 0;
}
char isUpper(unsigned char ch)
{
    return ch >= 'A' and ch <= 'Z' ? ch : 0;
}
char isSpace(unsigned char ch)
{
    return isspace(ch) ? ch : 0;
}
//------------------------------------------------------------------------------
class ParserType
{
public:
    enum ReturnCode { Err, Succsess };
    enum CharMode   { Eq, NotEq };
    virtual ~ParserType();
    virtual int get(char *&cstr);
protected:
    typedef char(*CheckChar)(unsigned char ch);
    CheckChar checkChar;
};
class CharType : public ParserType
{
    char ch;
    int curr, min, max;
    CharMode mode;
public:
    ~CharType();
    CharType(CheckChar ptr, CharMode m_mode, char m_ch, int m_min, int m_max);
    int get(char *&cstr);
};
class LineParser : public ParserType
{
    struct Child
    {
        ParserType *ptr;
        bool isChild;
        Child(ParserType *m_ptr, bool m_isChild)
        {
            ptr = m_ptr;
            isChild = m_isChild;
        }
    };
    list<Child> childList;
    stdstr *outstr;
    char   *outcstr, *startcstr;
    int get(char *&cstr);
    int run(char *&cstr);
    list<Child> *getList() { return &childList; }
public:
    ~LineParser();
    LineParser();
    LineParser(stdstr *str);
    LineParser(LineParser *ptr, stdstr *str);
    void add(CheckChar ptr, CharMode mode, char ch, int min, int max);
    void add(const char *cstr);
    void add(LineParser *ptr);
    int start(const char *ptr);
    int start(stdstr *str);
    void setString(stdstr *str);
    void copy(LineParser *ptr);
    void clear();
    const char *getCharPtr();
    const char *getStartPtr();
};
//------------------------------------------------------------------------------
#endif

