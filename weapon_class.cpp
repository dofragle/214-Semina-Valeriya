#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>

using namespace std;

class Opponent{ //Player использует оружие из класса Weapon и наносит урон
                //метод alive позволяет узнать умер ли противник
public:
    int health;
    Opponent(){health=100;}
    bool alive(){
        if (health>0)
            return 1;
        else
            return 0;
    }
};

class Weapon //Всего используется два вида оружия: Пистолет(сlass Gun) и Нож (class Knife)
{
protected:
    int damage;
    int condition;
    char *name; //наименование оружия
    static int counter; //счетчик имеющегося оружия (и ножей и пистолетов)
public:
    explicit Weapon(const char *str){ //конструктор
        damage = 0;
        condition = 0;
        name = new char [strlen(str) + 1];
        strcpy(name, str);
    }
    Weapon(const Weapon &w){ //конструктор копирования
        damage = w.damage;
        condition = w.condition;
        name = new char [strlen(w.name) + 1];
        strcpy(name, w.name);
        }
    ~Weapon(){ //деструктор
        delete[] name;
    }
    void print_amount() const {
        cout << "amount of weapons now: " << counter << endl;
    }

    virtual void use(Opponent &a)=0;

    Weapon& operator=(const Weapon &w){
        if (this != &w)
        {
            damage = w.damage;
            condition = w.condition;

            delete[] name;
            name = new char [strlen(w.name) + 1];
            strcpy(name, w.name);
        }
        return *this;
    }
    int operator()(int n){
        if(n <= condition){
            return(n*damage);
        }
        else
            return 0;
    }
    friend std::ostream& operator<< (std::ostream &out, const Weapon &w);

};

int Weapon::counter = 0;

std::ostream& operator<< (std::ostream &out, const Weapon &w){
    out << "name: " << w.name << endl << "damage: " << w.damage << endl << "condition: " << w.condition << endl;
    return out;
}
class Gun : public Weapon
{
public:
    explicit Gun(const char *str) : Weapon(str) {
        damage = 15;
        condition = 7;
        counter++;
    }
    void use(Opponent &a) override{
        if (condition > 0){
            cout << "gun was used" << endl;
            condition--;
            a.health-=damage;
        }
        else
            cout << "cannot be used" << endl;
    }
};

class Knife : public Weapon
{
public:
    explicit Knife(const char *str) : Weapon(str) {
        damage = 4;
        condition = 15;
        counter++;
    }
    void use(Opponent &a) override{
        if (condition > 5){
            cout << "knife was used" << endl;
            condition-=2;
            a.health-=damage;
        }
        else if (condition > 0){
            cout << "knife was used" << endl;
            condition--;
            a.health-=damage;
        }
        else
            cout << "cannot be used" << endl;
    }
};

class Player{ // Игрок использует оружие из класса Weapon, чтобы нанести урон Opponent
public:
    void hit(Weapon *w, Opponent &a){
        w->use(a);
    }
};


int main()
{
    Player player; Opponent opponent;
    Gun a("carbines");
    Knife b("")
    player.hit(&a, opponent);
    cout << opponent.alive();
    return 0;
}
