#include "tree.h"

PathNode *update_tree(PathNode *node, list<ValueNode*> &watchdog)
{
    bool next = true;
    for(node->to_begin(); node->exist(); next ? node->next():node->erase())
    {
        next = false;
        if     (node->pointer()->base_type() == PathType)
        {
            PathNode *path = update_tree(node->pointer<PathNode>(), watchdog);
            next = path->size();
        }
        else if(node->pointer()->base_type() == ValueType)
        {
            ValueNode *value = node->pointer<ValueNode>();
            next   =   value->is_set();
            if(value->can_get_signal())
                watchdog.push_back(value);            
        }
    }
    return node;
}

bool read_tree_values(fstream &fs, PathNode *node, map<stdstr,int> &filemap)
{
    bool done = true;
    for(node->to_begin(); node->exist(); node->next())
    {
        if     (node->pointer()->base_type() == PathType)
            done &= read_tree_values(fs, node->pointer<PathNode>(), filemap);
        else if(node->pointer()->base_type() == ValueType)
        {
            ValueNode *value = node->pointer<ValueNode>();
            if(value->is_non_volatile())
            {
                map<stdstr,int>::iterator
                i =filemap.find(types[value->type()]+" "+node->pointer()->get_path());
                done &= i != filemap.end();
                if(i != filemap.end())
                {       
                    value->set_offset(i->second);
                    read_value(fs, value);
                }
            }
        }
    }
    return done;
}

void open_directory(PathNode *&node, int offset)
{
    Iterator pos = node->get_iterator();
    if(!node->advance(offset))
        return;
    if(node->pointer()->base_type() == PathType)
    {
        PathNode *path = node->pointer<PathNode>();
        node = path;
        node->to_begin();
        return;
    }
    node->set_iterator(pos);
}
void close_directory(PathNode *&node)
{
    PathNode *path = (PathNode*)node->get_parent();
    node = path;
    node->to_begin();
}