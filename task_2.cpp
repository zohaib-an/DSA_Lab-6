#include <iostream>
#include <string>
using namespace std;

template <typename T>
class Stack {
    T* a;
    int cap, top;
public:
    Stack() : cap(10), top(0) { a = new T[cap]; }
    ~Stack() { delete[] a; }
    void push(T x) {
        if (top == cap) {
            T* b = new T[cap * 2];
            for (int i = 0; i < top; i++) b[i] = a[i];
            delete[] a;
            a = b;
            cap *= 2;
        }
        a[top++] = x;
    }
    T pop() { return a[--top]; }
    T peek() { return a[top - 1]; }
    T at(int i) { return a[i]; }
    bool empty() { return top == 0; }
    int size() { return top; }
};

bool isValidBrackets(string s) {
    Stack<char> st;
    for (int i = 0; i < (int)s.size(); i++) {
        char c = s[i];
        if (c == '(' || c == '{' || c == '[') {
            st.push(c);
        } else if (c == ')' || c == '}' || c == ']') {
            if (st.empty()) return false;
            char o = st.pop();
            if ((c == ')' && o != '(') || (c == '}' && o != '{') || (c == ']' && o != '['))
                return false;
        }
    }
    return st.empty();
}

struct Result {
    bool ok;
    string errs[100];
    int n;
};

void addErr(Result& r, string e) {
    if (r.n < 100) r.errs[r.n++] = e;
    r.ok = false;
}

Result validate_markup(string s) {
    Result r;
    r.ok = true;
    r.n = 0;
    Stack<string> st;
    int i = 0, n = s.size();

    while (i < n) {
        if (s[i] != '<') { i++; continue; }
        int j = s.find('>', i);
        if (j == (int)string::npos) {
            addErr(r, "Unclosed '<' at position " + to_string(i));
            break;
        }
        string tag = s.substr(i + 1, j - i - 1);
        i = j + 1;

        if (tag.empty()) { addErr(r, "Empty tag <>"); continue; }
        if (tag[0] == '!' || tag[0] == '?') continue;

        bool closing = (tag[0] == '/');
        bool self = (tag[tag.size() - 1] == '/');
        int k = closing ? 1 : 0;
        string name;
        while (k < (int)tag.size() && tag[k] != ' ' && tag[k] != '/') name += tag[k++];

        if (name.empty()) { addErr(r, "Tag with no name"); continue; }
        if (self && !closing) continue;
        if (!closing) { st.push(name); continue; }

        if (st.empty()) { addErr(r, "Unexpected closing tag </" + name + ">"); continue; }
        if (st.peek() == name) { st.pop(); continue; }

        bool found = false;
        for (int m = 0; m < st.size(); m++)
            if (st.at(m) == name) found = true;

        if (found) {
            while (st.peek() != name) addErr(r, "Tag <" + st.pop() + "> not closed");
            st.pop();
        } else {
            addErr(r, "Unexpected closing tag </" + name + ">");
        }
    }
    while (!st.empty()) addErr(r, "Tag <" + st.pop() + "> not closed");
    return r;
}

int main() {
    int ch;
    string s;
    do {
        cout << "\n1. Check brackets\n2. Check HTML/XML\n0. Exit\nChoice: ";
        if (!(cin >> ch)) break;
        cin.ignore();
        if (ch == 1) {
            cout << "Enter string: ";
            getline(cin, s);
            cout << (isValidBrackets(s) ? "Valid" : "Invalid") << endl;
        } else if (ch == 2) {
            cout << "Enter markup: ";
            getline(cin, s);
            Result r = validate_markup(s);
            cout << (r.ok ? "Valid" : "Invalid") << endl;
            for (int i = 0; i < r.n; i++) cout << "  " << r.errs[i] << endl;
        }
    } while (ch != 0);
    return 0;
}
