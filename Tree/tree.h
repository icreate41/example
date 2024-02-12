#ifndef TREE_H
#define	TREE_H

typedef basic_string<char> stdstr;
typedef map<string*, ShmValue*, string> map_t;
//----------------------------------------------------------------------------------------
stdstr to_string(int val)
{
    char cstr[12];
    sprintf(cstr, "%i", val);
    return stdstr(cstr);
}
//----------------------------------------------------------------------------------------
class BaseTag
{       
protected:
    const char *_name;
    ShmValue   *_shmv;
public:
    BaseTag();
    BaseTag(const char *name, ShmValue *value);
    bool is_set();
    void set(map_t::iterator &it);
    void read  (void *data);
    void write (void *data);
    int  read ();
    void write(int data);
    int  type();
};
class Trigger : public BaseTag
{
    int last;
public:
    Trigger(const char *name, ShmValue *value);
    bool change();
};
//----------------------------------------------------------------------------------------
enum BaseType
{
    ParentType, PathType, ValueType
};
class Base
{
protected:
    BaseType _type;
    stdstr   _name;
    Base    *_parent;
public:    
    Base(const stdstr &str, Base *from);
    Base(const stdstr &str);
    virtual ~Base();
    BaseType base_type();
    const char *get_name();
    stdstr get_directory();
    stdstr get_path();
};
//----------------------------------------------------------------------------------------
struct BaseCmp
{
    bool operator()(Base * a, Base * b);
};
//----------------------------------------------------------------------------------------
class Iterator
{
protected:
    set<Base*>::const_iterator _it;
    int _pos;
    Iterator _get_iterator();
    void     _set_iterator(Iterator &it);
};
//----------------------------------------------------------------------------------------
class PathNode : public Base, Iterator
{   
    set<Base*, BaseCmp> *_nodes;
    static PathNode _dummy;
public:
    static Base *get_dummy(const stdstr &str)
    {
        _dummy._name = str;
        return &_dummy;
    }
    PathNode(const stdstr &str, Base *from);
    PathNode(const stdstr &str);
    ~PathNode();
    void to_begin();
    void next();
    void prev();
    void erase();
    bool advance(int offset);
    bool exist();
    int  size();
    int  position();
    Base *pointer();
    Base *get_parent();
    Iterator get_iterator();
    void set_iterator(Iterator &it);
template<class T> 
T *pointer()
{   
    Base *obj = dynamic_cast<T*>(pointer());
    if(!obj)
        throw std::runtime_error("Unable to cast *Base");
    return (T*)obj;
}
template<class T> 
bool find(stdstr &str)
{
    Base *obj = T::get_dummy(str);
    _it  = _nodes->find(obj);
    _pos = -1;
    return exist();
}
template<class T> 
T *insert(stdstr &str)
{
    if(find<T>(str))
        return pointer<T>();
    Base *obj = (Base*)(new T(str, this));
    _it  = _nodes->insert(obj).first;
    _pos = -1;
    return (T*)obj;
}
};
PathNode PathNode::_dummy("");
//----------------------------------------------------------------------------------------
class ValueNode : public Base
{
    BaseTag _nv_tag, _val_tag, _read_tag, _input_tag, _write_tag, _output_tag;
    int _offset, _read_val, _write_val;
    static ValueNode _dummy;
public:
    static Base *get_dummy(const stdstr &str)
    {
        _dummy._name = str;
        return &_dummy;
    }
    ValueNode(const stdstr &str, Base *from);
    ValueNode(const stdstr &str);
    bool is_set();
    bool is_volatile();
    bool is_non_volatile();
    bool can_get_signal();
    bool can_set_signal();
    int  type();
    int  get_offset();
    void set_offset(int value);
    void read (void *data);
    int  read ();
    void write(void *data);
    bool get_signal(void *data);
    void set_signal(void *data);
    void set_attr(stdstr &attr, map_t::iterator &it, map_t *reg);
};
ValueNode ValueNode::_dummy("");
//----------------------------------------------------------------------------------------
#endif	/* TREE_H */

