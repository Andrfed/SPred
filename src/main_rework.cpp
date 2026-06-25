#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <set>
#include <random>

#include "myLogic_64bits.h"
#include "Predicate_saving.h"
#include "Sup_pos.h"

#define ull unsigned long long
#define ui unsigned int

using namespace std;

void get_closure(int mops_args, string filename) {
    Sup_pos sp(mops_args, filename);
    ofstream new_mop_details("new_mops_details.txt");
    ofstream new_mop("new_mops.txt");

    auto start = std::chrono::high_resolution_clock::now();
    sp.closure_full(new_mop_details, new_mop, true);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "elapsed time: " << elapsed.count() << " ms, " << elapsed.count()/1000.0 << " s" << std::endl;

    ofstream closure_out("closure_out.txt");
    auto mops = sp.get_MOps();
    for (int i=0;i<mops.size();i++) {
        closure_out<<myLogic_64bits::MOp_to_string(mops[i])<<'\n';
    }
}

pair<string, vector<ULLString> > check_pred_1(Predicate_saving &p, Sup_pos &sp) {
    sp.closure_step();
    vector<MOp> mops = sp.get_MOps();
    for (int i=0;i<mops.size();i++) {
        cout<<myLogic_64bits::MOp_to_string(mops[i])<<endl;
    }
    for (int i=0;i<mops.size();i++) {
        vector<ui> p_mop = preprocess_to_MOp(mops[i]);
        auto res = p.isSave(mops[i], p_mop);
        if (!res.first)
            return {myLogic_64bits::MOp_to_string(mops[i]), res.second};
    }
    return {"", vector<ULLString>()};
}

void partial_pred_check(int pred_nvars, string pred_filename, int suppos_nvars, string suppos_filename) {
    Sup_pos sp(suppos_nvars, suppos_filename);
    vector<MOp> mops = sp.get_MOps();
    vector<MOp> p_mops;
    int P_mops_number = 4;
    std::sample(mops.begin(), mops.end(), std::back_inserter(p_mops), P_mops_number, std::mt19937{std::random_device{}()});
    for (int i=0;i<p_mops.size();i++) {
        cout<<myLogic_64bits::MOp_to_string(p_mops[i])<<endl;
    }
    Sup_pos p_sp(suppos_nvars, p_mops);

    //Sup_pos sp(suppos_nvars, suppos_filename);
    Predicate_saving p = Predicate_saving::get_Predicate_from_file(pred_nvars, pred_filename);
    auto res = check_pred_1(p, p_sp);
    //auto res = check_pred(p, p_sp);
    if (res.first == "")
        cout<<"Save";
    else
        cout<<res.first<<"\nNot save on:\n"<<linesToString(res.second);
}

void partial_new_check(int suppos_nvars, string suppos_filename) {
    Sup_pos sp(suppos_nvars, suppos_filename);
    vector<MOp> mops = sp.get_MOps();
    cout<<"Number of m.op.s in file: "<<mops.size()<<endl;
    int P_mops_number = 50;
    vector<MOp> p_mops;
    std::sample(mops.begin(), mops.end(), std::back_inserter(p_mops), P_mops_number, std::mt19937{std::random_device{}()});
    cout<<"Random "<<P_mops_number<<" m.op.s from file:"<<endl;
    for (int i=0;i<p_mops.size();i++) {
        cout<<"|"<<myLogic_64bits::MOp_to_string(p_mops[i])<<endl;
    }
    Sup_pos p_sp(suppos_nvars, p_mops);
cout<<"Begin closure step"<<endl;
    p_sp.closure_step();
    vector<MOp> n_mops = p_sp.get_MOps();
    cout<<"number of new m.op.s: "<<n_mops.size()<<endl;
    for (int i=0;i<n_mops.size();i++) {
        if (!sp.inSup_pos(n_mops[i]))
            cout<<myLogic_64bits::MOp_to_string(n_mops[i])<<" not in saves"<<endl;
    }
}

void get_closures_and_intersection(int mops_args, string suppos_filename, string mops_to_closure_filename) {
    Sup_pos sp(mops_args, suppos_filename);
    set<ull> intersection;

    Sup_pos iter_sp(mops_args, mops_to_closure_filename);
    for (auto mop : iter_sp.get_MOps()) {
        vector<MOp> t_mops = sp.get_MOps();
        cout<<"mop: "<<myLogic_64bits::MOp_to_string(mop)<<'\n';
        t_mops.push_back(mop);
        Sup_pos t_sp(mops_args, t_mops);
        t_sp.closure_full();
        if (intersection.size() == 0) {
            for (auto j : t_sp.get_MOps())
                intersection.insert(j.m_op);
        }
        else {
            set<ull> n_intersection;
            for (auto j : t_sp.get_MOps()) {
                if (intersection.find(j.m_op) != intersection.end()) {
                    n_intersection.insert(j.m_op);
                }
            }
            intersection = n_intersection;
        }
    }
    ofstream intersection_out("intersection_out.txt");
    for (auto mop : intersection) {
        MOp t_mop = {mops_args, mop};
        intersection_out<<myLogic_64bits::MOp_to_string(t_mop)<<'\n';
    }
}

void solve_is_save(Predicate_saving p, MOp mop) {
    auto p_mop = preprocess_to_MOp(mop);
    auto res = p.isSave(mop, p_mop);
    if (res.first) {
        cout<<"Save";
    }
    else {
        cout<<"Not Save on:\n"<<linesToString(res.second);
    }
}

void check_funcs(int pred_nargs, string pred_filename, string funcs_filename) {
    Predicate_saving p = Predicate_saving::get_Predicate_from_file(pred_nargs, pred_filename);
    ifstream in(funcs_filename);
    string current;
    while (in>>current) {
        int sz = current.size();
        if (sz && !(sz & (sz - 1))) {
            MOp mop = myLogic_64bits::create_multioperation(get_pow2(sz), current);
            cout<<current<<":\n";
            solve_is_save(p, mop);
            cout<<"\n";
        }
    }
}

void comp_preds_withcout(MOp begMOp, int pred1_nargs, string pred1_filename, int pred2_nargs, string pred2_filename) {
    ofstream compare_preds("compare_preds.txt");

    ofstream comp_saves_1("compare_preds_saves_1.txt");
    ofstream comp_saves_2("compare_preds_saves_2.txt");

    Predicate_saving p1 = Predicate_saving::get_Predicate_from_file(pred1_nargs, pred1_filename);
    Predicate_saving p2 = Predicate_saving::get_Predicate_from_file(pred2_nargs, pred2_filename);

    MOp mop = begMOp;
    MOp max_mop;
    max_mop.n_vars = begMOp.n_vars;
    max_mop.m_op = (1<<((1<<begMOp.n_vars) * 2));

    int counter = 0;
    do {
        counter++;
        cout<<myLogic_64bits::MOp_to_string(mop)<<'\n';
        compare_preds<<myLogic_64bits::MOp_to_string(mop)<<'\n';

        vector<ui> preproc_mop = preprocess_to_MOp(mop);
        auto res1 = p1.isSave(mop, preproc_mop);
        auto res2 = p2.isSave(mop, preproc_mop);
        cout<<res1.first<<" "<<res2.first<<'\n';
        compare_preds<<res1.first<<" "<<res2.first<<'\n';

        if (res1.first) {
            comp_saves_1<<myLogic_64bits::MOp_to_string(mop)<<'\n';
        }
        if (res2.first) {
            comp_saves_2<<myLogic_64bits::MOp_to_string(mop)<<'\n';
        }

        if ( (res1.first ^ res2.first) == 1 ) {
            if (res1.first) {
                cout<<"1 Save\n";
                compare_preds<<"1 Save\n";
            }
            else {
                cout<<"1 Not Save on:\n"<<linesToString(res1.second);
                compare_preds<<"1 Not Save on:\n"<<linesToString(res1.second);
            }
            if (res2.first) {
                cout<<"2 Save\n";
                compare_preds<<"2 Save\n";
            }
            else {
                cout<<"2 Not Save on:\n"<<linesToString(res2.second);
                compare_preds<<"2 Not Save on:\n"<<linesToString(res2.second);
            }
        }

        mop.m_op++;

        if (counter % 10 == 0) {
            cout<<"Iteration: "<<counter<<'\n';
        }
    } while (mop.m_op < max_mop.m_op);
}

void comp_preds_withcout(MOp begMOp, MOp endMOp, int pred1_nargs, string pred1_filename, int pred2_nargs, string pred2_filename) {
    ofstream compare_preds("compare_preds.txt");

    ofstream comp_saves_1("compare_preds_saves_1.txt");
    ofstream comp_saves_2("compare_preds_saves_2.txt");

    Predicate_saving p1 = Predicate_saving::get_Predicate_from_file(pred1_nargs, pred1_filename);
    Predicate_saving p2 = Predicate_saving::get_Predicate_from_file(pred2_nargs, pred2_filename);

    MOp mop = begMOp;

    int counter = 0;
    do {
        counter++;
        cout<<myLogic_64bits::MOp_to_string(mop)<<'\n';
        compare_preds<<myLogic_64bits::MOp_to_string(mop)<<'\n';

        vector<ui> preproc_mop = preprocess_to_MOp(mop);
        auto res1 = p1.isSave(mop, preproc_mop);
        auto res2 = p2.isSave(mop, preproc_mop);
        cout<<res1.first<<" "<<res2.first<<'\n';
        compare_preds<<res1.first<<" "<<res2.first<<'\n';

        if (res1.first) {
            comp_saves_1<<myLogic_64bits::MOp_to_string(mop)<<'\n';
        }
        if (res2.first) {
            comp_saves_2<<myLogic_64bits::MOp_to_string(mop)<<'\n';
        }

        if ( (res1.first ^ res2.first) == 1 ) {
            if (res1.first) {
                cout<<"1 Save\n";
                compare_preds<<"1 Save\n";
            }
            else {
                cout<<"1 Not Save on:\n"<<linesToString(res1.second);
                compare_preds<<"1 Not Save on:\n"<<linesToString(res1.second);
            }
            if (res2.first) {
                cout<<"2 Save\n";
                compare_preds<<"2 Save\n";
            }
            else {
                cout<<"2 Not Save on:\n"<<linesToString(res2.second);
                compare_preds<<"2 Not Save on:\n"<<linesToString(res2.second);
            }
        }

        mop.m_op++;

        if (counter % 10 == 0) {
            cout<<"Iteration: "<<counter<<'\n';
        }
    } while (mop.m_op <= endMOp.m_op);
}

void comp_preds(MOp begMOp, int pred1_nargs, string pred1_filename, int pred2_nargs, string pred2_filename) {
    ofstream compare_preds("compare_preds.txt");

    ofstream comp_saves_1("compare_preds_saves_1.txt");
    ofstream comp_saves_2("compare_preds_saves_2.txt");

    Predicate_saving p1 = Predicate_saving::get_Predicate_from_file(pred1_nargs, pred1_filename);
    Predicate_saving p2 = Predicate_saving::get_Predicate_from_file(pred2_nargs, pred2_filename);

    MOp mop = begMOp;
    MOp max_mop;
    max_mop.n_vars = begMOp.n_vars;
    max_mop.m_op = (1<<((1<<begMOp.n_vars) * 2));

    int counter = 0;
    do {
        counter++;
        compare_preds<<myLogic_64bits::MOp_to_string(mop)<<'\n';

        vector<ui> preproc_mop = preprocess_to_MOp(mop);
        auto res1 = p1.isSave(mop, preproc_mop);
        auto res2 = p2.isSave(mop, preproc_mop);
        compare_preds<<res1.first<<" "<<res2.first<<'\n';

        if (res1.first) {
            comp_saves_1<<myLogic_64bits::MOp_to_string(mop)<<'\n';
        }
        if (res2.first) {
            comp_saves_2<<myLogic_64bits::MOp_to_string(mop)<<'\n';
        }

        if ( (res1.first ^ res2.first) == 1 ) {
            if (res1.first) {
                compare_preds<<"1 Save\n";
            }
            else {
                compare_preds<<"1 Not Save on:\n"<<linesToString(res1.second);
            }
            if (res2.first) {
                compare_preds<<"2 Save\n";
            }
            else {
                compare_preds<<"2 Not Save on:\n"<<linesToString(res2.second);
            }
        }

        mop.m_op++;

        if (counter % 10 == 0) {
        }
    } while (mop.m_op < max_mop.m_op);
}

void pred_save_withcout(MOp begMOp, MOp endMOp, int pred_nargs, string pred_filename) {
    ofstream saves("pred_saves.txt");

    Predicate_saving p = Predicate_saving::get_Predicate_from_file(pred_nargs, pred_filename);

    MOp mop = begMOp;

    int counter = 0;
    do {
        counter++;
        cout<<myLogic_64bits::MOp_to_string(mop)<<'\n';
        vector<ui> preproc_mop = preprocess_to_MOp(mop);
        auto res = p.isSave(mop, preproc_mop);
        cout<<res.first<<'\n';
        if (res.first) {
            saves<<myLogic_64bits::MOp_to_string(mop)<<endl;
        }
        mop.m_op++;
        if (counter % 10 == 0) {
            cout<<"Iteration: "<<counter<<'\n';
        }
    } while (mop.m_op <= endMOp.m_op);
}

// 0 - 0, 1 - 1, 2 - {0,1}, 3 - *.
int main()
{
    pred_save_withcout(myLogic_64bits::create_multioperation(2, "0000"), myLogic_64bits::create_multioperation(2, "****"), 2, "input_pred.txt");
    return 0;
}
