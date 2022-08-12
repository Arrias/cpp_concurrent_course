#include <iostream>

void debug(std::string str) {
    std::cout << str << std::endl;
}

void debug(int value) {
    debug(std::to_string(value));
}

/* dmalloc allocator
 * $ dmalloc -d 0 -l leak.log -p log-non-free
 * $ eval it (i.e. eval `...`)
 * $ sudo ldconfig -p | fgrep dmalloc
 * $ LD_PRELOAD="libdmalloccxx.so"
 * $ LD_PRELOAD="libdmalloc.so"
 *
 * dmalloc -- аллокатор для обнаружения утечек
 * tcmalloc -- хорошо показывает себя на большом количестве выделений памяти + умеет искать утечки
 * jemalloc ~ tcmalloc по производительности
 */

class SmallAllocator {
private:
    static const int _sz = 1024;
    char Memory[_sz]{};
    int used[4 * _sz]{};

public:
    void *Alloc(unsigned int Size) {
        //debug(Size);
        if (Size > _sz) return nullptr;
        int curr = 0;
        for (int i = 0; i < Size - 1; ++i)
            curr += used[i];
        for (int l = 0, r = Size - 1; r < _sz; ++l, ++r) {
            curr += used[r];
            //debug(std::to_string(l) + " " + std::to_string(r) + " " + std::to_string(curr));
            if (!curr && (!l || !used[l - 1]) && !used[r + 1]) {
                while (l <= r) {
                    used[l++] = 1;
                }
                return Memory + l - Size;
            }
            curr -= used[l];
        }
        return nullptr;
    };

    unsigned int getSize(void *Pointer) {
        char *curr = (char *) Pointer;
        int id = curr - Memory;
        int ret = 0;
        while (used[id]) {
            ++ret;
            ++id;
        }
        return ret;
    }

    void *ReAlloc(void *Pointer, unsigned int Size) {
        unsigned int real_size = getSize(Pointer);
        if (Size < real_size) Size = real_size;
        char* ptr = (char * )Alloc(Size);
        char* curr = (char *) Pointer;
        char* ret = ptr;
        if (ptr == nullptr) return ptr;
        int id = curr - Memory;
        while (id < _sz && used[id]) {
            *(ptr++) = *(curr++);
            used[id] = 0;
            id++;
        }
        return ret;
    };

    void Free(void *Pointer) {
        int id = (char *) Pointer - Memory;
        while (id < _sz && used[id]) {
            used[id] = 0;
            ++id;
        }
    };
};

int main(int argc, char **argv) {

    SmallAllocator A1;
    int * A1_P1 = (int *) A1.Alloc(sizeof(int));
    A1_P1 = (int *) A1.ReAlloc(A1_P1, 2 * sizeof(int));
    A1.Free(A1_P1);

    SmallAllocator A2;
    int * A2_P1 = (int *) A2.Alloc(10 * sizeof(int));
    //std::cout << A2_P1 << std::endl;

    for(unsigned int i = 0; i < 10; i++) A2_P1[i] = i;
    for(unsigned int i = 0; i < 10; i++) if(A2_P1[i] != i) std::cout << "ERROR 1" << std::endl;
    int * A2_P2 = (int *) A2.Alloc(10 * sizeof(int));

    //std::cout << A2_P2 << std::endl;
    for(unsigned int i = 0; i < 10; i++) A2_P2[i] = -1;
    for(unsigned int i = 0; i < 10; i++) if(A2_P1[i] != i) std::cout << "ERROR 2" << std::endl;
    for(unsigned int i = 0; i < 10; i++) if(A2_P2[i] != -1) std::cout << "ERROR 3" << std::endl;
    A2_P1 = (int *) A2.ReAlloc(A2_P1, 20 * sizeof(int));
    for(unsigned int i = 10; i < 20; i++) A2_P1[i] = i;
    for(unsigned int i = 0; i < 20; i++) if(A2_P1[i] != i) std::cout << "ERROR 4" << std::endl;
    for(unsigned int i = 0; i < 10; i++) if(A2_P2[i] != -1) std::cout << "ERROR 5" << std::endl;
    A2_P1 = (int *) A2.ReAlloc(A2_P1, 5 * sizeof(int));
    for(unsigned int i = 0; i < 5; i++) if(A2_P1[i] != i) std::cout << "ERROR 6" << std::endl;
    for(unsigned int i = 0; i < 10; i++) if(A2_P2[i] != -1) std::cout << "ERROR 7" << std::endl;
    A2.Free(A2_P1);
    A2.Free(A2_P2);

    return 0;
}