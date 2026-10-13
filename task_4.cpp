#include <iostream>
using namespace std;

class WaitingRoom {
    int* q;
    int cap, f, sz, nextId;
public:
    WaitingRoom() : cap(5), f(0), sz(0), nextId(1) { q = new int[cap]; }
    ~WaitingRoom() { delete[] q; }

    void RegisterPatient() {
        if (sz == cap) {
            int* b = new int[cap * 2];
            for (int i = 0; i < sz; i++) b[i] = q[(f + i) % cap];
            delete[] q;
            q = b;
            cap *= 2;
            f = 0;
        }
        q[(f + sz) % cap] = nextId;
        sz++;
        cout << "Patient registered with ID " << nextId << endl;
        nextId++;
    }

    void ServePatient() {
        if (sz == 0) { cout << "No patients waiting\n"; return; }
        cout << "Now serving patient " << q[f] << endl;
        f = (f + 1) % cap;
        sz--;
    }

    void CancelAll() {
        f = 0;
        sz = 0;
        cout << "All appointments cancelled\n";
    }

    bool CanDoctorGoHome() { return sz == 0; }

    void ShowAllPatient() {
        if (sz == 0) { cout << "No patients waiting\n"; return; }
        int* t = new int[sz];
        for (int i = 0; i < sz; i++) t[i] = q[(f + i) % cap];
        for (int i = 1; i < sz; i++) {
            int key = t[i], j = i - 1;
            while (j >= 0 && t[j] > key) { t[j + 1] = t[j]; j--; }
            t[j + 1] = key;
        }
        cout << "Waiting patients: ";
        for (int i = 0; i < sz; i++) cout << t[i] << " ";
        cout << endl;
        delete[] t;
    }
};

int main() {
    WaitingRoom w;
    int ch;
    do {
        cout << "\n1. Register patient\n2. Serve patient\n3. Cancel all\n"
                "4. Can doctor go home?\n5. Show all patients\n0. Exit\nChoice: ";
        cin >> ch;
        switch (ch) {
            case 1: w.RegisterPatient(); break;
            case 2: w.ServePatient(); break;
            case 3: w.CancelAll(); break;
            case 4: cout << (w.CanDoctorGoHome() ? "Yes" : "No") << endl; break;
            case 5: w.ShowAllPatient(); break;
        }
    } while (ch != 0);
    return 0;
}
