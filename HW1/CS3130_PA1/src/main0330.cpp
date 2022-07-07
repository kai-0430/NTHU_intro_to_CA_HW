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
    void read_input_data(char *file_name);
    void gen_prime_implicant();         // still not prime !!!
    //int merge_implicants_in_two_sets(vector<set<string>>& new_impli_table, set<string>& set1, set<string>& set2);
    void column_covering();
    void solve();
    void print_implicant() const;
    void print_prime_implicant() const; 
    void print_result() const;
    void gen_output_file(char *file_name) const;
private:
    int nVar, nProdTerm;
    bool sort_by_num_of_dc(const string&, const string&);   // sort from a large one to a small one
    bool sort_by_num_of_1(const string&, const string&);
    string merge(const string&, const string&);
    bool can_merge(const string&, const string&) const;
    bool can_cover(const string&, const string&) const;     // now is the same as can_merge ##########
    vector<string> impli;           // implicants, input data
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


    cout << "after sort by size" << endl;
    for (int i = 0; i < nSize; i++) {
        cout << i << "'s dc:" << endl;
        for (auto& str : impli_by_size[i])
            cout << "  " << str << endl;
    }
    cout << endl;


    // ***** merge implicants with the same size  *****
    int size_now = -1;
    for (auto& s : impli_by_size) {
        size_now++;
        if (!s.empty()) {
            int size = s.size();
            vector<bool> flag(size);
            for (int i = 0; i < size; i++)
                flag[i] = false;            // 0: cannot be emerged, 1: can be emerged, 2: repeat
            //set<string> temp;
            set<string>::iterator it1, it2;
            int i = 0, j = 0;
            for (it1 = s.begin(), i = 0; it1 != s.end(); it1++, i++) {
                for (it2 = next(it1), j = i + 1; it2 != s.end(); it2++, j++) {
                    string str = merge(*it1, *it2);
                    if (!str.empty()) {
                        flag[i] = 1;
                        flag[j] = 1;
                        impli_by_size[size_now + 1].insert(str);
                    }
                }
                if (flag[i] == 0)
                    prime_impli.push_back(*it1);      // temp.insert(*it1);
            }
            //s = temp;
        }
    }


    sort(prime_impli.begin(), prime_impli.end(), [this](const string& s1, const string& s2){
        return this->sort_by_num_of_dc(s1, s2);
    });

    cout << "prime_implicant" << endl;
    print_prime_implicant();
    cout << endl;

}        
/*

    for (int i = 0; i < nSize; i++) {
        cout << i << "'s dc:" << endl;
        for (auto& str : impli_by_size[i])
            cout << "  " << str << endl;
    }
    cout << endl;
*/




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
        newstr = "same";
    }
    return newstr;
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

/*

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
        len_of_vec[i] = col_cov_by[i].size();
        if (len_of_vec[i] == 0) throw "error";
        if (len_of_vec[i] == 1) {
            is_covered[i] = true;
            int j = col_cov_by[i][0];       // the only one
            result.push_back(prime_impli[j]);
            for (int k : row_cov[j])
                is_covered[k] = true;
        }
    }

    // ***** coumn covering from the implicants that's covered by the least prime implicants *****
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
                    for (int k : row_cov[j])          
                        is_covered[k] = true;
                }
            }
        }
    }

    // ***** from prime implicants that's with larger size *****

}

void Quine_McCluskey::gen_output_file(char *file_name) const
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
    cout << argv[1] << endl;
    problem.read_input_data(argv[1]);

    problem.print_implicant();
    cout << endl;
    
    problem.solve();
    problem.print_result();
    problem.gen_output_file(argv[2]);

    return 0;
}

