#include "myLogic_64bits.h"

using std::string;
using std::vector;

int get_pow2(int k) {
    int n = 0;
    int kk = k;
    while (kk > 1) {
        kk = kk>>1;
        n++;
    }
    return n;
}

ui myLogic_64bits::to_MOp(MOp mop, ui tomop, ui ind) {
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
        ull myLogic_64bits::get_Pow(ui k, ui n) {
            ull t = 1;
            for (int i=0;i<n;i++) {
                t *= k;
            }
            return t;
        }
        // 0 - 0, 1 - 1, 2 - -, 3 - *.
        ui myLogic_64bits::fromSymbolToNumber(char symb) {
            if (symb == '0')
                return 0;
            if (symb == '1')
                return 1;
            if (symb == '-')
                return 2;
            if (symb == '*')
                return 3;
        }
        ui myLogic_64bits::fromNumberToSymbol(ui num) {
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
        MOp myLogic_64bits::create_multioperation(ui nvars, vector<ui> v) {
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
        MOp myLogic_64bits::create_multioperation(ui nvars, string s) {
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
        ui myLogic_64bits::to_MOp(MOp mop, ui tomop) {
            return to_MOp(mop, tomop, 0);
        }
        string myLogic_64bits::MOp_to_string(MOp mop) {
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
        string myLogic_64bits::ULLString_to_string(ULLString s) {
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
