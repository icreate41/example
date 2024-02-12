#include "Bitmap.h"
//------------------------------------------------------------------------------
int Bitmap::LSB(int val) //calculate least significant bit fast
{
    if (val == 0)
        throw std::runtime_error("Could not find LSB");
    int pos = 0;
    if ((val & 0xFFFF) == 0)
    {
        pos += 16;
        val >>= 16;
    }
    if ((val & 0x00FF) == 0)
    {
        pos += 8;
        val >>= 8;
    }
    if ((val & 0x000F) == 0)
    {
        pos += 4;
        val >>= 4;
    }
    if ((val & 0x0003) == 0)
    {
        pos += 2;
        val >>= 2;
    }
    if ((val & 0x0001) == 0)
    {
        pos += 1;
        val >>= 1;
    }
    return pos;
}
Level* Bitmap::create(Level* prev, int size)
{
    size = (size + 31) / 32;
    Level* cur = new Level();
    cur->array = new int[size];
    memset(cur->array, -1, sizeof(int) * size);
    cur->next = prev;
    if (size > 1)
        return create(cur, size);
    return cur;
}
bool Bitmap::avalible(Level* current, int index)
{
    if (current->next)
        return avalible(current->next, index);
    int div = index / 32;
    int mod = index % 32;
    return current->array[div] & (1 << mod);
}
int Bitmap::get_free_index(Level* current, int pos)
{
    pos = pos * 32 + LSB(current->array[pos]);
    if (current->next)
        return get_free_index(current->next, pos);
    return pos;
}
int Bitmap::lock(Level* current, int index)
{
    if (current->next)
        index = lock(current->next, index);
    if (index < 0)
        return index;
    int div = index / 32;
    int mod = index % 32;
    current->array[div] &= ~(1 << mod);
    if (current->array[div] == 0)
        return div;
    return -1;
}
int Bitmap::unlock(Level* current, int index)
{
    if (current->next)
        index = unlock(current->next, index);
    int div = index / 32;
    int mod = index % 32;
    current->array[div] |= (1 << mod);
    return div;
}
int Bitmap::insert()
{
    if (cursize >= maxsize)
        throw std::runtime_error("Bitmap out of range");
    int index = get_free_index(head, 0);
    lock(head, index);
    cursize++;
    return index;
}
void Bitmap::insert(int index)
{
    if (index < 0 || index >= maxsize)
        throw std::runtime_error("Bitmap out of range");
    if (avalible(head, index) == false)
        throw std::runtime_error("Bitmap out of range");
    lock(head, index);
    cursize++;
}
void Bitmap::erase(int index)
{
    if (index < 0 || index >= maxsize)
        throw std::runtime_error("Bitmap out of range");
    if (avalible(head, index) == true)
        throw std::runtime_error("Bitmap out of range");
    unlock(head, index);
    cursize--;
}
bool Bitmap::avalible(int index)
{
    if (index < 0 || index >= maxsize)
        throw std::runtime_error("Bitmap out of range");
    return avalible(head, index);
}
int Bitmap::size()
{
    return cursize;
}
Bitmap::Bitmap(int size)
{
    if (size < 1)
        throw std::runtime_error("Bitmap out of range");
    if (size > 1048576)
        throw std::runtime_error("Bitmap out of range");
    maxsize = size;
    cursize = 0;
    head = create(NULL, maxsize);
}
Bitmap::~Bitmap()
{
    while (head)
    {
        Level* next = head->next;
        delete head->array;
        delete head;
        head = next;
    }
}
int Bitmap::clear_level(Level* cur, int size)
{
    if (cur->next)
        size = clear_level(cur->next, size);
    size = (size + 31) / 32;
    memset(cur->array, -1, sizeof(int) * size);
    return size;
}
void Bitmap::clear()
{
    clear_level(head, maxsize);
    cursize = 0;
}
//------------------------------------------------------------------------------
