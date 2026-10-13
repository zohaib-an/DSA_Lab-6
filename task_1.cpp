#include <iostream>
#include <string>
#include <cmath>
#include <stdexcept>
using namespace std;

template <typename T>
class Stack {
    struct Node {
        T val;
        Node* next;
    };
    Node* head;
public:
    Stack() : head(0) {}
    ~Stack() { while (head) pop(); }
    void push(T x) {
        Node* n = new Node;
        n->val = x;
        n->next = head;
        head = n;
    }
    T pop() {
        if (!head) throw runtime_error("Stack underflow");
        Node* n = head;
        T x = n->val;
        head = n->next;
        delete n;
        return x;
    }
    T peek() {
        if (!head) throw runtime_error("Stack empty");
        return head->val;
    }
    bool empty() { return head == 0; }
};

class Tokens {
    string* a;
    int cap;
public:
    int n;
    Tokens() : a(new string[16]), cap(16), n(0) {}
    ~Tokens() { delete[] a; }
    Tokens(const Tokens& o) : a(new string[o.cap]), cap(o.cap), n(o.n) {
        for (int i = 0; i < n; i++) a[i] = o.a[i];
    }
    void add(string s) {
        if (n == cap) {
            string* b = new string[cap * 2];
            for (int i = 0; i < n; i++) b[i] = a[i];
            delete[] a;
            a = b;
            cap *= 2;
        }
        a[n++] = s;
    }
    string& operator[](int i) { return a[i]; }
    void rev() {
        for (int i = 0, j = n - 1; i < j; i++, j--) {
            string t = a[i];
            a[i] = a[j];
            a[j] = t;
        }
    }
    string join() {
        string s;
        for (int i = 0; i < n; i++) {
            if (i) s += ' ';
            s += a[i];
        }
        return s;
    }
};

bool isOp(string t) {
    return t.size() == 1 && (t[0] == '+' || t[0] == '-' || t[0] == '*' || t[0] == '/' || t[0] == '^');
}
bool isNum(char c) { return (c >= '0' && c <= '9') || c == '.'; }
bool isAlpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
bool isOperand(string t) { return isNum(t[0]) || isAlpha(t[0]); }

int prec(string op) {
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/") return 2;
    return 3;
}

Tokens tokenize(string s) {
    Tokens t;
    int i = 0;
    while (i < (int)s.size()) {
        char c = s[i];
        if (c == ' ') { i++; continue; }
        if (isNum(c)) {
            string num;
            while (i < (int)s.size() && isNum(s[i])) num += s[i++];
            t.add(num);
        } else if (isAlpha(c) || c == '(' || c == ')' || isOp(string(1, c))) {
            t.add(string(1, c));
            i++;
        } else {
            throw runtime_error(string("Invalid character: ") + c);
        }
    }
    return t;
}

double toNum(string t) {
    if (!isNum(t[0])) throw runtime_error("Cannot evaluate variable " + t);
    double r = 0, f = 0.1;
    bool dot = false;
    for (int i = 0; i < (int)t.size(); i++) {
        if (t[i] == '.') dot = true;
        else if (!dot) r = r * 10 + (t[i] - '0');
        else { r += (t[i] - '0') * f; f /= 10; }
    }
    return r;
}

double calc(string op, double a, double b) {
    if (op == "+") return a + b;
    if (op == "-") return a - b;
    if (op == "*") return a * b;
    if (op == "/") {
        if (b == 0) throw runtime_error("Division by zero");
        return a / b;
    }
    return pow(a, b);
}

Tokens shunt(Tokens& t, bool reversed) {
    Tokens out;
    Stack<string> st;
    for (int i = 0; i < t.n; i++) {
        string x = t[i];
        if (isOperand(x)) {
            out.add(x);
        } else if (x == "(") {
            st.push(x);
        } else if (x == ")") {
            while (!st.empty() && st.peek() != "(") out.add(st.pop());
            if (st.empty()) throw runtime_error("Mismatched parentheses");
            st.pop();
        } else {
            while (!st.empty() && st.peek() != "(") {
                int p1 = prec(st.peek()), p2 = prec(x);
                bool right = (x == "^");
                bool pop = p1 > p2 || (p1 == p2 && (reversed ? right : !right));
                if (!pop) break;
                out.add(st.pop());
            }
            st.push(x);
        }
    }
    while (!st.empty()) {
        string op = st.pop();
        if (op == "(") throw runtime_error("Mismatched parentheses");
        out.add(op);
    }
    return out;
}

string infix_to_postfix(string expr) {
    Tokens t = tokenize(expr);
    Tokens out = shunt(t, false);
    return out.join();
}

string infix_to_prefix(string expr) {
    Tokens t = tokenize(expr);
    t.rev();
    for (int i = 0; i < t.n; i++) {
        if (t[i] == "(") t[i] = ")";
        else if (t[i] == ")") t[i] = "(";
    }
    Tokens out = shunt(t, true);
    out.rev();
    return out.join();
}

string postfix_to_infix(string expr) {
    Tokens t = tokenize(expr);
    Stack<string> st;
    for (int i = 0; i < t.n; i++) {
        if (isOperand(t[i])) {
            st.push(t[i]);
        } else if (isOp(t[i])) {
            string b = st.pop();
            string a = st.pop();
            st.push("(" + a + " " + t[i] + " " + b + ")");
        } else {
            throw runtime_error("Invalid token: " + t[i]);
        }
    }
    string res = st.pop();
    if (!st.empty()) throw runtime_error("Malformed expression");
    return res;
}

double evaluate_postfix(string expr) {
    Tokens t = tokenize(expr);
    Stack<double> st;
    for (int i = 0; i < t.n; i++) {
        if (isOperand(t[i])) {
            st.push(toNum(t[i]));
        } else if (isOp(t[i])) {
            double b = st.pop();
            double a = st.pop();
            st.push(calc(t[i], a, b));
        } else {
            throw runtime_error("Invalid token: " + t[i]);
        }
    }
    double res = st.pop();
    if (!st.empty()) throw runtime_error("Malformed expression");
    return res;
}

double evaluate_prefix(string expr) {
    Tokens t = tokenize(expr);
    Stack<double> st;
    for (int i = t.n - 1; i >= 0; i--) {
        if (isOperand(t[i])) {
            st.push(toNum(t[i]));
        } else if (isOp(t[i])) {
            double a = st.pop();
            double b = st.pop();
            st.push(calc(t[i], a, b));
        } else {
            throw runtime_error("Invalid token: " + t[i]);
        }
    }
    double res = st.pop();
    if (!st.empty()) throw runtime_error("Malformed expression");
    return res;
}

int main() {
    string line;
    cout << "Enter infix expression (blank to quit):\n";
    while (true) {
        cout << "> ";
        if (!getline(cin, line) || line.empty()) break;
        try {
            string post = infix_to_postfix(line);
            string pre = infix_to_prefix(line);
            cout << "Postfix: " << post << endl;
            cout << "Prefix:  " << pre << endl;
            cout << "Infix:   " << postfix_to_infix(post) << endl;
            try {
                cout << "Postfix value: " << evaluate_postfix(post) << endl;
                cout << "Prefix value:  " << evaluate_prefix(pre) << endl;
            } catch (exception& e) {
                cout << "(not evaluated: " << e.what() << ")\n";
            }
        } catch (exception& e) {
            cout << "Error: " << e.what() << endl;
        }
    }
    return 0;
}
