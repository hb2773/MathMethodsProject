#include <iostream> 
#include <queue>
#include <set>

int main() {
    std::cout << "Hello World" << std::endl;

    std::queue<std::pair<double, int>> lastHs;
    std::queue<std::pair<double, int>> lastLs;
    std::set<std::pair<double, int>, std::greater<std::pair<double, int>>> setLastHs;
    std::set<std::pair<double, int>, std::less<std::pair<double, int>>> setLastLs;
    
    std::vector<double> v {1., 2., 5., 109., 2., -1., -490., 23, 1., -1.};
    int k = 3;

    for (int i = 0; i < v.size(); i++) {
        setLastHs.insert({v[i], i});
        lastHs.push({v[i], i});
        setLastLs.insert({v[i], i});
        lastLs.push({v[i], i});
        if (lastHs.size() > k) {
            setLastHs.erase(lastHs.front());
            setLastLs.erase(lastLs.front());
            lastHs.pop();
            lastLs.pop();
        } 
        std::cout << setLastHs.begin()->first << std::endl;
        std::cout << setLastLs.begin()->first << std::endl;
    }
}