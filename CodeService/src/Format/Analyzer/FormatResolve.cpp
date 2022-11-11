#include "CodeService/Format/Analyzer/FormatResolve.h"

SpaceStrategy FormatResolve::GetSpaceStrategy() const {
    return _spaceStrategy;
}

TokenStrategy FormatResolve::GetTokenStrategy() const {
    return _tokenStrategy;
}
