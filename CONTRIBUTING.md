# Rules and Guidelines
## Naming
- Source file names must use `snake_case`. Use `*.h` for headers, `*.cpp` for compilation units, `*.inl` for inline definitions and `*.{assembler}.inc` for assembler includes.

```
- source/
    - example.h
    - example.cpp
    - example.inl
    - example.masm.inc
```

- Platform abstractions must be put under `arch` and `os` subdirectories. These must be included in the main header file they branch from using preprocessor rules.

```
- source/
    - arch/
        - example_x86.h
        - example_arm.h
    - os/
        - example_win32.h
        - example_linux.h

    - example.h
```

- Identifiers must use `snake_case`. This includes class names, functions, lambdas, parameters and variables.

```cpp
class example_class;

int example_function(int in_example_param)
{
    int result{};

    const auto example_lambda = [](int in_example_lambda_param) -> int
    {
        return in_example_lambda_param + 123;
    };

    result = example_lambda(in_example_param);

    return result;
}
```

- Macros must use `UPPER_SNAKE_CASE`. This includes macro parameters.
- Macros with multiple lines must place the line continuation escape character on the same column.

```cpp
#define EXAMPLE_MACRO(EXAMPLE_PARAM)                             \
    int g_example_global = example_function(EXAMPLE_PARAM);      \
    int g_example_global_2 = example_function(g_example_global); \
```

### Scope Hints
These hints must not be chained, rather they must follow the order below for which prefix takes precedence:
- Internal macros must use the `__CMNLIB_INTERNAL_` prefix.
- Global variables must use the `g_` prefix.
- Private variables must use the `m_` prefix.
- Private functions must use the `_` prefix.
- Constant variables (`const` and `constexpr`) must use the `k_` prefix. This does not include expressions.
- Function parameters must use the `in_` prefix for input parameters, and `out_` for output parameters. If a parameter may be treated as both input and output, use the `io_` prefix.
- Template parameters must use the `T_` prefix for type parameters, and `K_` for variable parameters. If a type parameter corresponds to a specific function parameter, then it must use the same identifier with the `T_` prefix (e.g. `T_param_a` for `in_param_a`). Type parameters may be specified as `T`, if there is only one of them.

```cpp
int g_global = 123;

constexpr int k_constant = 456;

template <typename T>
constexpr bool is_boolean_v = std::is_same_v<T, bool>;

template <typename T>
void example_template_function(T in_param);

template <typename T_param_a, typename T_param_b>
void example_template_function(T_param_a in_param_a, T_param_b in_param_b);

template <typename T, size_t K_example>
void example_template_function(T in_param);

template <typename T_param_a, typename T_param_b, size_t K_example>
void example_template_function(T_param_a in_param_a, T_param_b in_param_b);

class example_class
{
private:
    int m_private = 789;

public:
    bool try_do_stuff(int in_param, int& out_result)
    {
        out_result = in_param;
        return true;
    }

    void mutate_param(int& io_mutate_me)
    {
        io_mutate_me = 123;
    }
};
```

### Type Suffixes
- Type definitions must use the `_t` suffix.
- Constant expressions that return a value must use the `_v` suffix.

```cpp
using char_string_t = std::basic_string<char>;

template <typename t>
constexpr bool is_boolean_v = std::is_same_v<t, bool>;
```

### Variable Suffixes
Variables may specify a suffix if a copy must be made that converts an existing variable to a specific type.

Type                                    |Suffix
----------------------------------------|-------
`char` / `std::string`                  |`_c`
`wchar_t` / `std::wstring`              |`_w`
`std::basic_string_view<T>`             |`_sv`
`uint8_t` / `char8_t` / `std::u8string` |`_u8`
`int8_t`                                |`_s8`
`uint16_t`                              |`_u16`
`int16_t`                               |`_s16`
`uint32_t`                              |`_u32`
`int32_t`                               |`_s32`
`uint64_t`                              |`_u64`
`int64_t`                               |`_s64`
`uintptr_t`                             |`_uptr`
`intptr_t`                              |`_ptr`
`size_t`                                |`_sz`

```cpp
void example_function_a(int* in_param);

void example_function_b(size_t in_param)
{
    auto param_s32 = int(in_param);

    example_function_a(&param_s32);
}
```

## Includes
- If a compilation unit (`*.cpp`) has a header (`*.h`), it must be the first include at the top above any other.
- If a header (`*.h`) contains inline declarations, it must include the inline definitions (`*.inl`) at the bottom.
- Includes must be in alphanumeric order from 0-9 and A-Z.
- Includes must use quotes and Unix directory separators (e.g. `#include "example/example_header.h"`).
- Third-party includes must be relative to the `include` directory (e.g. `#include "../thirdparty/example.h"`).
- Internal and third-party headers must be included in files that require them, even if they would be inherited by another header.

## Declarations
- Pointer and reference operators on declarations must be suffixed to the type, rather than prefixed to the identifier.

```cpp
// Good
void example_function(int* in_param_a, int& in_param_b);

// Bad
void example_function(int *in_param_a, int &in_param_b);
```

- Parameters and variables must always be declared as `const`, unless they explicitly need to be modified. This does not include return types.
- Variables inside function scopes must always be declared as `auto`, if the assignment is not ambiguous. If a specific data type is needed, explicitly specify the type instead.
- Fundamental types must always use function style casts, rather than C style casts (e.g. `int(value)` instead of `(int)value`). Otherwise, use C++ style casts (e.g. `reinterpret_cast`, `static_cast`, etc).

```cpp
void example_function_a(int in_param);

void example_function_b(std::vector<uint8_t>& io_param);

void example_function_c(const int in_param)
{
    // Bad: Remove const qualifier.
    in_param = 0;

    // Good: const qualifier specified.
    // Bad: Integral type is ambiguous.
    const auto k_example_a = 1;

    // Bad: Remove const qualifier and "k_" prefix.
    k_example_a = 2;

    // Good: const qualifier specified and integral type explicitly specified
    //       for clarity.
    const size_t k_example_b = 1;

    // Good: Explicit cast from size_t to int with a function style cast.
    example_function_a(int(k_example_b));

    // Bad: Implicit cast from size_t to int. Use a function style cast.
    example_function_a(k_example_b);

    // Good: const qualifier specified, type is not ambiguous.
    const auto k_example_c = std::vector<uint8_t>();

    // Bad: const passed into function that can mutate it by reference.
    example_function_b(k_example_c);
}
```

- Variables must be initialised if the declaration has no assignment.

```cpp
void example_function()
{
    // Bad: Variable is not initialised. This could be initialised later under some
    //      conditions, but if the code changes in a way that this does not get set,
    //      but is still used, it could end up with a garbage value.
    int result;

    // Good
    int result{};
}
```

- Classes must always start with an access modifier.

```cpp
// Good
class example_class
{
private:
    int m_example_field{};

    void _private_function_example();

public:
    int example_field{};

    void public_function_example();
};

// Bad
class example_class
{
    int m_example_field{};

    void _private_function_example();

public:
    int example_field{};

    void public_function_example();
};
```

- Lambdas must explicitly specify their return type when possible.

```cpp
const auto example_lambda = []() -> size_t
{
    return 123;
};
```

## Documentation
- Comments must have a space after the comment syntax.
- Comments must be split onto new lines after ~80 characters or less, with the next line also starting as a single line comment. If the next line would only be a single word, move the last word from the previous line onto the next for clarity, or split the lines down to a shorter width. For code blocks in Doxygen comments, use standard coding conventions.
- Comments must use sentence case. Start with a capital letter and end with a full stop.

```cpp
// Bad
//Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.

// Good
// Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor
// incididunt ut labore et dolore magna aliqua.

// Bad
// Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor
// incididunt.

// Good
// Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod
// tempor incididunt.
```

- Comments for reminders may be used with `TODO`, `FIXME`, `NOTE`, `WARNING` or `HACK` prefixes. These must be attributed to the author that wrote them.

```cpp
// FIXME (Hyper): This will crash, oh my god!!
*(int*)0 = 0;
```

- Doxygen comments must use `///` blocks, with blank ones above and below the documentation.
- Doxygen comments must use backslashes for keywords.

```cpp
///
/// A function that does things.
///
/// \tparam T         The type.
/// \tparam K_example The example.
///
/// \param in_foo The foo.
/// \param in_bar The bar.
///
/// \returns An integer of some kind.
///
template <typename T, size_t K_example>
int example_function(int in_foo, int in_bar);
```

- Doxygen comments that reference other identifiers must use `\ref` before the identifier, if possible. Otherwise, use single backticks either side of the identifier.

```cpp
void example_function_a();

///
/// A function that might call \ref example_function_a().
///
void example_function_b();

// -- OR -- (only if the identifier cannot be referenced)

///
/// A function that might call `example_function_a()`.
///
void example_function_b();
```

## Syntax Rules
- Allman style indentation is mandatory.

```cpp
// Good
if (condition)
{
    if (another_condition)
    {
        do_stuff();

        if (yet_another_condition)
        {
            do_stuff_again();
            do_stuff_differently();
        }
    }

    do_even_more_stuff();
}

// Bad
if (condition) {
    if (another_condition) {
        do_stuff();

        if (yet_another_condition) {
            do_stuff_again();
            do_stuff_differently();
        }
    }

    do_even_more_stuff();
}
```

- If statements with a single line must not have braces. If an if statement has an else case, or the condition extends onto multiple lines, put it in a scope.

```cpp
// Good
if (condition)
    do_stuff();

// Bad
if (condition)
{
    do_stuff();
}

// Good
if (long_condition_that_is_on_the_left_side &&
    long_condition_that_is_on_the_right_side)
{
    do_stuff();
}

// Bad
if (long_condition_that_is_on_the_left_side &&
    long_condition_that_is_on_the_right_side)
    do_stuff();

// Good
if (condition)
{
    do_stuff();
}
else
{
    do_other_stuff();
}

// Bad
if (condition)
    do_stuff();
else
    do_other_stuff();
```

- Switch cases with a single line must not have braces. If a switch case declares a variable or needs spacing apart, put it in a scope.

```cpp
switch (value)
{
    // Good
    case 0:
        do_something();
        break;

    // Bad
    case 1:
        int a = 123;
        do_something(a);
        break;

    // Good
    case 2:
    {
        // Good: This allows later cases to create a variable of the same name.
        int a = 123;

        do_something(a);

        break;
    }

    // Bad
    case 3:
        do_something();

        if (condition)
            do_something_else();

        break;

    // Good
    case 4:
    {
        do_something();

        if (condition)
            do_something_else();

        break;
    }
}
```

- Ternary conditions must be split onto new lines.

```cpp
int a = 1;
int b = 2;
int c = 3;
int d = 4;

// Good
auto selection = condition
    ? a
    : b;

// Acceptable: Only if the results of the condition are short enough.
auto selection = condition ? a : b;

// Acceptable: An if statement would be preferable instead.
auto selection = condition
    ? another_condition
        ? a
        : yet_another_condition
            ? b
            : c
    : d;

// Bad
auto selection = condition ? another_condition ? a : yet_another_condition ? b : c : d;

// Bad
auto selection = (condition ? (another_condition ? a : (yet_another_condition ? b : c)) : d);
```

- Function declarations that have parameters exceeding ~128 characters must be split onto new lines.

```cpp
// Good
void example_function
(
    int in_param_a,
    int in_param_b,
    int in_param_c,
    int in_param_d,
    int in_param_e,
    int in_param_f,
    int in_param_g,
    int in_param_h
)
{
    do_stuff(in_param_a, in_param_b, in_param_c, in_param_d,
        in_param_e, in_param_f, in_param_g, in_param_h);
}

// Bad
void example_function(int in_param_a, int in_param_b, int in_param_c, int in_param_d, int in_param_e, int in_param_f, int in_param_g, int in_param_h)
{
    do_stuff(in_param_a, in_param_b, in_param_c, in_param_d, in_param_e, in_param_f, in_param_g, in_param_h);
}
```

- Pointer validity must be checked via the `!` operator, rather than explicitly comparing against `nullptr`.

```cpp
void example_function(int* in_param)
{
    // Good
    if (!in_param)
        return;

    // Bad
    if (in_param == nullptr)
        return;

    // Good
    if (in_param)
        do_stuff(in_param);

    // Bad
    if (in_param != nullptr)
        do_stuff(in_param);
}
```

- Functions that return zero for success must be explicitly compared against zero for clarity, rather than using the `!` operator. If the library providing this function has its own error constants, use them where applicable.

```cpp
int result{};

// Good: The success result is clear.

if (erroring_function(&result) == 0)
    on_success(result);

if (erroring_function(&result) != 0)
    on_failure(result);

// Bad: The return value is ambiguous and could be confused for an operation that
//      occurs on failure.

if (!erroring_function(&result))
    on_success(result);

if (erroring_function(&result))
    on_failure(result);

// Good: Using an error constant from the library that provides this function. Do
//       not mix error constants from libraries unrelated to the function being
//       called.
if (lib1_erroring_function(&result) == LIB1_ERROR_SUCCESS)
    on_success(result);

// Bad: The error constant may not be guaranteed to work with the library
//      function's result, and may not always be available.
if (lib1_erroring_function(&result) == LIB2_ERROR_SUCCESS)
    on_success(result);
```

- Source files must end with an empty line.

## Exceptions
Avoid using exceptions and library functions that can throw exceptions. If a library function provides overloads for receiving error codes instead, use them where applicable. Otherwise, catch the exception and return an error status.

```cpp
void example_function_a(const std::filesystem::path& in_path)
{
    // Bad: This function throws an exception, use the error code overload to
    //      suppress it.
    if (std::filesystem::exists(in_path))
        do_stuff(in_path);

    // Good: This suppresses the function's exception and provides an error code to
    //       work with instead.
    std::error_code error_code{};
    if (std::filesystem::exists(in_path, error_code))
        do_stuff(in_path);
}

// Good: The function that throws an exception can be suppressed and return a
//       status of any kind instead.
bool example_function_b()
{
    try
    {
        lib_function_that_throws();
    }
    catch
    {
        return false;
    }

    return true;
}
```

## Return Values
- Functions that return a value must start with a `result` variable for storing the return value.
- The `result` variable must be mutated until the end of the function where it is returned, if possible.
- If the function early returns, return the `result` variable.

```cpp
std::vector<int> example_function(bool* in_modifier)
{
    std::vector<int> result{};

    // Bad: Constructs a new object for the return value, rather than using the
    //      existing one that would be empty here.
    if (!in_modifier)
        return {};
    
    // Good
    if (!in_modifier)
        return result;

    if (*in_modifier)
    {
        result = { 1, 2, 3 };
    }
    else
    {
        result = { 4, 5, 6 };
    }

    // Good: Uses named return value optimisation.
    return result;
}
```
