#pragma once

#include "DBBase.h"
#include "Lib/LineIndex/LineIndex.h"

class LineIndexDB : public SharedDBBase<std::size_t, LineIndex> {

};
