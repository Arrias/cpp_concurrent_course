#include <iostream>
#include <memory>
#include <cassert>

void debug(std::string str) {
    std::cout << str << std::endl;
}

void debug(int value) {
    debug(std::to_string(value));
}

class StringPointer {
public:
    std::string *operator->() {
        if (ptr == nullptr) ptr = new std::string("");
        return ptr;
    }

    std::string operator*() {
        if (ptr == nullptr) ptr = new std::string("");
        return *ptr;
    }

    StringPointer(std::string *Pointer) : ptr(Pointer) {}

    ~StringPointer() {
        if (ptr != nullptr) {
            delete ptr;
            ptr = nullptr;
        }
    }

private:
    std::string *ptr;
};

class scoped_ptr {
    std::string *myPointer;

    scoped_ptr(const scoped_ptr &p);

    const scoped_ptr &operator=(const scoped_ptr &p);

public:
    scoped_ptr(std::string *ptr) : myPointer(ptr) {}

    ~scoped_ptr() {
        if (myPointer != nullptr) {
            delete myPointer;
        }
    };

    std::string *ptr() const {
        return myPointer;
    }

    std::string &operator*() const {
        return *myPointer;
    }

    std::string *operator->() const {
        return myPointer;
    }

    bool isNull() const {
        return myPointer != nullptr;
    }
};

void panic() {
    std::string some_string = "some_string";

    //scoped_ptr ptr(&some_string); // double corruption

    scoped_ptr nice_ptr(new std::string("lol"));
}

void null_test() {
    std::shared_ptr<int> sharedPtr(nullptr);
    sharedPtr.reset();
}

struct Detonator {
    ~Detonator() {
        std::cout << "BLOW UP!" << std::endl;
    }
};

struct Batman {
    std::shared_ptr<Batman> other_batman;
    int *array;

    Batman(Batman *batman) : other_batman(batman) {
        array = new int[5];
    }

    ~Batman() {
        std::cout << "Batman destructor" << std::endl;

        // other_batman.reset();
        // вызывается деструктор other_batman
        delete[] array;
    }
};

void reset_ub_example_1() {
    std::shared_ptr<Detonator> p1(new Detonator());
    std::shared_ptr p2 = p1;

    debug(p1.use_count());

    std::shared_ptr<Detonator> p3(new Detonator());

    debug(p3.use_count());
    p1.reset(p3.get()); // UB! <- reset(link) is UB if somebody owns link
    debug("after reset:");
    debug(p1.use_count());
    debug(p2.use_count());
    debug(p3.use_count());
}

void reset_ub_example_2() {
    Batman *batman1 = new Batman(new Batman(nullptr));
    Batman *batman2 = new Batman(new Batman(nullptr));

    batman1->other_batman = batman2->other_batman;
    //batman1->other_batman.reset(batman2->other_batman.get());

    debug("hello");
    assert(batman1->other_batman.get() == batman2->other_batman.get());
    debug(batman1->other_batman.use_count());

    delete batman1;
    delete batman2;
}

struct List {
    List() {}
    std::shared_ptr<List> next_node{};
};

void cyclic_references_example() {
    // create cyclic 1-linked list
    std::shared_ptr<List> firstNode(new List());
    std::shared_ptr<List> secondNode(new List());
    firstNode->next_node = secondNode;
    secondNode->next_node = firstNode;

//    {
//        // yes, but memory leak, because here are links to first and second node yet
//        firstNode.reset();
//        secondNode.reset();
//    }

    // maybe solution but ...
    {
        firstNode->next_node.reset();
        secondNode->next_node.reset();
    }
}

struct B;

struct A {
    std::weak_ptr<B> b;
    ~A() {
        debug("~A()");
    }
};

struct B {
    std::shared_ptr<A> a;
    ~B() {
        debug("~B()");
    }
};

// its works
void weak_ptr_solution() {
    std::shared_ptr<B> b(new B());
    std::shared_ptr<A> a(new A());
    a->b = b;
    b->a = a;
    // regardless of the order of call destructors of shared_ptr a, b all is ok
    // (!) but it can change the order of A, B destructor calls
}

int main() {
    cyclic_references_example();
    weak_ptr_solution();
    cyclic_references_example();
    panic();

    std::string s1 = "Hello, world!";

    StringPointer sp1(new std::string("hello world!"));
    StringPointer sp2(nullptr);

    std::cout << sp1->length() << std::endl;
    std::cout << *sp1 << std::endl;
    std::cout << sp2->length() << std::endl;
    std::cout << *sp2 << std::endl;
    return 0;
}
