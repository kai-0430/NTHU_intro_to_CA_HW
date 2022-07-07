#include <iostream>
#include <vector>
#include <fstream>
#include <set>
#include <list>
#include <cmath>
#include <algorithm>
using namespace std;

class Net {
friend class FM_algorithm;
public:
    Net() { n_pins = 0; }
private:
    int n_pins;
    vector<int> C;      // the cells it connected
};

class Cell {
friend class FM_algorithm;
public:
    Cell() { n_pins = 0; size = 0; group = -1; }
private:
    int n_pins;
    int size;
    int group;
    vector<int> N;      // the nets it connected
};

class Step {
friend class FM_algorithm;
public:
    Step(int _move_cell = -1, int _g = 0, int _G = 0, int _F = -1, int _T = -1, int _balancy = 0) {
        move_cell = _move_cell;
        g = _g;
        G = _G;
        F = _F;
        T = _T;
        balancy = _balancy;
    }
private:
    int move_cell;
    int F;              // from partition
    int T;              // to partition
    int g;              // gain
    int G;              // partial sum of gain
    int balancy;        // new_T_cell_size - total_size / k, the smaller the better
};

class FM_algorithm
{
public:
    FM_algorithm() {}
    void read_input_data(string file_path);
    void gen_output_file(string file_path) const;
    void judge_way();
    void construct_cell_array();
    void initial_partition();
    void compute_initial_info();   // net_distri, gain, partition_size
    int choose_base_cell();
    void move_and_update();
    void solve();
    int evaluate() const;
    void print() const;
    void print_step_list() const;
private:
    int max_area;
    int n_cells;
    int n_nets;
    int total_size = 0;
    int k;                      // k-way
    int G_max;                  // max partial sum of gain (initialize in each iteration)
    int nth_step = 0;
    int nth_ite = 0;
    int evaluated_cost;
    vector<Cell> cell;
    vector<Net> net;
    vector<int> gain;           // the gain of each cell (initialize in each iteration)
    vector<int> cell_gr;        // the group of each cell (initialize in each iteration)
    vector<int> locked_cell;    // the locked cell (initialize in each iteration)
    vector<int> partition_size;
    vector<vector<int>> net_distri; 
    list<Step> step_list;       // log of each step (initialize in each iteration)
};



void FM_algorithm::read_input_data(string file_path)
{
    ifstream inputFile;

    inputFile.open(file_path);
    if (inputFile.is_open())
    {
        string tmp;
        inputFile >> max_area;
        inputFile >> tmp;
        inputFile >> n_cells;
        cell = vector<Cell>(n_cells);
        for (int i = 0; i < n_cells; i++)
            inputFile >> tmp >> cell[i].size;
        inputFile >> tmp;
        inputFile >> n_nets;
        net = vector<Net>(n_nets);
        for (int i = 0; i < n_nets; i++)
        {
            inputFile >> net[i].n_pins;
            net[i].C = vector<int>(net[i].n_pins);
            for (int j = 0; j < net[i].n_pins; j++)
                inputFile >> net[i].C[j];
        }
        gain = vector<int>(n_cells, 0);
        cell_gr = vector<int>(n_cells, 0);
        locked_cell = vector<int>(n_cells, 0);
        net_distri = vector<vector<int>>(n_nets); 
        inputFile.close();
    } else
        cout << "error: read_input_data\n";
    
}

void FM_algorithm::construct_cell_array()
{
    for (int i = 0; i < n_nets; i++)
    {
        for (int j = 0; j < net[i].n_pins; j++)
        {
            int index = net[i].C[j];
            cell[index].n_pins++;
            cell[index].N.push_back(i);
        }
    }
}

void FM_algorithm::judge_way()
{
    total_size = 0;
    for (int i = 0; i < n_cells; i++)
        total_size += cell[i].size;
    if (max_area > total_size / 2)
        k = 2;
    else
        k = total_size / max_area + 1;
}

void FM_algorithm::initial_partition()
{
    int sum;
    vector<pair<int,int>> net_total_size(n_nets);

    for (int i = 0; i < n_nets; i++)
    {
        sum = 0;
        for (auto index : net[i].C)
            sum += cell[index].size;
        net_total_size[i].first = i;
        net_total_size[i].second = sum;
    }

    sort(net_total_size.begin(), net_total_size.end(), [](pair<int, int> a, pair<int, int> b) {
        return a.second < b.second;
        });
    fill(locked_cell.begin(), locked_cell.end(), 0);
    

    if (k == 2)
    {
        partition_size = vector<int>(2, 0);
        for (int i = 0; i < n_nets; i++)
        {
            int net_ind = net_total_size[i].first;
            for (int c : net[net_ind].C)
            {
                if (!locked_cell[c])
                {
                    if (partition_size[0] + cell[c].size <= max_area)
                    {
                        cell[c].group = 0;
                        partition_size[0] += cell[c].size;
                        locked_cell[c] = 1;
                    }
                    else if (partition_size[1] + cell[c].size <= max_area)
                    {
                        cell[c].group = 1;
                        partition_size[1] += cell[c].size;
                        locked_cell[c] = 1;
                    }
                    else
                    {
                        cell[c].group = 1;
                        partition_size[1] += cell[c].size;
                        locked_cell[c] = 1;
                    }
                }
            }
        }

        int i = 0;
        while (partition_size[0] > max_area) // && i < n_cells
        {
            if (partition_size[1] + cell[i].size < max_area)
            {
                cell[i].group = 1;
                partition_size[0] -= cell[i].size;
                partition_size[1] += cell[i].size;
            }
            i++;
        }
    }
    else
    {
        //partition_size = vector<int>(k, 0);
        int tmp_size = 0;
        int g = 0;          // group

        for (int i = 0; i < n_nets; i++)
        {
            int net_ind = net_total_size[i].first;
            for (int c : net[net_ind].C)
            {
                if (!locked_cell[c])
                {
                    if (tmp_size + cell[c].size <= max_area)
                    {
                        cell[c].group = g;
                        tmp_size += cell[c].size;
                        locked_cell[c] = 1;
                    }
                    else
                    {
                        g++;
                        cell[c].group = g;
                        tmp_size = cell[c].size;
                        locked_cell[c] = 1;
                    }
                }
            }
        }
        k = g + 1;
    }
}

void FM_algorithm::compute_initial_info()
{
    /* Get the distribution of cells. */
    for (int i = 0; i < n_nets; i++)
    {
        for (int j : net[i].C)
        {
            if (cell[j].group == 0) net_distri[i][0]++;
            if (cell[j].group == 1) net_distri[i][1]++;
        }
    }
        
    /* Compute the partition size and the initial gain of each cell. */
    for (int i = 0; i < n_cells; i++)
    {
        int F = cell[i].group;          // from cell
        int T = !F;                     // to cell
        partition_size[F] += cell[i].size;
        for (int net_ind : cell[i].N)
        {
            if (net_distri[net_ind][F] == 1) gain[i]++;
            if (net_distri[net_ind][T] == 0) gain[i]--;
        }
    }
    //total_size = partition_size[0] + partition_size[1];
}

int FM_algorithm::choose_base_cell()
{
    int state = 1;
    int move_cell = -1;
    vector<pair<int, int>> _gain(n_cells - nth_step);    // first = cell index, second = gain


    /* Construct a new gain array and sort it descendingly by the gain values. */
    int n = 0;
    for (int i = 0; i < n_cells; i++)
    {
        if (!locked_cell[i])
        {
            pair<int, int> tmp(i, gain[i]);
            _gain[n++] = tmp;
        }
    }
    std::sort(_gain.begin(), _gain.end(), [](pair<int, int> a, pair<int, int> b) {
        return a.second > b.second;
        });
    // for (auto g : _gain)
    //     cout << g.first << " " << g.second << endl;

    /* Choose the cell to be moved. */
    unsigned int ind = 0;
    int gain_now;
    while (move_cell == -1)
    {
        /* Choose the candidate cells to be moved. */
        vector<int> candidate;
        gain_now = _gain[ind].second;
        for ( ; ind < _gain.size(); ind++)
        {
            if (_gain[ind].second == gain_now)
                candidate.push_back(_gain[ind].first);  // all the candidates are with the same gain
            else {
                //ind++;    // no need, the last loop terminates after ind++
                break;
            }
        }

        /* determine by number of pins */
        int max = INT32_MIN;
        for (unsigned int i = 0; i < candidate.size(); i++)
        {
            if (cell[candidate[i]].n_pins > max)
            {
                int F = cell_gr[candidate[i]];      // from cell
                int T = !F;                         // to cell
                int new_size = partition_size[T] + cell[candidate[i]].size;
                if (new_size < max_area)
                {
                    max = cell[candidate[i]].n_pins;
                    move_cell = candidate[i];
                } 
            }
        }
   
        /* the check for that there is no cell can be moved w/o violating the constraint */
        if (move_cell == -1 && gain[candidate[0]] == 0) 
            break;
    }

    /* Write into the step list. */
    if (move_cell == -1)
    {
        cout << "Stops since no movement satisfies the area constraint.\n";
        state = 0;
    }
    else
    {
        //cout << move_cell << " " << gain[move_cell] << " " << cell_gr[move_cell] << endl; // del
        int G = step_list.back().G + gain_now;
        if (G > G_max) G_max = G;
        int F = cell_gr[move_cell];     // from cell
        int T = !F;                     // to cell
        int balancy = std::abs(partition_size[T] + cell[move_cell].size - total_size / k);
        Step step(move_cell, gain_now, G, F, T, balancy);
        step_list.push_back(step);
        nth_step++;
        locked_cell[move_cell] = 1;

        for (int i = 0; i < n_cells; i++)
            if (!locked_cell[i]) return 1;  // if there is any cell unlocked, keep going
        state = 0;
    }
    return state;
}

void FM_algorithm::move_and_update()
{
    int base_cell = step_list.back().move_cell;
    int F = cell_gr[base_cell];
    int T = !F;

    for (int net_ind : cell[base_cell].N)
    {
        /* before movement  */
        if (net_distri[net_ind][T] == 0)
        {
            for (int cell_ind : net[net_ind].C)
                gain[cell_ind]++;
        }
        if (net_distri[net_ind][T] == 1)
        {
            int a;
            for (int cell_ind : net[net_ind].C)
                if (cell_gr[cell_ind] == T && cell_ind != base_cell) {
                    a = cell_ind;
                    break;
                }
            gain[a]--;
        }

        /* move */
        cell_gr[base_cell] = T;
        partition_size[F] -= cell[base_cell].size;
        partition_size[T] += cell[base_cell].size;
        net_distri[net_ind][F]--;
        net_distri[net_ind][T]++;
        
        /* after movement */
        if (net_distri[net_ind][F] == 0)
        {
            for (int cell_ind : net[net_ind].C)
                gain[cell_ind]--;
        }
        if (net_distri[net_ind][F] == 1)
        {
            int a;
            for (int cell_ind : net[net_ind].C)
                if (cell_gr[cell_ind] == F && cell_ind != base_cell) {
                    a = cell_ind;
                    break;
                }
            gain[a]++;
        }
    }
}

void FM_algorithm::solve()
{
    int _0_tolerence = 0;   // allow some G == 0
    int _0_times = 0;

    judge_way();
    initial_partition();

    if (k == 2)
    {
        construct_cell_array();

        if (n_cells < 100)
            _0_tolerence = 3;
            
        G_max = 1;
        nth_ite = 0;
        while (G_max >= 0 && _0_times <= _0_tolerence)
        {
            vector<pair<int, int>> critical_steps;   // first = ith step, second = cell_size;
            int state;

            cout << ++nth_ite << "th iteration:\n";

            /* initialization */
            nth_step = 0;
            G_max = INT32_MIN;
            partition_size = vector<int>(k, 0);     // k-way, k partitions
            step_list.clear();
            step_list.push_back(Step(-1, 0, 0));    // dummy step
            std::fill(locked_cell.begin(), locked_cell.end(), 0);
            std::fill(gain.begin(), gain.end(), 0);
            for (int i = 0; i < n_cells; i++)
                cell_gr[i] = cell[i].group;
            for (int i = 0; i < n_nets; i++)
                net_distri[i] = vector<int>(k, 0);

            /* first movement */
            compute_initial_info();
            state = choose_base_cell();

            /* the following movements */
            while (state) {
                move_and_update();
                state = choose_base_cell();
            }

            /* decide the movements in this iteration */
            list<Step>::iterator it;
            int i = 1;
            for (it = ++step_list.begin(); it != step_list.end(); it++, i++)
            {
                if (it->G == G_max)
                    critical_steps.push_back(pair<int,int>(i, it->balancy));
            }

            int steps = 0, min = INT32_MAX;
            for (auto p : critical_steps)
            {
                if (p.second < min) {
                    min = p.second;
                    steps = p.first;
                }
            }
            /* new partition */
            for (i = 0, it = ++step_list.begin(); i < steps; it++, i++)
                cell[it->move_cell].group = it->T;

            if (G_max == 0)
                _0_times++;
            else
                _0_times = 0;
            //cout << "_0_times: " << _0_times << endl;
        }
    }


    for (int i = 0; i < n_nets; i++)
        net_distri[i] = vector<int>(k, 0);

    for (int i = 0; i < n_nets; i++)
    {
        int g;
        for (int j : net[i].C)
        {
            g = cell[j].group;
            net_distri[i][g]++;
        }
    }
    
    evaluated_cost = evaluate();
    //cout << "cost = " << evaluated_cost << endl;
}

int FM_algorithm::evaluate() const
{
    int totalC, C;
    int span;

    totalC = 0;
    for (int i = 0; i < n_nets; i++)
    {
        span = 0;
        for (int j = 0; j < k; j++)
        {
            if (net_distri[i][j] > 0)
                span++;
        }
        C = (span - 1) * (span - 1);
        totalC += C;
    }
    return totalC;
}


void FM_algorithm::print() const
{
    cout << "print\n";
    cout << max_area << endl;
    cout << ".cells\n" <<  n_cells << endl;
    for (int i = 0; i < n_cells; i++)
    {
        cout << i << " group=" << cell[i].group << endl;
        //cout << i << " " << cell[i].size << endl;
        // cout << cell[i].n_pins << endl;
        // for (int j = 0; j < cell[i].n_pins; j++)
        //     cout << cell[i].N[j] << " ";
        // cout << endl;
    }

    cout << ".nets\n" << n_nets << endl;
    for (int i = 0; i < n_nets; i++)
    {
        cout << net[i].n_pins << endl;
        for (int j = 0; j < net[i].n_pins; j++)
            cout << net[i].C[j] << " ";
        cout << endl;
    }

}

void FM_algorithm::print_step_list() const
{
    cout << "print step list\n";
    for (auto step : step_list) {
        cout << "move:" << step.move_cell << ", g=" << step.g << ", G=" << step.G
            << ", F=" << step.F << ", T=" << step.T << ", balancy=" << step.balancy << endl;
    }
}

void FM_algorithm::gen_output_file(string file_path) const
{
    ofstream outputFile;

    outputFile.open(file_path);
    if (outputFile.is_open()) {
        outputFile << evaluated_cost << endl;
        outputFile << k << endl;
        for (int i = 0; i < n_cells; i++) {
            outputFile << cell[i].group << endl;
        }
        outputFile.close();
    } else
        cout << "error: gen_output_file\n";
}

int main(int argc, char* argv[])
{
    FM_algorithm problem;

    problem.read_input_data(argv[1]);
    problem.solve();
    problem.gen_output_file(argv[2]);

    return 0;
}