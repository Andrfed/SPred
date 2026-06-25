#pragma once

#include <vector>
#include <set>
#include <utility>
#include <string>
#include <algorithm>
#include <fstream>
#include "myLogic_64bits.h"

#define ull unsigned long long
#define ui unsigned int

class Predicate_saving{
    private:
        std::vector<ui> p;
        std::set<ui> distincts;
        ui n;
        ui k;
        bool inPred(ui col);
        std::pair<bool, std::vector<ULLString> > combinations(int index, MOp &mop, std::vector<int> &cmb);
        std::pair<bool, std::vector<ULLString> > combinations(int index, MOp &mop, std::vector<int> &cmb, std::vector<ui> &preproc_mop);
    public:
        Predicate_saving(std::vector<ui> pred, ui pred_k);
        std::pair<bool, std::vector<ULLString> > isSave(MOp mop);
        std::pair<bool, std::vector<ULLString> > isSave(MOp mop, std::vector<ui> &preproc_mop);
        std::string to_string();
        static Predicate_saving get_Predicate_from_file(int n_vars, std::string filename);
};
