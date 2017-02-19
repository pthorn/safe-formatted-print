#ifndef FMT_H
#define FMT_H

/*
 * from http://stackoverflow.com/a/25392245/1092084
 * Format string syntax https://github.com/fmtlib/fmt/blob/master/doc/syntax.rst
 */

#include <cstddef>
#include <cstdint>


class Arg {
public:
    Arg(int value)
    : type(SIGNED) {
        u.signed_value = value;
    }

    Arg(unsigned value)
    : type(UNSIGNED) {
        u.unsigned_value = value;
    }

    Arg(unsigned long value)  // TODO assumes sizeof(int) == sizeof(long)!
    : type(UNSIGNED) {
        u.unsigned_value = value;
    }

//    Arg(uint8_t value)
//    : type(INT) {
//        u.int_value = value;
//    }

//    Arg(int8_t value)
//    : type(INT) {
//        u.int_value = value;
//    }

    Arg(char value)
    : type(CHAR) {
        u.char_value = value;
    }

    Arg(const char *value)
    : type(CHARP) {
        u.charp_value = const_cast<char*>(value);
    }

    Arg(const void *value)
    : type(POINTER) {
        u.pointer_value = reinterpret_cast<std::size_t>(value);
    }

// TODO #ifdef
//    Arg(double value)
//    : type(DOUBLE) {
//        u.double_value = value;
//    }

    void print(OutStream& out_stream, char fmt_flag) const;

private:
  enum Type {
      SIGNED, UNSIGNED, CHAR, CHARP, POINTER //,DOUBLE
  };

  Type type;

  union {
    int32_t signed_value;
    uint32_t unsigned_value;
    char char_value;
    char *charp_value;
    size_t pointer_value;
    //double double_value;
  } u;
};


void do_safe_printf(OutStream& out_stream, const char *format, const Arg *args, std::size_t num_args);

template <typename... Args>
void fmt_print(OutStream& out_stream, const char *format, const Args&... args) {
    Arg arg_array[] = {args...};
    do_safe_printf(out_stream, format, arg_array, sizeof...(Args));
}

#endif // FMT_H
