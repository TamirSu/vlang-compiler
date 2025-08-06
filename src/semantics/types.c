#include "types.h"

// utility constructors
Type type_scl(void){ Type t={TYPE_SCL,0}; return t; }
Type type_vec(int n){ Type t={TYPE_VEC,n}; return t; }
