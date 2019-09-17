#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <mutex>
#include <sstream>
#include "CacheManager.h"
#include "Convertor.h"


template<typename P, typename S>
class FileCacheManager : public CacheManager<P, S> {
    string path;
    Convertor<P, S> *convertors;
    unordered_map <string, string> allPS;
    mutex mtx;
public:

    FileCacheManager(const string &path, Convertor<P, S> *convertors) : path(path),
                                                                        convertors(convertors) {
        loadFromFile();
    }


    bool isExistSol(const P &pob) const override {
        string psv = convertors->conVecProblemToString(pob);
        return allPS.find(psv) != allPS.end();
    }

    bool isExistProb(const P &pob) const override {
        string psv = convertors->conVecProblemToString(pob);
        return allPS.find(psv) != allPS.end();
    }

    void mapAdding(P problem, S solution) {
        string problemS = convertors->conVecProblemToString(problem);
        string solveS = convertors->conSolvToString(solution);
        allPS[problemS] = solveS;

    }

    void savePS(P problem, S solution) override {
        mtx.lock();
        /* Create file . */
        ofstream dataFile(this->path, ios::app);

        /* Adding to map. */
        mapAdding(problem, solution);
        /* Write to file */
        string problemS = convertors->conVecProblemToString(problem);
        string solveS = convertors->conSolvToString(solution);
        dataFile << problemS << '$' << solveS << endl;

        /* Close file. */
        dataFile.close();
        mtx.unlock();
    }

    S getSol(P problem) override {
        string problemS = convertors->conVecProblemToString(problem);
        if (isExistProb(problem)) {
            return allPS[problemS];
        } else {
            string answer = "NO problem Exist";
            return answer;
        }
    }

    void loadFromFile() override {
        string prob;
        string solu;
        ifstream data(this->path);
        stringstream stringstream1;
        string buffer;

        if (!data.good()) {
            return;
        }
        vector<string> vector;
        /* Read a line from file. */
        while (std::getline(data, buffer)) {
            vector = splitIt(buffer, "$");

            allPS[vector[0]] = vector[1];
        }

        data.close();
    }
private:
    bool is_empty(const string &pFile) {
        ifstream data;
        data.open(pFile);
        return data.peek() == std::ifstream::traits_type::eof();
    }

    vector<string> splitIt(string str, string token) {
        vector<string> result;
        while (str.size()) {
            int index = str.find(token);
            if (index != string::npos) {
                result.push_back(str.substr(0, index));
                str = str.substr(index + token.size());
                if (str.size() == 0)result.push_back(str);
            } else {
                result.push_back(str);
                str = "";
            }
        }
        return result;
    }

};
