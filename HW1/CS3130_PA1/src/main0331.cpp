#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <set>
#include <algorithm>
#include <functional>
using namespace std;

class Quine_McCluskey
{
public:
    Quine_McCluskey() {};
    ~Quine_McCluskey() {};
    void read_input_data(string file_name);
    void gen_prime_implicant();         // still not prime !!!
    void column_covering();
    void solve();
    void print_implicant() const;
    void print_prime_implicant() const; 
    void print_result() const;
    void gen_output_file(string file_name) const;
private:
    int nVar, nProdTerm;
    bool sort_by_num_of_dc(const string&, const string&);   // sort from a large one to a small one
    bool sort_by_num_of_1(const string&, const string&);
    string merge(const string&, const string&);
    void merge_implicnats_in_one_set(vector<set<string>>&, int);
    void merge_implicants_in_two_sets(vector<set<string>>&, int, int);
    bool can_merge(const string&, const string&) const;
    bool can_cover(const string&, const string&) const;     // now is the same as can_merge ##########
    vector<string> impli;           // implicants, input data
    vector<string> prime_impli;     // prime implicants
    vector<string> result;          // optimization result
};

void Quine_McCluskey::read_input_data(string file_name)
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

void Quine_McCluskey::print_implicant() const
{
    cout << "the number of product terms = " << nProdTerm << endl;
    cout << "the number of variables = " << nVar << endl;
    for (int i = 0; i < nProdTerm; i++)
        cout << impli[i] << endl;
}

void Quine_McCluskey::print_prime_implicant() const
{
    for (auto& str : prime_impli)
        cout << str << endl;
}

void Quine_McCluskey::print_result() const
{
    cout << "optimization result" << endl;
    int size = result.size();
    for (int i = 0; i < size; i++) {
        cout << result[i] << endl;
    }

}

void Quine_McCluskey::gen_prime_implicant()
{
    int nSize = nVar + 1;
    
    // ***** sort by the size (# of dc) of implicant *****
    vector<set<string>> impli_by_size(nSize);
    for (int i = 0; i < nProdTerm; i++) {
        int cnt = 0;
        for (int j = 0; j < nVar; j++) {
            if (impli[i][j] == '-')
                cnt++;
        }
        impli_by_size[cnt].insert(impli[i]);
    }


    cout << "after sorting by size" << endl;
    for (int i = 0; i < nSize; i++) {
        cout << i << "'s dc:" << endl;
        for (auto& str : impli_by_size[i])
            cout << "  " << str << endl;
    }
    cout << endl;



    // ***** merge implicants with the same size  *****
    for (int i = 0; i < nSize; i++) {
        if (!impli_by_size[i].empty())
            merge_implicnats_in_one_set(impli_by_size, i);
    }


    // ***** merge implicants across different sizes  *****
    // one for loop
    // Here gen prime imple !!
    for (int i = 0; i < nSize - 1; i++) {
        if (!impli_by_size[i].empty()) {
            if (!impli_by_size[i + 1].empty()) {
                merge_implicants_in_two_sets(impli_by_size, i, i + 1);
                merge_implicnats_in_one_set(impli_by_size, i + 1);
            } else {
                for (auto& str : impli_by_size[i]) {
                    prime_impli.push_back(str);
                }

            }
        }
    }

    // ***** merge implicants across different sizes  *****
    // two for loop
    /*
    for (int i = 0; i < nSize - 1; i++) {
        for (int j = 0; j < nSize - 1; j++) {
            if (!impli_by_size[i].empty()) {
                if (!impli_by_size[j].empty()) {
                    merge_implicants_in_two_sets(impli_by_size, i, j);
                    merge_implicnats_in_one_set(impli_by_size, j);
                } else {
                    for (auto& str : impli_by_size[i]) {
                        prime_impli.push_back(str);
                    }
                }
            }
        }
    }
    */
/*
    cout << "after merging" << endl;
    for (int i = 0; i < nSize; i++) {
        cout << i << "'s dc:" << endl;
        for (auto& str : impli_by_size[i])
            cout << "  " << str << endl;
    }
    cout << endl;
*/

    // ***** sort prime implicants, from large to small *****
    sort(prime_impli.begin(), prime_impli.end(), [this](const string& s1, const string& s2){
        return this->sort_by_num_of_dc(s1, s2);
    });

}

string Quine_McCluskey::merge(const string& s1, const string& s2)
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
        cout << "error: same string" << endl;
        throw "error: merge, find same strings";
    }
    return newstr;
}

void Quine_McCluskey::merge_implicnats_in_one_set(vector<set<string>>& impli_by_size, int impli_size)
{
    if (impli_by_size[impli_size].empty()) return;

    set<string> &s = impli_by_size[impli_size];
/*    int size_of_impli;
    for (int i = 0; i < nVar; i++) {
        if ((*s.begin())[i] == '-') size_of_impli++;
    }
*/
    int set_size = s.size();
    vector<bool> flag(set_size); 
    for (int i = 0; i < set_size; i++)
        flag[i] = false; // 0: cannot be emerged, 1: can be emerge
    //set<string> temp;
    set<string>::iterator it1, it2;
    //int i = 0, j = 0;
    for (it1 = s.begin(); it1 != s.end(); it1++) {
        for (it2 = next(it1); it2 != s.end(); it2++) {
            string str = merge(*it1, *it2);
            if (!str.empty()) {
                //flag[i] = 1;
                //flag[j] = 1;
                impli_by_size[impli_size + 1].insert(str);
                cout << "merge " << *it1 << " and " << *it2 << ", get " << str
                     << " , insert in " << impli_size + 1 << endl;
            }
        }
        //if (flag[i] == 0)
        //    prime_impli.push_back(*it1);      // temp.insert(*it1);
    }
    //s = temp;

}

void Quine_McCluskey::merge_implicants_in_two_sets(vector<set<string>>& impli_by_size, int impli_size_1, int impli_size_2)
{
    // set1: smaller impli, set2: larger impli
    set<string> &set1 = impli_by_size[impli_size_1];
    set<string> &set2 = impli_by_size[impli_size_2];

    //if (set1.empty() || set2.empty()) return;
    
    set<string>::iterator it1, it2;
    int set1_size = set1.size();
    vector<bool> flag(set1_size);
    string newstr(nVar, '\0'), inistr(nVar, '\0'); // nVar + 1??

    for (int i = 0; i < set1_size; i++)
        flag[i] = false; 
    int i = 0;
    for (it1 = set1.begin(), i = 0; it1 != set1.end(); it1++, i++) {
        for (it2 = set2.begin(); it2 != set2.end(); it2++) {
            int cnt = 0, dc = 0;      // cnt: count for 0 1 pair, cnt at most 1
            newstr = inistr;
            for (int k = 0; k < nVar && cnt <= 1; k++) {
                if ((*it1)[k] == (*it2)[k]) {
                    if ((*it1)[k] == '-') dc++;
                    newstr[k] = (*it1)[k];
                } else {
                    if ((*it1)[k] == '-') {
                        cnt = -1;
                        break;
                    } else if ((*it2)[k] == '-') {
                        dc++;
                        newstr[k] = (*it1)[k];
                    } else {      // (*it1)[k] == (*it2)[k], and (*it2)[k] == 0 or 1
                        cnt++;
                        newstr[k] = '-';
                    }
                }
            }
            if (cnt == 1) {     // one 0, 1 pair
                cout << "merge " << *it1 << " and " << *it2 << ", get " << newstr << " , insert in " << dc << endl;
                impli_by_size[dc].insert(newstr);
                flag[i] = true; // can be merged
            }
            if (cnt == 0) {     // only diff at where (*it2) = '-' (or completely the smae, but it's error).
                flag[i] = true; // can be merged
            }
        }
        if (flag[i] == 0)       // can not be merged, push into prime impli
            prime_impli.push_back(*it1);
    }


}


bool Quine_McCluskey::can_merge(const string& s_small, const string& s_large) const 
{
    for (int i = 0; i < nVar; i++) {
        if (s_small[i] != s_large[i] && s_large[i] != '-') 
            return false;
    }
    return true;
}

bool Quine_McCluskey::can_cover(const string& s1, const string& s2) const    // impli, prime_impli
{
    for (int i = 0; i < nVar; i++) {
        if (s1[i] != s2[i] && s2[i] != '-')
            return false;
    }
    return true;
}

bool Quine_McCluskey::sort_by_num_of_dc(const string& s1, const string& s2)
{
    int cnt_1 = 0 , cnt_2 = 0;
    for (int i = 0; i < nVar; i++) {
        if (s1[i] == '-') cnt_1++;
        if (s2[i] == '-') cnt_2++;
    }
    return (cnt_1 > cnt_2);     // sort from a large one to a small one
}


bool Quine_McCluskey::sort_by_num_of_1(const string& s1, const string& s2)
{
    int cnt_1 = 0 , cnt_2 = 0;
    for (int i = 0; i < nVar; i++) {
        if (s1[i] == '1') cnt_1++;
        if (s2[i] == '1') cnt_2++;
    }
    return (cnt_1 < cnt_2);
}

void Quine_McCluskey::column_covering()
{
    vector<vector<int>> col_cov_by(nProdTerm);
    vector<vector<int>> row_cov(prime_impli.size());
    vector<int> len_of_vec(nProdTerm);
    vector<bool> is_covered(nProdTerm);

    for (int i = 0; i < nProdTerm; i++) {
        len_of_vec[i] = 0;
        is_covered[i] = false;
    }

    // establish both tables
    for (int i = 0; i < nProdTerm; i++) {
        for (int j = 0; j < prime_impli.size(); j++) {
            if (can_cover(impli[i], prime_impli[j])) {
                col_cov_by[i].push_back(j);
                row_cov[j].push_back(i);
            }
        }
    }



    // ***** essential prime implicant *****
    for (int i = 0; i < nProdTerm; i++) {
        if (!is_covered[i]) {
            len_of_vec[i] = col_cov_by[i].size();
            if (len_of_vec[i] == 0) {
                cout << "error" << endl;
                //throw "error";
            }
            if (len_of_vec[i] == 1) {
                is_covered[i] = true;
                int j = col_cov_by[i][0];       // the only one
                result.push_back(prime_impli[j]);
                cout << "col " << i << ", ess prime impli " << j << " " << prime_impli[j] << ", cover col";
                for (int k : row_cov[j]) {
                    is_covered[k] = true;
                    cout << " " << k;
                }
                cout << endl;
            }
        }
    }

    // ***** column covering from the implicants that's covered by the least prime implicants *****
    int len_of_vec_now = 1;
    bool flag = true;
    while (flag) {
        flag = 0;
        len_of_vec_now++;
        for (int i = 0; i < nProdTerm; i++) {
            if (!is_covered[i]) {
                flag = 1;
                if (len_of_vec[i] == len_of_vec_now) {
                    is_covered[i] = true;
                    int j = col_cov_by[i][0];   // the largest one (check?)  Is it brings best benefit?
                    result.push_back(prime_impli[j]);
                    cout << "col " << i << ", prime impli " << j << " " << prime_impli[j] << ", cover col";
                    for (int k : row_cov[j]) {         
                        is_covered[k] = true;
                        cout << " " << k;
                    }
                    cout << endl;
                }
            }
        }
    }

    // ***** from prime implicant that's with a larger size *****

}

void Quine_McCluskey::gen_output_file(string file_name) const
{
    int literal_cnt = 0;
    int num_product_term = result.size();

    for (int i = 0; i < num_product_term; i++) {
        for (int j = 0; j < nVar; j++) {
            if (result[i][j] != '-')
                literal_cnt++;
        }
    }

    ofstream outputFile(file_name);
    if (outputFile.is_open()) {
        outputFile << literal_cnt << endl;
        outputFile << num_product_term << endl;
        for (int i = 0; i < num_product_term; i++) {
            outputFile << result[i] << endl;
        }
        outputFile.close();
    } else
        throw "error: gen_output_file";    
}

void Quine_McCluskey::solve()
{
    gen_prime_implicant();
    column_covering();
}

int main(int argc, char* argv[])
{
    Quine_McCluskey problem;

    //char file_name[] = "../testcases/case00.in";
    //problem.read_input_data(file_name);
    
    cout << "read input" << endl;
    //string filename = "testcases/";
    string filename = "../testcases/";
    filename = filename + argv[1];
    problem.read_input_data(filename);

    cout << "gen prime implicant" << endl;
    problem.gen_prime_implicant();

    cout << "prime implicant" << endl;
    problem.print_prime_implicant();

    cout << "column covering" << endl;
    problem.column_covering();
    
    //problem.solve();
    //problem.print_result();

    cout << "gen output file" << endl;
    filename = "../verifier/";
    filename = filename + argv[2];
    problem.gen_output_file(filename);

    return 0;
}

