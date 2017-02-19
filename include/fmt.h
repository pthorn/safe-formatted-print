#ifndef FMT_H
#define FMT_H

/*
 * from http://stackoverflow.com/a/25392245/1092084
 * Format string syntax https://github.com/fmtlib/fmt/blob/master/doc/syntax.rst
 */

#include <cstddef>
#include <cstdint>


class OutStream {
public:
    virtual void send_char(char c) = 0;
    virtual void complete() = 0;

    virtual ~OutStream() { }
};


class Arg {
    friend class Printer;

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


class Printer {
public:
    Printer(OutStream& out_stream):
        out_stream(out_stream) { }

    void operator()(char const* format, Arg const* args, std::size_t num_args);

private:
    void print_arg(const Arg &arg);
    void print_number(uint32_t val);
    void number_to_string(char *buf, uint32_t val);

    OutStream& out_stream;
    uint8_t base;
    uint8_t print_base;  // boolean
    uint8_t width;
    char fill_char;
    uint8_t left_justify;  // boolean
};


template <typename... Args>
void fmt_print(Printer const& printer, const char *format, const Args&... args) {
    Arg const arg_array[] = {args...};
    std::size_t const num_args = sizeof...(Args);

    printer(format, arg_array, num_args);
}


template <typename... Args>
void fmt_print(OutStream& out_stream, const char *format, const Args&... args) {
    Arg const arg_array[] = {args...};
    std::size_t const num_args = sizeof...(Args);

    Printer printer(out_stream);
    printer(format, arg_array, num_args);
}

#endif // FMT_H
