#include <iostream>
#include <vector>
#include <queue> // ��� ������������� ������� � �����������
#include <algorithm> // ��� ����������
#include <fstream> // ��� ������ � �������

using namespace std;

struct Element
{
    int price, weight; // ���������, ���
    double heuristics; // (price / weight) - ��������� ��� ����������
};

class Node
{
    public:
    int ind, price, weight; // ������ �������� ��������, ��� ���������, ���
    double profit; // ����������� ��������� �������, ������� ����� ��������, ��������� �������� ������ � ������ � ����� ����

    Node(int i, int pric, int w, double prof)
    {
        ind = i;
        price = pric;
        weight = w;
        profit = prof;
    }

    Node() {};
};


// ������� ��� ����������� ������������ ���� �� ���� �������
double profit(int ind, int price, int weight, int N, int W, vector<Element> &items)
{
    // ���� �� ������� ������ ��������� ��� ����� ��� ��� ��������� ����������� ���������� ���,
    // �� ���������� ������� ��������� = 0
    if (weight >= W) return 0.0;


    // ���������� ������� �� ���� ������� ������� ����� ������� ������ �� �����
    double total_profit = price;

    // ������ ��������� ��������������� �������
    int i = ind + 1;

    // ���������� ��� �� ���� ������� ������� ����� ���� ������ �� �����
    int total_weight = weight;

    // ���� ��������� ��� ���� ������ �� �������� ����������� ��������� � ���� �� ����������� ��������
    // �������, ��� ����� � ��������� �������� � ��������� �����, ���� �������� ��������� �������
    while (total_weight + items[i].weight <= W && i < N)
    {
        total_profit += items[i].price;
        total_weight += items[i].weight;
        i = i + 1;
    }

    // ���� ��������� ��� ��� ������ �������������, � ���� ��� ����������������� �������,
    // �� ������� ��������� ������� �� ����������� ������� ����� ���������
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
    // ������-������� ��� ��������� Element'��, ����������, ��� �������� ���������� � ������� sort
    bool (*cmp)(const Element &a, const Element &b) = [](const Element &a, const Element &b) { return a.heuristics > b.heuristics; };
    // ��������� �� ����� ��������� ������ items
    sort(items.begin(), items.end(), cmp);

    // ������-������� ��� ��������� Node'��, ����������, ��� �������� �������� ��������� Node'�� � ������ ������������ �������
    bool (*cmp_bound)(const Node &a, const Node &b) = [](const Node &a, const Node &b) { return a.profit < b.profit;};
    // ��������� ������� � ����������� PQ, ������� ����� ������� ����, ������������� �� ������������ �������(��� ���� �������, ��� ���� ���������)
    priority_queue<Node, vector<Node>, decltype(cmp_bound)> PQ(cmp_bound);

    // ���������� �� �� ����� �� ������ ��������
    int ind = -1, price = 0, weight = 0;
    // c������ �� ��������� �������
    double prof = profit(ind, price, weight, N, W, items);
    // ��������� ������� u � ������� � �����������
    Node u(ind, price, weight, prof);
    PQ.push(u);

    // �� ������ ������ ��������� ��� ���� ������ � ��������� ��������� ����� 0
    int max_profit = 0;
    int total_weight = 0;

    Node v, w;
    // ���� ������� �� �����
    while (!PQ.empty())
    {
        // ��������� ����� ���������� ����. � ������� PQ ����� ������ ������� - ����� ����������, ��������� ���
        w = PQ.top();
        // ������� ����� ������ �������
        PQ.pop();

        if (w.ind + 1 >= N) break; // ���� ��������� ��� N ������, �� ������������

        // ��������� ������� w � �������� ������

        // ��������� ���-�� ��������� ������ � ������
        v.ind = w.ind + 1;
        // ��������� ��������� ��� �������
        v.price = w.price + items[v.ind].price;
        // ��������� ��������� ��������� �������
        v.weight = w.weight + items[v.ind].weight;

        // ���� ��� ������ ���� ������ ������������� ���� W � ��� ��������� ������ �������� ���������,
        // �� ������ ��� � ��������� �������
        if (v.price > max_profit && v.weight <= W)
        {
            total_weight = v.weight;
            max_profit = v.price;
        }

        // ��� ������ ���� ��������� ������������ �������(� ������ ���� ���������� �����)
        v.profit = profit(v.ind, v.price, v.weight, N, W, items);
        // � ��������� ��� � �������, ���� ��� ������� ������ �������
        if (v.profit > max_profit)
        {
            PQ.push(v);
        }

        // ���� �� �� �� ������ �������� ������� w � �������� ������,
        // �� �������, ������������ ������� �� ���� �������
        v.weight = w.weight;
        v.price = w.price;
        v.profit = profit(v.ind, v.price, v.weight, N, W, items);

        if (v.profit > max_profit)
            PQ.push(v);
    }



    return {max_profit, total_weight};
}


// -1 - ���� �� ��������, 0 - ��� �������� ���������
int main()
{
    // ������ ����
    ifstream input("ks_10000_0");

    if (!input)
    {
        cerr << "Error: The file didn't open" << endl;
        return -1;
    }

    // ������ ������ ������
    int N, W;
    input >> N >> W;

    // ������ N ����� � ������������ �� � ������
    vector<Element> items(N);
    int j = 0, a, b;
    for (j; j < N; ++j)
    {
        input >> a >> b;
        items[j].price = a;
        items[j].weight = b;
        items[j].heuristics = (double)a / b;
    }

    // ��������� ����
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

