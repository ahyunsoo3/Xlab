## Side-by-side summary

| Code        | New memory? | Can rebind? | Can be null? |
|-------------|-------------|-------------|--------------|
| `int a = b` | ✅ Yes      | N/A         | ❌ No        |
| `int& r = b`| ❌ No       | ❌ No       | ❌ No        |
| `int* p = &b` | ✅ Yes    | ✅ Yes      | ✅ Yes       |
