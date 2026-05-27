#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <set>
#include <random>

#define ull unsigned long long
#define ui unsigned int


using namespace std;

struct MOp{
    ui n_vars;
    ull m_op;
};

struct ULLString{
    ui n;
    ull str;
};

int get_pow2(int k) {
    int n = 0;
    int kk = k;
    while (kk > 1) {
        kk = kk>>1;
        n++;
    }
    return n;
}

class myLogic_64bits{
    // only for multioperations with < 32 symbols (<= 16 symbols)
    private:
        static ui to_MOp(MOp mop, ui tomop, ui ind) {
            if (ind == mop.n_vars) {
                ui n2 = 0;
                ui p_tomop = tomop;
                ui div = 1;
                while (p_tomop > 0) {
                    n2 += (p_tomop % 4) * div;
                    p_tomop /= 4;
                    div *= 2;
                }
                return (mop.m_op / (1<<((((1<<mop.n_vars)-1) - n2)*2))) % 4;
            }
            ui proc = (tomop / (1<<(ind*2))) % 4;
            if (proc == 2) {
                ui res1 = to_MOp(mop, tomop - 2*(1<<(ind*2)), ind + 1);
                if (res1 == 3)
                    return 3;
                ui res2 = to_MOp(mop, tomop - 2*(1<<(ind*2)) + (1<<(ind*2)), ind + 1);
                if (res2 == 3)
                    return 3;
                if (res1 == 2 || res2 == 2)
                    return 2;
                if (res1 == 0 && res2 == 0)
                    return 0;
                if (res1 == 1 && res2 == 1)
                    return 1;
                return 2;
            }
            if (proc == 3) {
                return 3;
            }
            return to_MOp(mop, tomop, ind + 1);
        }
    public:
        // !!!
        static ull get_Pow(ui k, ui n) {
            ull t = 1;
            for (int i=0;i<n;i++) {
                t *= k;
            }
            return t;
        }
        // 0 - 0, 1 - 1, 2 - -, 3 - *.
        static ui fromSymbolToNumber(char symb) {
            if (symb == '0')
                return 0;
            if (symb == '1')
                return 1;
            if (symb == '-')
                return 2;
            if (symb == '*')
                return 3;
        }
        static ui fromNumberToSymbol(ui num) {
            if (num == 0)
                return '0';
            if (num == 1)
                return '1';
            if (num == 2)
                return '-';
            if (num == 3)
                return '*';
        }
        // !!!
        static MOp create_multioperation(ui nvars, vector<ui> v) {
            MOp mop;
            mop.n_vars = nvars;
            mop.m_op = 0;
            ull d = 1;
            for (int i=v.size()-1;i>-1;i--) {
                mop.m_op += d * v[i];
                d *= 4;
            }
            return mop;
        }
        // !!!
        static MOp create_multioperation(ui nvars, string s) {
            MOp mop;
            mop.n_vars = nvars;
            mop.m_op = 0;
            ull d = 1;
            for (int i=s.size()-1;i>-1;i--) {
                mop.m_op += d * fromSymbolToNumber(s[i]);
                d *= 4;
            }
            return mop;
        }
        // !!!
        static ui to_MOp(MOp mop, ui tomop) {
            return to_MOp(mop, tomop, 0);
        }
        static string MOp_to_string(MOp mop) {
            string out = "";
            int pw = get_Pow(2, mop.n_vars);
            for (int i=0;i<pw;i++) {
                out += '0';
            }
            MOp p_mop = mop;
            int ind = pw - 1;
            while (ind > -1) {
                out[ind] = fromNumberToSymbol(p_mop.m_op % 4);
                ind--;
                p_mop.m_op /= 4;
            }
            return out;
        }
        static string ULLString_to_string(ULLString s) {
            string out = "";
            for (int i=0;i<s.n;i++) {
                out += '0';
            }
            ULLString p_s = s;
            int ind = s.n - 1;
            while (ind > -1) {
                out[ind] = fromNumberToSymbol(p_s.str % 4);
                ind--;
                p_s.str /= 4;
            }
            return out;
        }
};

vector<ui> preprocess_to_MOp(MOp mop) {
    vector<ui> out(1<<(mop.n_vars*2));
    for (ui i = 0; i < out.size(); i++) {
        out[i] = myLogic_64bits::to_MOp(mop, i);
    }
    return out;
}

string linesToString(vector<ULLString> lines) {
    string s = "";
    for (int i=0;i<lines.size();i++) {
        s += myLogic_64bits::ULLString_to_string(lines[i]);
        s += '\n';
    }
    return s;
}

class Predicate_saving{
    private:
        vector<ui> p;
        set<ui> distincts;
        ui n;
        ui k;

        bool inPred(ui col) {
            return distincts.find(col) != distincts.end();
        }

        pair<bool, vector<ULLString> > combinations(int index, MOp &mop, vector<int> &cmb) {
            if (index == mop.n_vars) {
                vector<ULLString> lines;
                ui div = 1;
                for (int i=0;i<k;i++) {
                    ULLString line;
                    line.n = mop.n_vars;
                    line.str = 0;
                    ui divj = 1;
                    for (int j=cmb.size()-1;j>-1;j--) {
                        line.str += ((p[cmb[j]] / div) % 4) * divj;
                        divj *= 4;
                    }
                    div *= 4;
                    lines.push_back(line);
                }
                //reverse(lines.begin(), lines.end());
                ui col = 0;
                div = 1;
                for (int i=0;i<k;i++) {
                    col += myLogic_64bits::to_MOp(mop, lines[i].str) * div;
                    div *= 4;
                }
                if (!inPred(col)) {
                    ULLString ulls;
                    ulls.n = k;
                    ulls.str = col;
                    //cout<<myLogic_64bits::ULLString_to_string(ulls)<<'\n';
                    return {0, lines};
                }
                else {
                    return {1, lines};
                }
            }
            else {
                for (int i=0;i<n;i++) {
                    cmb[index] = i;
                    auto res = combinations(index+1, mop, cmb);
                    if (!res.first) {
                        return res;
                    }
                }
            }
            return {1, vector<ULLString>(1)};
        }
        pair<bool, vector<ULLString> > combinations(int index, MOp &mop, vector<int> &cmb, vector<ui> &preproc_mop) {
            if (index == mop.n_vars) {
                vector<ULLString> lines;
                ui div = 1;
                for (int i=0;i<k;i++) {
                    ULLString line;
                    line.n = mop.n_vars;
                    line.str = 0;
                    ui divj = 1;
                    for (int j=cmb.size()-1;j>-1;j--) {
                        line.str += ((p[cmb[j]] / div) % 4) * divj;
                        divj *= 4;
                    }
                    div *= 4;
                    lines.push_back(line);
                }
                //reverse(lines.begin(), lines.end());
                ui col = 0;
                div = 1;
                for (int i=0;i<k;i++) {
                    col += preproc_mop[lines[i].str] * div;
                    div *= 4;
                }
                if (!inPred(col)) {
                    ULLString ulls;
                    ulls.n = k;
                    ulls.str = col;
                    //cout<<myLogic_64bits::ULLString_to_string(ulls)<<'\n';
                    reverse(lines.begin(), lines.end());
                    return {0, lines};
                }
                else {
                    return {1, lines};
                }
            }
            else {
                for (int i=0;i<n;i++) {
                    cmb[index] = i;
                    auto res = combinations(index+1, mop, cmb, preproc_mop);
                    if (!res.first) {
                        return res;
                    }
                }
            }
            return {1, vector<ULLString>(1)};
        }

    public:
        Predicate_saving(vector<ui> pred, ui pred_k) {
            p = pred;
            n = p.size();
            k = pred_k;
            for (int i=0;i<n;i++) {
                distincts.insert(p[i]);
            }
        }
        pair<bool, vector<ULLString> > isSave(MOp mop) {
            vector<int> cmb;
            cmb.resize(mop.n_vars);
            return combinations(0, mop, cmb);
        }
        pair<bool, vector<ULLString> > isSave(MOp mop, vector<ui> &preproc_mop) {
            vector<int> cmb;
            cmb.resize(mop.n_vars);
            return combinations(0, mop, cmb, preproc_mop);
        }
        string to_string() {
            string out = "";
            ui div = myLogic_64bits::get_Pow(4, k-1);
            for (int i=0;i<k;i++) {
                string line;
                for (int j=0;j<n;j++) {
                    line += myLogic_64bits::fromNumberToSymbol((p[j] / div) % 4);
                }
                div /= 4;
                out += line;
                out += '\n';
            }
            return out;
        }
        static Predicate_saving get_Predicate_from_file(int n_vars, string filename) {
            ifstream fin(filename);
            vector<ui> pred;
            string s;
            ui div = myLogic_64bits::get_Pow(4, n_vars-1);
            while (fin>>s) {
                if (pred.size() == 0) {
                    pred.resize(s.size(), 0);
                }
                for (int i=0;i<s.size();i++) {
                    pred[i] += myLogic_64bits::fromSymbolToNumber(s[i]) * div;
                }
                div /= 4;
            }
            return Predicate_saving(pred, n_vars);
        }
};

class Sup_pos{
    // MOps must be the same length
private:
    int n_vars;
    vector<MOp> mops;
    vector<vector<ui> > preproc_mops;
    set<ull> distincts;
    vector<MOp> get_MOps_from_file(string filename) {
        ifstream fin(filename);
        string line;
        vector<MOp> out;
        while (fin>>line) {
            out.push_back(myLogic_64bits::create_multioperation(get_pow2(line.size()), line));
        }
        return out;
    }
    bool inMOps(MOp mop) {
        return distincts.find(mop.m_op) != distincts.end();
    }
    void combinations(int index, MOp &mop, vector<int> &cmb, vector<ui> &preproc_mop, vector<MOp> &new_mops, set<ull> &new_distincts, ofstream &new_mop_details) {
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
        void combinations(int index, MOp &mop, vector<int> &cmb, vector<ui> &preproc_mop, vector<MOp> &new_mops, set<ull> &new_distincts) {
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
public:
    Sup_pos(int nvars, vector<MOp> m_ops) {
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
    Sup_pos(int nvars, string filename) {
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
    vector<MOp> get_new_MOps(MOp mop, vector<ui> &preproc_mop) {
        vector<MOp> new_mops;
        set<ull> new_distincts;
        vector<int> cmb;
        cmb.resize(mop.n_vars);
        combinations(0, mop, cmb, preproc_mop, new_mops, new_distincts);
        return new_mops;
    }
    vector<MOp> get_new_MOps(MOp mop, vector<ui> &preproc_mop, ofstream &new_mop_details, ofstream &new_mop) {
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
    bool closure_step(ofstream &new_mop_details, ofstream &new_mop) {
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
    bool closure_step() {
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
    void closure_full(ofstream &new_mop_details, ofstream &new_mop, bool step_counter_on) {
        int counter = 1;
        while (closure_step(new_mop_details, new_mop)) {
            if (step_counter_on) {
                cout<<"step "<<counter<<endl;
                counter++;
            }
        }
    }
    void closure_full() {
        while (closure_step()) {
            continue;
        }
    }
    vector<MOp> get_MOps() {
        return mops;
    }
    void args_up_from2to3() {
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
    bool inSup_pos(MOp mop) {
        return distincts.find(mop.m_op) != distincts.end();
    }
    bool inSup_pos(Sup_pos sp) {
        for (auto mop : sp.get_MOps()) {
            if (distincts.find(mop.m_op) == distincts.end())
                return false;
        }
        return true;
    }
};

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


// Внутреннее представление
// 0 - 0, 1 - 1, 2 - {0,1}, 3 - *.
int main()
{
    /*
    ofstream new_mop_details("new_mops_details.txt");
    ofstream new_mop("new_mops.txt");
    Sup_pos sp(2, "input_got_funcs.txt");
    sp.closure_step(new_mop_details, new_mop);
    */

    get_closure(2, "input_got_funcs.txt");

    //partial_pred_check(4, "input_pred.txt", 3, "input_partial_1.txt");
    //partial_new_check(3, "input_partial.txt"); // all 3 m.op.s that save tmp_R_dot = 32656

    //get_closures_and_intersection(2, "input_got_funcs.txt", "input_funcs_to_intersect.txt");
/*
    Sup_pos sp_C3_1(2, "intersection_C3_1_K_10.txt");
    Sup_pos sp_C4_1(2, "intersection_C4_1_K_10.txt");
    Sup_pos sp_C4_2(2, "intersection_C4_2_K_10.txt");

    cout<<sp_C3_1.inSup_pos(sp_C4_1)<<" "<<sp_C3_1.inSup_pos(sp_C4_2);
*/
/*
    Sup_pos sp(2, "input_got_funcs.txt");
    sp.args_up_from2to3();
    ofstream closure_out("closure_out.txt");
    auto mops = sp.get_MOps();
    for (int i=0;i<mops.size();i++) {
        closure_out<<myLogic_64bits::MOp_to_string(mops[i])<<'\n';
    }
*/
    return 0;
}
