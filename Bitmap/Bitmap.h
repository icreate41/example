#ifndef BITMAP_H
#define	BITMAP_H
//------------------------------------------------------------------------------
/*
class for working with memory blocks of the same length located in a single continuous space
provides logarithmic complexity (basis = 32) for all operations (insert, erase, search)
*/
struct Level
{
    int* array;
    Level* next;
};
class Bitmap
{
    int maxsize, cursize;
    int LSB(int val);
    Level* head;
    Level* create(Level* prev, int size);
    bool avalible(Level* current, int index);
    int get_free_index(Level* current, int pos);
    int lock(Level* current, int index);
    int unlock(Level* current, int index);
    int clear_level(Level* cur, int size);
public:
    int insert();
    void insert(int index);
    void erase(int index);
    void clear();
    bool avalible(int index);
    int size();
    Bitmap(int size);
    ~Bitmap();
};
//------------------------------------------------------------------------------
#endif

