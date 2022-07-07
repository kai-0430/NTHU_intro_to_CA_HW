#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <set>
using namespace std;

class Quine_McCluskey
{
public:
    Quine_McCluskey() {};
    ~Quine_McCluskey() {};
    void read_input_data(char *file_name);
    //void sort_by_num_of_1(); 
    void gen_prime_implicant();         // still not prime
    //int merge_implicants_in_two_sets(vector<set<string>>& new_impli_table, set<string>& set1, set<string>& set2);
    void column_covering() {};
    void solve();
    void print_implicant() const;
    void print_prime_implicant() const; 
private:
    int nVar, nProdTerm;
    string merge(string&, string&);
    bool can_merge(string&, string&);
    vector<int> oneArr;             // number of ones of each ??
    vector<string> impli;           // implicants
    vector<string> prime_impli;     // prime implicants
    vector<string> result;          // optimization result
};

void Quine_McCluskey::read_input_data(char *file_name)
{
    ifstream inputFile(file_name);
    if (inputFile.is_open()) {
        string line;
        getline(inputFile, line);
        nVar = stoi(line);
        getline(inputFile, line);
        nProdTerm = stoi(line);
        impli = vector<string> (nProdTerm);
        for (int i = 0; i < nProdTerm; i++) {
            getline(inputFile, line);
            impli[i] = line;
        }
        inputFile.close();
    } else
        throw "error";
}

void Quine_McCluskey::print_implicant(void) const
{
    cout << "the number of product terms = " << nProdTerm << endl;
    cout << "the number of variables = " << nVar << endl;
    for (int i = 0; i < nProdTerm; i++)
        cout << impli[i] << endl;
}

void Quine_McCluskey::gen_prime_implicant()
{
    // ***** sort by the size (# of dc) of implicant *****
    int nSize = nVar + 1;
    vector<set<string>> impli_by_size(nSize);
    for (int i = 0; i < nProdTerm; i++) {
        int cnt = 0;
        for (int j = 0; j < nVar; j++) {
            if (impli[i][j] == '-')
                cnt++;
        }
        impli_by_size[cnt].insert(impli[i]);
    }


    for (int i = 0; i < nSize; i++) {
        cout << i << "'s dc:" << endl;
        for (string& str : impli_by_size[i])
            cout << "  " << str << endl;
    }
    cout << endl;

        set<string>::iterator itr;
            for (itr = impli_by_size[i].begin(); itr != impli_by_size[i].end(); itr++) {
                int cnt = 0;
                for (int j = 0; j < nVar; j++) {
                    if ((*itr)[j] == '1')
                        cnt++;
                }
                impli_sort_by_1[cnt].insert(*itr);
            }

    // ***** merge implicants with the same size  *****
    int size_now = -1;
    for (auto& v : impli_by_size) {
        size_now++;
        if (!v.empty()) {
            int size = v.size();
            vector<bool> flag(len);
            set<string> temp;
            for (int i = 0; i < len; i++)
                flag[i] = false;            // 0: cannot be emerged, 1: can be emerged, 2: repeat
            set<string>::iterator it1, it2;
            int i = 0, j = 0;
            for (it1 = v.begin(), i = 0; it1 != v.end(); it1++, i++) {
                //if (flag[i] == 2)        // skip, the same
                //    continue;
                for (it2 = ++itr, j = 0; it2 != v.end(); it2++, j++) {
                    string str = merge(*it1, *it2);
                    if (!str.empty()) {
                        //if (str == "same")
                        //    flag[j] = 2;
                        //else {
                            flag[i] = 1;
                            flag[j] = 1;
                            impli_by_size[size_now + 1].insert(str);
                        //}
                    }
                }
                if (flag[i] == 0)
                    temp.insert(v[i]);
            }
            v = temp;
        }
    }


    for (int i = 0; i < nSize; i++) {
        cout << i << "'s dc:" << endl;
        for (string str : impli_by_size[i])
            cout << "  " << str << endl;
    }
    cout << endl;

    // ***** merge implicants across different sizes  *****
    for (int i = nSize - 1; i >= 0 && !impli_by_size[i].empty(); i--) {
        for (int j = i - 1; j >= 0 && !impli_by_size[j].empty(); j--) {
            for (int m = 0; m < impli_by_size[i]; m++)
                for (int n = 0; n < impli_by_size[j]; n++) {
                    if (can_merge(impli_by_size[i][m], impli_by_size[j][n])) {   // (string& s_small, string& s_large)

                    }
                    prime_impli.push(impli_by_size[i][m]);
                }
        }
    }

/*
    int flag = 1;
    while (flag) {
        flag = 0;
        for (int i = 0; i < nSize; i++) {
            // sort by the number of 1 for each size
            vector<set<string>> impli_sort_by_1(nVar + 1);  // ????????????
            set<string>::iterator itr;
            for (itr = impli_by_size[i].begin(); itr != impli_by_size[i].end(); itr++) {
                int cnt = 0;
                for (int j = 0; j < nVar; j++) {
                    if ((*itr)[j] == '1')
                        cnt++;
                }
                impli_sort_by_1[cnt].insert(*itr);
            }
        }
    }

*/

    


/*
    int flag = 1;       // determine whether there exist terms that can be merged
    while (flag) {
        flag = merge_implicants_in_two_sets(new_impli_table, impli_sort_by_1[4], impli_sort_by_1[5]);
    }

    vector<set<string>> new_impli_table;
    for (int i = 0; i < nVar - 1; i++) {
        merge_implicants_in_two_sets(new_impli_table, impli_sort_by_1[i], impli_sort_by_1[i + 1]);
    }
    
    int i = 0;
    for (auto& myset : new_impli_table) {
        cout << i++ << ":" << endl;
        for (auto& mystr : myset)
            cout << mystr << endl;
    }
*/

}

string Quine_McCluskey::merge(string& s_small, string& s_large)
{
    int cnt = 0, ind = -1;
    string newstr;
    for (int i = 0; i < nVar && cnt <= 1; i++) {
        if (s1[i] != s2[i]) {
            cnt++;
            ind = i;
        }
    }
    if (cnt == 1) {
        newstr = s1;
        newstr[ind] = '-';
    }
    if (cnt == 0) {
        newstr = "same";
    }
    return newstr;
}

bool Quine_McCluskey::can_merge(string& s1, string& s2)
{
    int s1_has_
    bool can_emrge = true;
    for (int i = 0; i < nVar && can_emrge; i++) {
        if (s1[i] != s2[i] && s2[i] != '-')
            can_emrge = flase;
        }
    }
    return can_emrge;
}
/*
void Quine_McCluskey::sort_by_num_of_1()
{

}

int Quine_McCluskey::merge_implicants_in_two_sets(vector<set<string>>& new_impli_table, set<string>& set1, set<string>& set2)
{
    set<string> new_str_set;
    for (auto& s1 : set1) {
        for (auto& s2 : set2) {
            int cnt = 0, ind = -1;
            for (int i = 0; i < nVar && cnt <= 1; i++) {
                if (s1[i] != s2[i]) {
                    cnt++;
                    ind = i;
                }
            }
            if (cnt == 1) {
                string newstr = s1;
                newstr[ind] = '-';
                new_str_set.insert(newstr);
            }
            if (cnt == 0) throw "error: same implicants when merge implicants";
        }
    }
    new_impli_table.push_back(new_str_set);
}
*/



void Quine_McCluskey::solve()
{
    gen_prime_implicant();
    column_covering();
}

int main(int argc, char* argv[])
{
    Quine_McCluskey problem;

    char file_name[] = "../testcases/case52.in";
    problem.read_input_data(file_name);
    problem.print_implicant();

    cout << endl;
    problem.gen_prime_implicant();

    return 0;
}

