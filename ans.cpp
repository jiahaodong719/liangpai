#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <cmath>

using namespace std;

// 结构体表示交易记录
struct Record {
    long long TimeStamp;
    string Symbol;
    int Quantity;
    int Price;
};

// 结构体表示结果
struct Result {
    long long TimeStamp;
    int MaxTimeGap;
    int Volume;
    int WeightedAveragePrice;
    int MaxPrice;
};

// 按分隔符分割字符串
vector<string> split(const string& str, char delim) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(str);
    while (getline(tokenStream, token, delim)) {
        tokens.push_back(token);
    }
    return tokens;
}

// 读取CSV文件并返回记录向量
vector<Record> read_csv(const string& filename) {
    vector<Record> data;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        vector<string> parts = split(line, ',');
        if (parts.size() == 4) {
            Record record;
            try {
                record.TimeStamp = stoll(parts[0]);
                record.Symbol = parts[1];
                record.Quantity = stod(parts[2]);
                record.Price = stod(parts[3]);
                data.push_back(record);
            } catch (const exception& e) {
                cerr << "Error parsing record: " << e.what() << endl;
            }
        }
    }
    return data;
}

int main() {
    // 读取CSV文件中的数据
    vector<Record> data = read_csv("data.csv");
    map<string, Result> results;

    // 遍历数据，计算结果
    for (const auto& record : data) {
        if (results.find(record.Symbol) != results.end()) {
            Result& existingResult = results[record.Symbol];
            existingResult.Volume += record.Quantity;
            existingResult.MaxTimeGap = max(existingResult.MaxTimeGap, static_cast<int>(record.TimeStamp - existingResult.TimeStamp));
            existingResult.TimeStamp = record.TimeStamp;
            existingResult.WeightedAveragePrice += record.Quantity * record.Price;
            existingResult.MaxPrice = max(existingResult.MaxPrice, record.Price);
        } else {
            Result newResult;
            newResult.TimeStamp = record.TimeStamp;
            newResult.Volume = record.Quantity;
            newResult.WeightedAveragePrice = record.Quantity * record.Price;
            newResult.MaxPrice = record.Price;
            results[record.Symbol] = newResult;
        }
    }

    // 计算加权平均价格
    for (auto& [_, result] : results) {
        result.WeightedAveragePrice = round(result.WeightedAveragePrice / result.Volume);
    }

    // 将结果写入输出文件
    ofstream outfile("out.csv");
    if (!outfile.is_open()) {
        cerr << "Error: Unable to open output file!" << endl;
        return 1;
    }

    for (const auto& [symbol, result] : results) {
        outfile << symbol << ","
                << result.MaxTimeGap << ","
                << result.Volume << ","
                << result.WeightedAveragePrice << ","
                << result.MaxPrice << endl;
    }

    outfile.close();

    return 0;
}
