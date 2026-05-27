#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <set>

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
            return -1;
        }
        static char fromNumberToSymbol(ui num) {
            if (num == 0)
                return '0';
            if (num == 1)
                return '1';
            if (num == 2)
                return '-';
            if (num == 3)
                return '*';
            return '#';
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

// 0 - 0, 1 - 1, 2 - -, 3 - *.
int main()
{

    check_funcs(3, "input_pred_R_tmp.txt", "input_funcs.txt");
/*
    auto start = std::chrono::high_resolution_clock::now();
    comp_preds(myLogic_64bits::create_multioperation(2, "0000"), 4, "input_pred.txt", 3, "input_pred_R_tmp.txt");
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "elapsed time: " << elapsed.count() << " ms, " << elapsed.count()/1000.0 << " s" << std::endl;
*/
/*
    auto start = std::chrono::high_resolution_clock::now();
    comp_preds_withcout(myLogic_64bits::create_multioperation(3, "00000000"), myLogic_64bits::create_multioperation(3, "0000*000"), 4, "input_pred.txt", 3, "input_pred_R_tmp.txt");
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "elapsed time: " << elapsed.count() << " ms, " << elapsed.count()/1000.0 << " s" << std::endl;
*/
/*
    auto start = std::chrono::high_resolution_clock::now();
    pred_save_withcout(myLogic_64bits::create_multioperation(2, "0000"), myLogic_64bits::create_multioperation(2, "****"), 3, "input_pred_R_tmp.txt");
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "elapsed time: " << elapsed.count() << " ms, " << elapsed.count()/1000.0 << " s" << std::endl;
*/
    return 0;
}
// after first rework: elapsed time: 3531.9 ms, 3.5319 s (on m.op. 2, k=4n=172, k=3n=51)
// after second rework(preproc m.op.): elapsed time: 3282.71 ms, 3.28271 s (on m.op. 2, k=4n=172, k=3n=51)
// elapsed time: 205056 ms, 205.056 s
// (preproc m.op.) elapsed time: 181535 ms, 181.535 s
