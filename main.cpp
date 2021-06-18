#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <thread>
#include <iomanip>
#include "nlohmann/json.hpp"

size_t get_substring(const std::string &row, const std::string &starts_with, const std::string &ends_with, std::string &result){
    std::size_t start_pos  = row.find(starts_with);
    if (start_pos == std::string::npos){
        std::cerr << "can not find start_pos" << std::endl;
        return std::string::npos;
    }
    start_pos = start_pos + starts_with.length();

    std::size_t end_pos = row.find(ends_with, start_pos);
    if (end_pos == std::string::npos){
        std::cerr << "can not find end_pos" << std::endl;
        return std::string::npos;
    }

    result = std::string(row, start_pos, end_pos - start_pos);
    return end_pos;
}

typedef bool (*sort_fn)(float, float);
bool lower (float i,float j) { return (i<j); }
bool greater (float i,float j) { return (i>j); }


void sort(const std::unordered_map<float, int> &tree, std::vector<float> &keys, sort_fn fn){
    for (auto& it : tree) {
        keys.push_back(it.first);
    }
    std::sort(keys.begin(), keys.end(), fn);
}

void get_from_js(nlohmann::json &json, const std::string &what, std::unordered_map<float, int> &result){
    for(const auto &it : json["tick"][what]) {
        float price = it.begin()->get<float>();
        int amount = std::next(it.begin())->get<int>();
        amount == 0 ? result.erase(price) : result[price] = amount;
    }
}

int main() {
    std::unordered_map<float, int> asks{};
    std::unordered_map<float, int> bids{};

    std::ifstream ifs{"../data/huobi_dm_depth.log"};
    std::ofstream ofs{"../data/output.dat", std::ios::app};
    std::string row{};
    while (std::getline(ifs, row)){
        row += '\n';

        std::string value_from_str{};
        get_substring(row, "message: ", "\n", value_from_str);

        nlohmann::json js = nlohmann::json::parse(value_from_str);
        if(js.find("ping") != js.end()){
//            std::cerr << "[WARN] Ping" << std::endl;
            continue;
        }

        get_from_js(js, "asks", asks);
        get_from_js(js, "bids", bids);
        size_t tm = js["tick"]["ts"].get<uint64_t>();

        std::vector<float> keys_asks;
        keys_asks.reserve(asks.size());
        std::thread sort_asks(sort, std::ref(asks), std::ref(keys_asks), std::ref(lower));

        std::vector<float> keys_bids;
        keys_asks.reserve(bids.size());
        std::thread sort_bids(sort, std::ref(bids), std::ref(keys_bids), std::ref(greater));

        sort_asks.join();
        sort_bids.join();

        char buff[180];
        sprintf (buff,"%ld, %.2f, %.2d, %.2f, %.2d \n", tm, keys_bids[0], bids[keys_bids[0]], keys_asks[0], asks[keys_asks[0]]);
        ofs << buff;

    }
    ifs.close();
    ofs.close();

    return 0;
}