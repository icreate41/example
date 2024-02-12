#include "tree.h"
//----------------------------------------------------------------------------------------
BaseTag::BaseTag()
{
    _name = NULL;
    _shmv = NULL;
}
BaseTag::BaseTag(const char *name, ShmValue *value)
{
    _name = name;
    _shmv = value;
}
bool BaseTag::is_set()
{
    return (_name and _shmv);
}
void BaseTag::set(map_t::iterator &it)
{
    if(is_set())
        throw std::runtime_error("Double set BaseTag error");
    _name = it->first->c_str();
    _shmv = it->second;
}
void BaseTag::read(void *data)
{
    if(!is_set())
        throw std::runtime_error("Unable to read non-existent BaseTag");
    if(!mem.get(_shmv->offset(), data, sizes[_shmv->type()]))
        throw std::runtime_error("Error while reading BaseTag");
}
void BaseTag::write(void *data)
{
    if(!is_set())
        throw std::runtime_error("Unable to write non-existent BaseTag");
    if(mem.set(_shmv->offset(), data, sizes[_shmv->type()]) != sizes[_shmv->type()])
        throw std::runtime_error("Error while writing BaseTag");
}
int  BaseTag::read()
{
    int tmp = 0;
    read(&tmp);
    if(type() == BOOL)
        return *((char*)&tmp);
    if(type() == SHORT)
        return *((short*)&tmp);
    return tmp;
}
void BaseTag::write(int data)
{
    write(&data);
}
int BaseTag::type()
{
    if(!is_set())
        throw std::runtime_error("Unable to get_type of non-existent BaseTag");
    return _shmv->type();
}
//----------------------------------------------------------------------------------------
Trigger::Trigger(const char *name, ShmValue *value)
{
    _name = name;
    _shmv = value;
    last  = read();
}
bool Trigger::change()
{
    int tmp  = read();
    if (last!= tmp)
    {
        last = tmp;
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------------------
Base::Base(const stdstr &str, Base *from)
{
    _type   = ParentType;
    _name   = str;
    _parent = from;
}
Base::Base(const stdstr &str) : Base(str, this) {}
Base::~Base() {}
BaseType  Base::base_type()
{
    return _type;
}
const char *Base::get_name()
{
    return _name.c_str();
}
stdstr Base::get_directory()
{
    if(_parent != this)
        return _parent->get_directory() + _name  + "/";
    return _name;
}
stdstr Base::get_path()
{
    return _parent->get_directory() + _name;
}
bool BaseCmp::operator()(Base * a, Base * b)
{
    if(a->base_type() != b->base_type())
        return a->base_type() < b->base_type();
    return strcmp(a->get_name(), b->get_name()) < 0;
}
//----------------------------------------------------------------------------------------
ValueNode::ValueNode(const stdstr &str, Base *from) : Base(str, from)
{
    _type   = ValueType;
    _offset = -1;
}
ValueNode::ValueNode(const stdstr &str) : ValueNode(str, this) {}
bool ValueNode::is_set()
{
    return _nv_tag.is_set() xor _val_tag.is_set();
}
bool ValueNode::is_volatile()
{
    return is_set() and _val_tag.is_set();
}
bool ValueNode::is_non_volatile()
{
    return is_set() and _nv_tag.is_set();
}
bool ValueNode::can_get_signal() //Write Output for non-volatile
{
    return is_non_volatile() and _write_tag.is_set() and _output_tag.is_set();
}
bool ValueNode::can_set_signal() //Read Input
{
    //return is_set() and _read_tag.is_set() and _input_tag.is_set();
    return is_set() and _read_tag.is_set();
}
int ValueNode::type()
{
    if(is_non_volatile())
        return _nv_tag.type();
    if(is_volatile())
        return _val_tag.type();
    throw std::runtime_error("Unable to get_type of non-existent ValueNode");
}
int ValueNode::get_offset()
{
    if(_offset < 0)
        throw std::runtime_error("Unable to get_offset of ValueNode");
    return _offset;
}
void ValueNode::set_offset(int value)
{
    _offset = value;
}
int  ValueNode::read()
{
    if(is_non_volatile())
        return _nv_tag.read();
    if(is_volatile())
        return _val_tag.read();
    throw std::runtime_error("Unable to read non-existent ValueNode");
}
void ValueNode::read(void *data)
{
    if(is_non_volatile())
        return _nv_tag.read(data);
    if(is_volatile())
        return _val_tag.read(data);
    throw std::runtime_error("Unable to read non-existent ValueNode");
}
void ValueNode::write(void* data)
{
    if(is_non_volatile())
        return _nv_tag.write(data);
    if(is_volatile())
        return _val_tag.write(data);
    throw std::runtime_error("Unable to write non-existent ValueNode");
}
bool ValueNode::get_signal(void *data)
{
    if(!can_get_signal())
        throw std::runtime_error("Unable to get_signal of ValueNode");
    int tmp = _write_tag.read();
    if(_write_val != tmp)
    {
        _write_val = tmp;
        _output_tag.read(data);
        return true;
    }
    return false;
}
void ValueNode::set_signal(void *data)
{
    if(!can_set_signal())
        throw std::runtime_error("Unable to set_signal of ValueNode");
    if( _input_tag.is_set())
        _input_tag.write(data);
    _read_tag .write(++_read_val);
}
void ValueNode::set_attr(stdstr &attr, map_t::iterator &it, map_t *reg)
{
    if     (attr == "N")
    {
        if(reg == &mem.map_coil or reg == &mem.map_holdreg)
            _nv_tag.set(it);
    }
    else if(attr == "V")
    {
        if(reg == &mem.map_status or reg == &mem.map_inreg)
            _val_tag.set(it);
    }
    else if(attr == "R")
    {
        if(reg == &mem.map_coil or reg == &mem.map_holdreg)
        {
            _read_tag.set(it);
            _read_val = _read_tag.read();
        }
    }
    else if(attr == "I")
    {
        if(reg == &mem.map_coil or reg == &mem.map_holdreg)
            _input_tag.set(it);
    }
    else if(attr == "W")
    {
        if(reg == &mem.map_status or reg == &mem.map_inreg)
        {
            _write_tag.set(it);
            _write_val = _write_tag.read();
        }
    }
    else if(attr == "O")
    {
        if(reg == &mem.map_status or reg == &mem.map_inreg)
            _output_tag.set(it);
    }
}
//----------------------------------------------------------------------------------------
Iterator Iterator::_get_iterator()
{
    return *this;
}
void Iterator::_set_iterator(Iterator &it)
{
    *this = it;
}
//----------------------------------------------------------------------------------------
PathNode::PathNode(const stdstr &str, Base *from) : Base(str, from)
{
    _type  = PathType;
    _nodes = new set<Base*, BaseCmp>();
    _it    = _nodes->end();
    _pos   = -1;
}
PathNode::PathNode(const stdstr &str) : PathNode(str, this) {}
PathNode::~PathNode()
{
    to_begin();
    while(exist())
        erase();
    delete _nodes;
}
void PathNode::to_begin()
{
    _it  = _nodes->begin();
    _pos = 0;
}
void PathNode::next()
{
    if(_it != _nodes->end())
    {
        ++_it;
        ++_pos;
    }
}
void PathNode::prev()
{
    if(_it != _nodes->begin())
    {
        --_it;
        --_pos;
    }
}
void PathNode::erase()
{
    if(!exist())
        throw std::runtime_error("Unable to erase PathNode");
    delete (*_it);
    _nodes->erase(_it++);
}
bool PathNode::advance(int offset)
{
    if(offset < 0 - position())
        return false;
    if(offset >= size() - position())
        return false;
    std::advance(_it, offset);
    _pos += offset;
    return true;
}
bool PathNode::exist()
{
    return _it != _nodes->end();
}
int PathNode::size()
{
    return _nodes->size();
}
int PathNode::position()
{
    if( _pos < 0)
        _pos = std::distance(_nodes->begin(), _it);
    return _pos;
}
Base *PathNode::pointer()
{
    if(!exist())
        throw std::runtime_error("Unable to get pointer of PathNode");
    return *_it;
}
Base *PathNode::get_parent()
{
    return _parent;
}
Iterator PathNode::get_iterator()
{
    return _get_iterator();
}
void PathNode::set_iterator(Iterator &it)
{
    _set_iterator(it);
}
//----------------------------------------------------------------------------------------