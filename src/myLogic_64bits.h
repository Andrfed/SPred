#pragma once

#include <vector>
#include <string>

#define ull unsigned long long
#define ui unsigned int

struct MOp{
    ui n_vars;
    ull m_op;
};

struct ULLString{
    ui n;
    ull str;
};

int get_pow2(int k);

class myLogic_64bits{
    // only for multioperations with < 32 symbols (<= 16 symbols)
    private:
        static ui to_MOp(MOp mop, ui tomop, ui ind);
    public:
        static ull get_Pow(ui k, ui n);
        static ui fromSymbolToNumber(char symb);
        static ui fromNumberToSymbol(ui num);
        static MOp create_multioperation(ui nvars, std::vector<ui> v);
        static MOp create_multioperation(ui nvars, std::string s);
        static ui to_MOp(MOp mop, ui tomop);
        static std::string MOp_to_string(MOp mop);
        static std::string ULLString_to_string(ULLString s);
};

std::vector<ui> preprocess_to_MOp(MOp mop);

std::string linesToString(std::vector<ULLString> lines);
