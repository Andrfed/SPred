#include "Sup_pos.h"

using std::vector;
using std::set;
using std::ifstream;
using std::ofstream;
using std::string;
using std::reverse;
using std::endl;
using std::cout;

vector<MOp> Sup_pos::get_MOps_from_file(string filename) {
        ifstream fin(filename);
        string line;
        vector<MOp> out;
        while (fin>>line) {
            out.push_back(myLogic_64bits::create_multioperation(get_pow2(line.size()), line));
        }
        return out;
    }
bool Sup_pos::inMOps(MOp mop) {
        return distincts.find(mop.m_op) != distincts.end();
    }
void Sup_pos::combinations(int index, MOp &mop, vector<int> &cmb, vector<ui> &preproc_mop, vector<MOp> &new_mops, set<ull> &new_distincts, ofstream &new_mop_details) {
            if (index == mop.n_vars) {
                vector<ULLString> lines;
                ui div = 1;
                for (int i=0;i<(1<<n_vars);i++) {
                    ULLString line;
                    line.n = mop.n_vars;
                    line.str = 0;
                    ui divj = 1;
                    for (int j=cmb.size()-1;j>-1;j--) {
                        line.str += ((mops[cmb[j]].m_op / div) % 4) * divj;
                        divj *= 4;
                    }
                    div *= 4;
                    lines.push_back(line);
                }
                MOp col;
                col.n_vars = n_vars;
                col.m_op = 0;
                div = 1;
                for (int i=0;i<(1<<n_vars);i++) {
                    col.m_op += preproc_mop[lines[i].str] * div;
                    div *= 4;
                }
                if (!inMOps(col)) {
                    if (new_distincts.find(col.m_op) == new_distincts.end()) {
                        new_distincts.insert(col.m_op);
                        new_mops.push_back(col);
                        new_mop_details<<myLogic_64bits::MOp_to_string(mop)<<"(\n";
                        reverse(lines.begin(), lines.end());
                        new_mop_details<<linesToString(lines);
                        new_mop_details<<")= "<<myLogic_64bits::MOp_to_string(col)<<endl;
                    }
                }
            }
            else {
                for (int i=0;i<mops.size();i++) {
                    cmb[index] = i;
                    combinations(index+1, mop, cmb, preproc_mop, new_mops, new_distincts, new_mop_details);
                }
            }
        }
void Sup_pos::combinations(int index, MOp &mop, vector<int> &cmb, vector<ui> &preproc_mop, vector<MOp> &new_mops, set<ull> &new_distincts) {
            if (index == mop.n_vars) {
                vector<ULLString> lines;
                ui div = 1;
                for (int i=0;i<(1<<n_vars);i++) {
                    ULLString line;
                    line.n = mop.n_vars;
                    line.str = 0;
                    ui divj = 1;
                    for (int j=cmb.size()-1;j>-1;j--) {
                        line.str += ((mops[cmb[j]].m_op / div) % 4) * divj;
                        divj *= 4;
                    }
                    div *= 4;
                    lines.push_back(line);
                }
                MOp col;
                col.n_vars = n_vars;
                col.m_op = 0;
                div = 1;
                for (int i=0;i<(1<<n_vars);i++) {
                    col.m_op += preproc_mop[lines[i].str] * div;
                    div *= 4;
                }
                if (!inMOps(col)) {
                    if (new_distincts.find(col.m_op) == new_distincts.end()) {
                        new_distincts.insert(col.m_op);
                        new_mops.push_back(col);
                    }
                }
            }
            else {
                for (int i=0;i<mops.size();i++) {
                    cmb[index] = i;
                    combinations(index+1, mop, cmb, preproc_mop, new_mops, new_distincts);
                }
            }
        }
Sup_pos::Sup_pos(int nvars, vector<MOp> m_ops) {
        n_vars = nvars;
        for (int i=0;i<m_ops.size();i++) {
            if (distincts.find(m_ops[i].m_op) == distincts.end()) {
                distincts.insert(m_ops[i].m_op);
                mops.push_back(m_ops[i]);
            }
        }
        preproc_mops.resize(mops.size());
        for (int i=0;i<mops.size();i++) {
            preproc_mops[i] = preprocess_to_MOp(mops[i]);
        }
    }
Sup_pos::Sup_pos(int nvars, string filename) {
        n_vars = nvars;
        vector<MOp> m_ops = get_MOps_from_file(filename);
        for (int i=0;i<m_ops.size();i++) {
            if (distincts.find(m_ops[i].m_op) == distincts.end()) {
                distincts.insert(m_ops[i].m_op);
                mops.push_back(m_ops[i]);
            }
        }
        preproc_mops.resize(mops.size());
        for (int i=0;i<mops.size();i++) {
            preproc_mops[i] = preprocess_to_MOp(mops[i]);
        }
    }
vector<MOp> Sup_pos::get_new_MOps(MOp mop, vector<ui> &preproc_mop) {
        vector<MOp> new_mops;
        set<ull> new_distincts;
        vector<int> cmb;
        cmb.resize(mop.n_vars);
        combinations(0, mop, cmb, preproc_mop, new_mops, new_distincts);
        return new_mops;
    }
vector<MOp> Sup_pos::get_new_MOps(MOp mop, vector<ui> &preproc_mop, ofstream &new_mop_details, ofstream &new_mop) {
        vector<MOp> new_mops;
        set<ull> new_distincts;
        vector<int> cmb;
        cmb.resize(mop.n_vars);
        combinations(0, mop, cmb, preproc_mop, new_mops, new_distincts, new_mop_details);
        for (int i=0;i<new_mops.size();i++) {
            new_mop<<myLogic_64bits::MOp_to_string(new_mops[i])<<'\n';
        }
        return new_mops;
    }
bool Sup_pos::closure_step(ofstream &new_mop_details, ofstream &new_mop) {
        vector<MOp> new_mops;
        set<ull> t_distincts;
        vector<MOp> p_mops;
        for (int i=0;i<mops.size();i++) {
            p_mops = get_new_MOps(mops[i], preproc_mops[i], new_mop_details, new_mop);
            for (int j=0;j<p_mops.size();j++) {
                if (t_distincts.find(p_mops[j].m_op) == t_distincts.end()) {
                    t_distincts.insert(p_mops[j].m_op);
                    new_mops.push_back(p_mops[j]);
                }
            }
        }
        for (int i=0;i<new_mops.size();i++) {
            if (distincts.find(new_mops[i].m_op) == distincts.end()) {
                distincts.insert(new_mops[i].m_op);
                mops.push_back(new_mops[i]);
                preproc_mops.push_back(preprocess_to_MOp(new_mops[i]));
            }
        }
        return new_mops.size() > 0;
    }
bool Sup_pos::closure_step() {
        vector<MOp> new_mops;
        set<ull> t_distincts;
        vector<MOp> p_mops;
        for (int i=0;i<mops.size();i++) {
            p_mops = get_new_MOps(mops[i], preproc_mops[i]);
            for (int j=0;j<p_mops.size();j++) {
                if (t_distincts.find(p_mops[j].m_op) == t_distincts.end()) {
                    t_distincts.insert(p_mops[j].m_op);
                    new_mops.push_back(p_mops[j]);
                }
            }
        }
        for (int i=0;i<new_mops.size();i++) {
            if (distincts.find(new_mops[i].m_op) == distincts.end()) {
                distincts.insert(new_mops[i].m_op);
                mops.push_back(new_mops[i]);
                preproc_mops.push_back(preprocess_to_MOp(new_mops[i]));
            }
        }
        return new_mops.size() > 0;
    }
void Sup_pos::closure_full(ofstream &new_mop_details, ofstream &new_mop, bool step_counter_on) {
        int counter = 1;
        while (closure_step(new_mop_details, new_mop)) {
            if (step_counter_on) {
                cout<<"step "<<counter<<endl;
                counter++;
            }
        }
    }
void Sup_pos::closure_full() {
        while (closure_step()) {
            continue;
        }
    }
vector<MOp> Sup_pos::get_MOps() {
        return mops;
    }
void Sup_pos::args_up_from2to3() {
        vector<MOp> old_mops = mops;
        distincts.clear();
        mops.clear();
        n_vars = 3;
        MOp x1 = myLogic_64bits::create_multioperation(3, "00001111");
        MOp x2 = myLogic_64bits::create_multioperation(3, "00110011");
        MOp x3 = myLogic_64bits::create_multioperation(3, "01010101");
        mops.push_back(x1);
        distincts.insert(x1.m_op);
        mops.push_back(x2);
        distincts.insert(x2.m_op);
        mops.push_back(x3);
        distincts.insert(x3.m_op);
        vector<MOp> new_mops;
        ofstream new_mop_details("new_mops_details.txt");
        ofstream new_mop("new_mops.txt");
        for (int i=0;i<old_mops.size();i++) {
            vector<MOp> new_mops_tmp = get_new_MOps(old_mops[i], preproc_mops[i], new_mop_details, new_mop);
            for (int j=0;j<new_mops_tmp.size();j++)
                new_mops.push_back(new_mops_tmp[j]);
        }
        preproc_mops.clear();
        preproc_mops.push_back(preprocess_to_MOp(x1));
        preproc_mops.push_back(preprocess_to_MOp(x2));
        preproc_mops.push_back(preprocess_to_MOp(x3));
        for (int i=0;i<new_mops.size();i++) {
            if (distincts.find(new_mops[i].m_op) == distincts.end()) {
                distincts.insert(new_mops[i].m_op);
                mops.push_back(new_mops[i]);
                preproc_mops.push_back(preprocess_to_MOp(new_mops[i]));
            }
        }
    }
bool Sup_pos::inSup_pos(MOp mop) {
        return distincts.find(mop.m_op) != distincts.end();
    }
bool Sup_pos::inSup_pos(Sup_pos sp) {
        for (auto mop : sp.get_MOps()) {
            if (distincts.find(mop.m_op) == distincts.end())
                return false;
        }
        return true;
    }
