#include <iostream>
#include <string>
 
using namespace std;
 
class Animal {
public:
        Animal(const string& name) : name(name) {}
        virtual string talk() = 0;
        const string name;
};
 
class Cat : public Animal {
public:
        Cat(const string& name) : Animal(name) {}
        virtual string talk() { return "Meow!"; }
};
 
class Dog : public Animal {
public:
        Dog(const string& name) : Animal(name) {}
        virtual string talk() { return "Woof! Woof!"; }
};
 
// prints the following:
//
// Missy: Meow!
// Mr. Mistoffelees: Meow!
// Lassie: Woof! Woof!!
//
int main()
{
        Animal* animals[] = {
                new Cat("Missy"),
                new Cat("Mr. Mistoffelees"),
                new Dog("Lassie")
        };
 
        for (int i = 0; i < 3; ++i) {
                cout << animals[i]->name << ": " << animals[i]->talk() << endl;
                delete animals[i];
        }
}



// vim600:ts=4 st=4 foldmethod=marker foldmarker=<<<,>>>
// vim600:syn=cpp encoding=utf-8 commentstring=//\ %s
