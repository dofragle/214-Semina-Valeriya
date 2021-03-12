// реализация класса стек и волнового алгоритма для поиска кратчайшего пути в матрице с заданными координатами старта и финиша

#include <iostream>
using namespace std;

struct data {int str; int stl;};
class Stack
{
  public:
    Stack() : root (nullptr) {} //по умолчанию стек пуст
    void push(data x) //добавление в стек
    {
        Node* newnode = new Node;
        newnode->dt.str = x.str;
        newnode->dt.stl = x.stl;
        newnode->next = root;
        root = newnode;
    }
    bool empty() const //проверка на пустоту
    {
        return root == nullptr; 
    }
    const data& top()const //вершина стека
    {
        if (empty())
        {
            throw length_error("stack is empty");
        }
        return root->dt;
    }
    data pop() //удаление из стека
    {
        if (empty())
        {
            throw length_error("stack is empty");
        }
        Node * delnode = root;
        data x = delnode->dt;
        root = delnode->next;
        delete delnode;
        return x;
    }
    
    ~Stack() //деструктор
    {
        while (!empty())
        {
            pop();
        }
    }
  private:
    struct Node //вспомогательная структура
    {
        data dt; 
        Node * next; 
    };
    Node* root;
};

int main()
{
    int n;
    cout << "Введите размер лабиринта"<< endl;
    cin >> n;
    int **labi = new int*[n]; // выделение памяти для массива
    for ( int i = 0; i < n; i++) 
        labi[i] = new int[n];
        
    cout << "Введите лабиринт, где 0 - закрыто, а 1 - открыто" << endl;
    for (int i = 0; i < n; i++) 
    {
        for (int j = 0; j < n; j++)
        {
            cin >> labi[i][j];
            labi[i][j]--;
            labi[i][j]--;
        }
    }
    
    cout << "Введите координаты старта. Примечание: нумерация начинается с нуля" << endl;
    data start, finish;
    cin >> start.str >> start.stl;
    
    cout << "Введите координаты финиша. Примечание: нумерация начинается с нуля" << endl;
    cin >> finish.str >> finish.stl;
    
    const int WALL   = -2;         // непроходимая ячейка
    const int BLANK  = -1;         // свободная непомеченная ячейка
    
    int dx[4] = {1, 0, -1, 0};   // смещения, соответствующие соседям ячейки
    int dy[4] = {0, 1, 0, -1};   // справа, снизу, слева и сверху
    int d, x, y, k;
    bool stop;

    if (labi[start.str][start.stl] == WALL || labi[finish.str][finish.stl] == WALL) 
        cout << "no way!";  // ячейка старта или финиша - стена
    else
    {
        // распространение волны
        d = 0;
        labi[start.str][start.stl] = 0;            // стартовая ячейка помечена 0
        do 
        {
            stop = true;               // предполагаем, что все свободные клетки уже помечены
            for ( y = 0; y < n; ++y )
                for ( x = 0; x < n; ++x )
                    if ( labi[x][y] == d )                         // ячейка (x, y) помечена числом d
                    {
                        for ( k = 0; k < 4; ++k )                    // проходим по всем непомеченным соседям
                        {
                            int iy=y + dy[k], ix = x + dx[k];
                            if ( iy >= 0 && iy < n && ix >= 0 && ix < n && labi[ix][iy] == BLANK )
                            {
                                stop = false;              // найдены непомеченные клетки
                                labi[ix][iy] = d + 1;      // распространяем волну
                            }
                        }
                    }
            d++;
        } while ( !stop && labi[finish.str][finish.stl] == BLANK );
        if (labi[finish.str][finish.stl] == BLANK)
            cout << "no way!";
        else     //восстановление пути
        {
            int len = labi[finish.str][finish.stl];            // длина кратчайшего пути из (ax, ay) в (bx, by)
            x = finish.str;
            y = finish.stl;
            d = len;
            Stack st;
            data local;
        
            while ( d > 0 )
            {
                local.str = x;
                local.stl = y;                  // записываем ячейку (x, y) в путь
                st.push(local);
                d--;
                for (k = 0; k < 4; ++k)
                {
                    int iy=y + dy[k], ix = x + dx[k];
                    if ( iy >= 0 && iy < n && ix >= 0 && ix < n && labi[ix][iy] == d)
                    {
                        x = x + dx[k];
                        y = y + dy[k];           // переходим в ячейку, которая на 1 ближе к старту
                        break;
                    }
                }
            }
            st.push(start);
            cout << "координаты кратчайшего пути:" << endl;
            while(!st.empty())
            {
                local = st.pop();
                cout <<"x = " << local.str << " y = "<< local.stl << endl;
            }
        }
    }

    
    for (int i = 0; i < n; i++)
        delete [] labi[i]; // очистка памяти
    return 0;
    
}
