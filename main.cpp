#include <iostream>
#include <vector>
#include <queue> // Для использования очереди с приоритетом
#include <algorithm> // Для сортировки
#include <fstream> // Для работы с файлами

using namespace std;

struct Element
{
    int price, weight; // стоимость, вес
    double heuristics; // (price / weight) - эвристика для сортировки
};

class Node
{
    public:
    int ind, price, weight; // индекс текущего элемента, его стоимость, вес
    double profit; // максимально возможная прибыль, которую можно получить, продолжая алгоритм ветвей и границ с этого узла

    Node(int i, int pric, int w, double prof)
    {
        ind = i;
        price = pric;
        weight = w;
        profit = prof;
    }

    Node() {};
};


// функция для определения прибыльности пути от этой вершины
double profit(int ind, int price, int weight, int N, int W, vector<Element> &items)
{
    // если на текущей стадии суммарный вес равен или уже превышает максимально допустимый вес,
    // то дальнейшая прибыль алгоритма = 0
    if (weight >= W) return 0.0;


    // изначально прибыль от этой вершины логично равна прибыли только ее самой
    double total_profit = price;

    // индекс следующей рассматриваемой вершины
    int i = ind + 1;

    // изначально вес от этой вершины логично равен весу только ее самой
    int total_weight = weight;

    // пока суммарный вес всех вершин не превысил максимально возможный и пока не закончились предметы
    // смотрим, что будет с суммарной прибылью и суммарным весом, если добавить следующую вершину
    while (total_weight + items[i].weight <= W && i < N)
    {
        total_profit += items[i].price;
        total_weight += items[i].weight;
        i = i + 1;
    }

    // если суммарный вес уже больше максимального, а есть еще незадействованные вершины,
    // то считаем суммарную прибыль от изначальной вершины через эвристику
    if (i < N)
    {
        double h = items[i].heuristics;
        double prof = (W - total_weight) * h;
        total_profit += prof;
    }

    return total_profit;
}


vector<int> Branch_and_Bound(int N, int W, vector<Element> &items)
{
    // лямбда-функция для сравнения Element'ов, подающаяся, как параметр сортировки в функцию sort
    bool (*cmp)(const Element &a, const Element &b) = [](const Element &a, const Element &b) { return a.heuristics > b.heuristics; };
    // сортируем по этому параметру вектор items
    sort(items.begin(), items.end(), cmp);

    // лямбда-функция для сравнения Node'ов, подающаяся, как параметр оператор сравнения Node'ов в шаблон приоритетной очереди
    bool (*cmp_bound)(const Node &a, const Node &b) = [](const Node &a, const Node &b) { return a.profit < b.profit;};
    // создается очередь с приоритетом PQ, которая будет хранить узлы, упорядоченные по максимальной прибыли(чем выше граница, тем выше приоритет)
    priority_queue<Node, vector<Node>, decltype(cmp_bound)> PQ(cmp_bound);

    // изначально мы не брали ни одного предмета
    int ind = -1, price = 0, weight = 0;
    // cчитаем от следующей вершины
    double prof = profit(ind, price, weight, N, W, items);
    // добавляем вершину u в очередь с приоритетом
    Node u(ind, price, weight, prof);
    PQ.push(u);

    // на данный момент суммарный вес всех вершин и суммарная стоимость равны 0
    int max_profit = 0;
    int total_weight = 0;

    Node v, w;
    // пока очередь не пуста
    while (!PQ.empty())
    {
        // извлекаем самый прибыльный узел. В очереди PQ самый первый элемент - самый прибыльный, извлекаем его
        w = PQ.top();
        // удаляем самый первый элемент
        PQ.pop();

        if (w.ind + 1 >= N) break; // если перебрали все N вершин, то заканичиваем

        // добавляем вершину w в итоговый рюкзак

        // обновляем кол-во засунутых вершин в рюкзак
        v.ind = w.ind + 1;
        // обновляем суммарный вес рюкзака
        v.price = w.price + items[v.ind].price;
        // обновляем суммарную стоимость рюкзака
        v.weight = w.weight + items[v.ind].weight;

        // если вес нового узла меньше максимального веса W и его стоимость больше текущего максимума,
        // то меняем вес и стоимость рюкзака
        if (v.price > max_profit && v.weight <= W)
        {
            total_weight = v.weight;
            max_profit = v.price;
        }

        // для нового узла вычисляем максимальную прибыль(с учетом всех предыдущих узлов)
        v.profit = profit(v.ind, v.price, v.weight, N, W, items);
        // и добавляем его в очередь, если эта прибыль больше текущей
        if (v.profit > max_profit)
        {
            PQ.push(v);
        }

        // если же мы не смогли добавить элемент w в итоговый рюкзак,
        // то считаем, максимальную прибыль от этой вершины
        v.weight = w.weight;
        v.price = w.price;
        v.profit = profit(v.ind, v.price, v.weight, N, W, items);

        if (v.profit > max_profit)
            PQ.push(v);
    }



    return {max_profit, total_weight};
}


// -1 - Файл не открылся, 0 - все работало правильно
int main()
{
    // читаем файл
    ifstream input("ks_10000_0");

    if (!input)
    {
        cerr << "Error: The file didn't open" << endl;
        return -1;
    }

    // читаем первую строку
    int N, W;
    input >> N >> W;

    // читаем N строк и переписываем их в вектор
    vector<Element> items(N);
    int j = 0, a, b;
    for (j; j < N; ++j)
    {
        input >> a >> b;
        items[j].price = a;
        items[j].weight = b;
        items[j].heuristics = (double)a / b;
    }

    // закрываем файл
    input.close();

    vector<int> Victor = Branch_and_Bound(N, W, items);

    int profit_branch_and_bound = Victor[0];
    int weight_branch_and_bound = Victor[1];
    cout << "Profit: " << profit_branch_and_bound << endl;
    cout << "Total weight: " << weight_branch_and_bound << endl;

    return 0;
}
/*
1099893
999994
*/

