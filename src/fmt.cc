#include "fmt.h"
#include "usart.h"


namespace {
    void print_decimal(OutStream& out_stream, uint32_t val) {
        if (val == 0) {
            out_stream.send_char('0');
            return;
        }

        char buf[17];  // ?
        std::int8_t i = 0;

        for (; val > 0; val = val/10) {
            buf[i++] = val%10 + '0';
        }

        --i;
        while (i >= 0) {
            out_stream.send_char(buf[i--]);
        }
    }

    void print_hex(OutStream& out_stream, uint32_t val) {
        char buf[17];  // TODO
        std::int8_t i = 0;

        for (; val > 0; val = val / 16) {
            uint8_t const digit = val % 16;

            if (digit < 10) {
                buf[i++] = digit + '0';
            } else {
                buf[i++] = digit + 'a' - 10;
            }
        }

        --i;
        while (i >= 0) {
            out_stream.send_char(buf[i--]);

            if (i == 3) {
                out_stream.send_char(' ');
            }
        }
    }

    inline void print_number(OutStream& out_stream, uint32_t val, char fmt_flag, char default_fmt = 'd') {
        char fmt = fmt_flag == 0 ? default_fmt : fmt_flag;

        if (fmt == 'd') {
            print_decimal(out_stream, val);
        } else if (fmt == 'x') {
            print_hex(out_stream, val);
        } else {
            out_stream.send_char('?'); out_stream.send_char('f'); out_stream.send_char('?');
        }
    }
}


void Arg::print(OutStream& out_stream, char fmt_flag) const {
    if (type == SIGNED) {
        std::uint32_t val;

        if (u.signed_value < 0) {
            out_stream.send_char('-');
            val = -u.signed_value;
        } else {
            val = +u.signed_value;
        }

        print_number(out_stream, val, fmt_flag);

    } else if (type == UNSIGNED) {
        print_number(out_stream, u.unsigned_value, fmt_flag);

    } else if (type == CHAR) {
        out_stream.send_char(u.char_value);

    } else if (type == CHARP) {
        for (auto i = u.charp_value; *i != 0; ++i) {
            out_stream.send_char(*i);
        }

    } else if (type == POINTER) {
        print_number(out_stream, u.pointer_value, fmt_flag, 'x');  // TODO passing size_t into a uint32_t

    } else {
        out_stream.send_char('?');
    }
}


void do_safe_printf(OutStream& out_stream, const char *format, const Arg *args, std::size_t num_args) {
    auto i = format;
    std::size_t current_arg = 0;

    while (*i != 0) {
        if (*i == '{') {
            ++i;

            std::size_t arg_n = 0;
            bool has_arg_n = false;
            char fmt_flag = 0;

            while (*i >= '0' && *i <= '9') {
                has_arg_n = true;
                arg_n = 10 * arg_n + *i - '0';
                ++i;
            }

            if (*i == ':') {  // format syntax: {:x} or {3:x}
                ++i;

                if ((*i >= 'A' && *i <= 'Z') || (*i >= 'a' && *i <= 'z')) {
                    fmt_flag = *i;
                    ++i;
                }
            }

            if (*i == '}') {
                ++i;
            } else {
                out_stream.send_char('?'); out_stream.send_char('}'); out_stream.send_char('?');
                return;
            }

            if (!has_arg_n) {
                arg_n = current_arg++;
            }

            if (arg_n >= num_args) {
                out_stream.send_char('?'); out_stream.send_char('N'); out_stream.send_char('?');
                return;
            }

            args[arg_n].print(out_stream, fmt_flag);
        } else {
            out_stream.send_char(*i++);
        }
    }

    out_stream.complete();
}
