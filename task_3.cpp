#include <iostream>
using namespace std;

struct Req {
    int id, arrive, proc;
};

class CQueue {
    Req* a;
    int cap, f, sz;
public:
    CQueue() : cap(2), f(0), sz(0) { a = new Req[cap]; }
    ~CQueue() { delete[] a; }

    bool isEmpty() { return sz == 0; }
    bool isFull() { return sz == cap; }

    void resize(int nc) {
        Req* b = new Req[nc];
        for (int i = 0; i < sz; i++) b[i] = a[(f + i) % cap];
        delete[] a;
        a = b;
        cap = nc;
        f = 0;
        cout << "  [resized to capacity " << nc << "]\n";
    }

    void enqueue(Req x) {
        if (isFull()) resize(cap * 2);
        a[(f + sz) % cap] = x;
        sz++;
    }

    Req dequeue() {
        Req x = a[f];
        f = (f + 1) % cap;
        sz--;
        if (cap > 2 && sz <= cap / 4) resize(cap / 2);
        return x;
    }

    Req front() { return a[f]; }

    void show() {
        cout << "  Queue (size " << sz << ", cap " << cap << "): ";
        if (sz == 0) cout << "empty";
        for (int i = 0; i < sz; i++) {
            Req r = a[(f + i) % cap];
            cout << "[ID" << r.id << " arr=" << r.arrive << " proc=" << r.proc << "] ";
        }
        cout << endl;
    }
};

CQueue q;
int clk = 0, totalWait = 0, done = 0;

void add(int id, int arrive, int proc) {
    Req r = {id, arrive, proc};
    cout << "Enqueue request " << id << endl;
    q.enqueue(r);
    q.show();
}

void serve() {
    if (q.isEmpty()) { cout << "Queue empty\n"; return; }
    Req r = q.dequeue();
    int start = clk > r.arrive ? clk : r.arrive;
    int wait = start - r.arrive;
    clk = start + r.proc;
    totalWait += wait;
    done++;
    cout << "Dequeue request " << r.id << " (start=" << start << ", wait=" << wait << ", end=" << clk << ")\n";
    q.show();
}

int main() {
    add(1, 0, 5);
    add(2, 1, 3);
    add(3, 2, 4);
    add(4, 3, 2);
    add(5, 4, 6);
    serve();
    serve();
    add(6, 8, 3);
    add(7, 9, 2);
    while (!q.isEmpty()) serve();

    cout << "\nProcessed: " << done << endl;
    cout << "Average waiting time: " << (double)totalWait / done << endl;
    return 0;
}
