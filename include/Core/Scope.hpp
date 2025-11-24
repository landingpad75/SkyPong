#pragma once

#define Scope(x) std::make_unique<x>(&app.mgr)
#define ScopeArgs(x, ...) std::make_unique<x>(&app.mgr, __VA_ARGS__)