#include <cstring>

#include "fmt.h"


void Printer::number_to_string(char* buf, uint32_t val) {
    if (val == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }

    if (base == 0) {
        base = 10;
    }

    uint64_t x = 1;
    uint32_t n_digits = 0;

    while (x <= val) {
        x *= base;
        n_digits += 1;
    }

    for (std::uint8_t i = 0; val > 0; ++i) {
        uint8_t const digit = val % base;

        if (digit < 10) {
            buf[n_digits - i - 1] = digit + '0';
        } else {
            buf[n_digits - i - 1] = digit + (uppercase ? 'A' : 'a') - 10;
        }

        val = val / base;
    }

    buf[n_digits] = '\0';
}


void Printer::print_arg(Arg const& arg) {
    if (arg.type == Arg::CHAR) {
        out_stream.send_char(arg.u.char_value);
        return;
    }

    char buf[12];
    char* str_val;

    if (arg.type == Arg::CHARP) {
        str_val = arg.u.charp_value;
    } else {
        std::uint32_t num_val;

        if (arg.type == Arg::SIGNED) {
            num_val = (arg.u.signed_value < 0) ?
                -arg.u.signed_value : +arg.u.signed_value;
        } else if (arg.type == Arg::UNSIGNED) {
            num_val = arg.u.unsigned_value;
        } else if (arg.type == Arg::POINTER) {
            if (base == 0) {
                base = 16;
            }
            num_val = arg.u.pointer_value;  // TODO passing size_t into a uint32_t
        }

        number_to_string(buf, num_val);
        str_val = buf;

        if (arg.type == Arg::SIGNED && arg.u.signed_value < 0) {
            out_stream.send_char('-');
        }

        if (print_base) {
            if (base == 16) {
                out_stream.send_char('0');
                out_stream.send_char(uppercase ? 'X' : 'x');
            }
        }
    }

    if (fill_char != '\0') {
        uint32_t buf_width = std::strlen(buf);

        if (width > buf_width) {
            for (uint32_t i = 0; i < width - buf_width; ++i) {
                out_stream.send_char(fill_char);
            }
        }
    }

    for (auto i = str_val; *i != 0; ++i) {
        out_stream.send_char(*i);
    }
}


void Printer::operator()(char const* format, Arg const* args, std::size_t num_args)
{
    auto i = format;
    std::size_t current_arg = 0;

    while (*i != 0) {
        if (*i != '%') {
            out_stream.send_char(*i++);
            continue;
        }

        ++i;

        if (*i == '%') {
            out_stream.send_char(*i++);
            continue;
        }

        base = 0;  // 0 means 16 for pointers, 10 for everything else
        uppercase = false;
        print_base = false;
        width = 0;
        fill_char = '\0';
        left_justify = false;

        while (true) {
            if (*i == '#') {
                print_base = true;
            } else if (*i == '0') {
                fill_char ='0';
            } else if (*i == ' ') {
                fill_char =' ';
            } else if (*i == '-') {
                left_justify = true;
            } else {
                break;
            }

            ++i;
        }

        while (*i >= '0' && *i <= '9') {
            width = 10 * width + *i - '0';
            ++i;
        }

        if (*i == 's') {
            // TODO
        } else if (*i == 'd' || *i == 'i') {
            base = 10;
        } else if (*i == 'x') {
            base = 16;
        } else if (*i == 'X') {
            base = 16;
            uppercase = true;
        } else {
            // ERROR
        }

        ++i;

        if (current_arg >= num_args) {
            // ERROR
        } else {
            print_arg(args[current_arg++]);
        }
    }

    out_stream.complete();
}
