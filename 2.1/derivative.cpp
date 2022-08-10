#include <string>
#include <iostream>
#include <map>

struct Logger {
    static void log(std::string message) {
#ifdef _debug
        static int time = 0;
        std::cout << "[" << time << "]: " << message << std::endl;
        ++time;
#endif
    }

private:
    Logger();
};

class Polynomial {
public:
    void add_monomial(int deg, int coefficient) {
        Logger::log("add_monomial");
        Logger::log(std::to_string(deg));
        Logger::log(std::to_string(coefficient));
        coefficient_by_deg[deg] += coefficient;
    }

    void add_monomial(std::string monomial) {
        Logger::log(monomial);
        if (monomial.empty()) return;
        int x_pos = monomial.find('x');
        if (x_pos == std::string::npos) {
            add_monomial(0, std::stoi(monomial));
            return;
        }
        int coefficient = (x_pos - 1 > 0) ? std::stoi(monomial.substr(1, x_pos - 1)) : 1;
        int degree = (x_pos + 1 < monomial.size()) ? std::stoi(monomial.substr(x_pos + 2, monomial.size() - (x_pos + 2))) : 1;
        if (monomial[0] == '-') coefficient *= -1;
        add_monomial(degree, coefficient);
    }

    Polynomial() {}

    Polynomial(const std::string &poly) {
        std::string curr;
        for (char c: poly) {
            if (c == '+' || c == '-') {
                add_monomial(curr);
                curr.clear();
            }
            curr += c;
        }
        add_monomial(curr);
    }

    Polynomial get_derivation() {
        Logger::log("derivation begins...");
        Polynomial ret;
        for (auto &it: coefficient_by_deg) {
            if (it.first - 1 >= 0) {
                ret.add_monomial(it.first - 1, it.second * it.first);
            }
        }
        return ret;
    }

    std::string to_string() {
        std::string ret;
        for (auto it = coefficient_by_deg.rbegin(); it != coefficient_by_deg.rend(); ++it) {
            if (it->second == 0) continue;
            if (!ret.empty() && it->second > 0) ret += '+';
            if (it->first) {
                if (it->second != 1) {
                    ret += std::to_string(it->second);
                    ret += "*x";
                } else {
                    ret += "x";
                }
                if (it->first > 1) ret += "^" + std::to_string(it->first);
            } else {
                ret += std::to_string(it->second);
            }
        }
        if (ret.empty()) ret = "0";
        return ret;
    }

private:
    std::map<int, int> coefficient_by_deg;
};

std::string derivative(std::string poly) {
    if (poly[0] != '-') poly.insert(poly.begin(), '+');
    Polynomial polynomial(poly);
    return polynomial.get_derivation().to_string();
}

int main() {
    std::string polynomial;
    std::cin >> polynomial;
    std::cout << derivative(polynomial) << std::endl;
    return 0;
}
