#pragma once

#include <vector>
#include <set>
#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>

#include "myLogic_64bits.h"

#define ull unsigned long long
#define ui unsigned int

class Sup_pos{
    // MOps must be the same length
private:
    int n_vars;
    std::vector<MOp> mops;
    std::vector<std::vector<ui> > preproc_mops;
    std::set<ull> distincts;
    std::vector<MOp> get_MOps_from_file(std::string filename);
    bool inMOps(MOp mop);
    void combinations(int index, MOp &mop, std::vector<int> &cmb, std::vector<ui> &preproc_mop, std::vector<MOp> &new_mops, std::set<ull> &new_distincts, std::ofstream &new_mop_details);
    void combinations(int index, MOp &mop, std::vector<int> &cmb, std::vector<ui> &preproc_mop, std::vector<MOp> &new_mops, std::set<ull> &new_distincts);
public:
    Sup_pos(int nvars, std::vector<MOp> m_ops);
    Sup_pos(int nvars, std::string filename);
    std::vector<MOp> get_new_MOps(MOp mop, std::vector<ui> &preproc_mop);
    std::vector<MOp> get_new_MOps(MOp mop, std::vector<ui> &preproc_mop, std::ofstream &new_mop_details, std::ofstream &new_mop);
    bool closure_step(std::ofstream &new_mop_details, std::ofstream &new_mop);
    bool closure_step();
    void closure_full(std::ofstream &new_mop_details, std::ofstream &new_mop, bool step_counter_on);
    void closure_full();
    std::vector<MOp> get_MOps();
    void args_up_from2to3();
    bool inSup_pos(MOp mop);
    bool inSup_pos(Sup_pos sp);
};
