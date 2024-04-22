#include <iostream> 
#include <queue>
#include <set>

int main() {
    std::cout << "Hello World" << std::endl;

    std::queue<std::pair<float, int>> lastHs;
    std::queue<std::pair<float, int>> lastLs;
    std::set<std::pair<float, int>, std::greater<std::pair<float, int>>> setLastHs;
    std::set<std::pair<float, int>, std::less<std::pair<float, int>>> setLastLs;
    
    std::vector<float> v {1.f, 2.f, 5.f, 109.f, 2.f, -1.f, -490.f, 23.f, 1.f, -1.f};
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