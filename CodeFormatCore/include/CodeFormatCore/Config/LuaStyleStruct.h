#pragma once

struct ContinuationIndent {
    std::size_t in_expr = 4;
    std::size_t before_block = 4;
    std::size_t in_table = 4;

    void SetAll(std::size_t value) {
        in_expr = value;
        before_block = value;
        in_table = value;
    }
};

struct SpaceBeforeSingleArg {
    FunctionSingleArgSpace table = FunctionSingleArgSpace::Always;
    FunctionSingleArgSpace string = FunctionSingleArgSpace::Always;

    void SetAll(FunctionSingleArgSpace value) {
        table = value;
        string = value;
    }
};

struct SpaceAroundMath {
    bool exponent = true;
    bool other = true;

    void SetAll(bool value) {
        exponent = value;
        other = value;
    }
};