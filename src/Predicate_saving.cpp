#include "Predicate_saving.h"

using std::vector;
using std::set;
using std::string;
using std::pair;
using std::reverse;
using std::ifstream;

bool Predicate_saving::inPred(ui col) {
            return distincts.find(col) != distincts.end();
        }
pair<bool, vector<ULLString> > Predicate_saving::combinations(int index, MOp &mop, vector<int> &cmb) {
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
pair<bool, vector<ULLString> > Predicate_saving::combinations(int index, MOp &mop, vector<int> &cmb, vector<ui> &preproc_mop) {
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
Predicate_saving::Predicate_saving(vector<ui> pred, ui pred_k) {
            p = pred;
            n = p.size();
            k = pred_k;
            for (int i=0;i<n;i++) {
                distincts.insert(p[i]);
            }
        }
pair<bool, vector<ULLString> > Predicate_saving::isSave(MOp mop) {
            vector<int> cmb;
            cmb.resize(mop.n_vars);
            return combinations(0, mop, cmb);
        }
pair<bool, vector<ULLString> > Predicate_saving::isSave(MOp mop, vector<ui> &preproc_mop) {
            vector<int> cmb;
            cmb.resize(mop.n_vars);
            return combinations(0, mop, cmb, preproc_mop);
        }
string Predicate_saving::to_string() {
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
Predicate_saving Predicate_saving::get_Predicate_from_file(int n_vars, string filename) {
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
