#include "hoe_bytecode.h"

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_read.h"
#include "file_write.h"
#include "hoe_bytecode.h"
#include "lstring.h"

/*
**  Let's call the scripting language of the `.hoe` files "hoelang"
**  (pronounced "holang")
*/

int in_function = 0;
int in_if = 0;
int in_then = 0;

int is_lstring_char(uint8_t c);
void print_hoe_event_var(hoe_var *var);

void print_indent(int n)
{
    int indent_size = 4;
    for (int a = 0; a < n; a++)
    {
        for (int b = 0; b < indent_size; b++)
        {
            printf(" ");
        }
    }
}

uint32_t read_4byte_char(uint8_t *bytecode, size_t *i)
{
    // what if we read beyond the limit?
    uint32_t res = lsb_32(*(uint32_t *)(bytecode + *i));
    *i += 4;
    return res;
}

uint8_t read_1byte_char(uint8_t *bytecode, size_t *i)
{
    // what if we read beyond the limit?
    uint8_t res = bytecode[*i];
    *i += 1;
    return res;
}

int print_hoelang_expr(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation);

int print_hoelang_push(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation)
{
    uint32_t expr_type = read_4byte_char(bytecode, i);
    switch (expr_type)
    {
    case 1:
        uint32_t value = read_4byte_char(bytecode, i);

        // TODO: try to use variable names from lstrings instead of varX
        printf("var%d", value);
        break;
    case 2:
        uint32_t hoe_var_index = read_4byte_char(bytecode, i);
        print_hoe_event_var(event->hoe_vars + hoe_var_index);
        printf("(hoe_vars[%d])", hoe_var_index);
        break;
    case 3:
        uint32_t value_3 = read_4byte_char(bytecode, i);
        printf("0x03 push %d", value_3);
        break;
    case 4:
    case 6:
        in_function = 1;
        uint32_t fun_len = read_4byte_char(bytecode, i);
        uint8_t fun_name[512] = {0};
        for (size_t a = 0; a < fun_len; a++)
        {
            fun_name[a] = bytecode[*i + a];
        }

        *i += fun_len;

        uint32_t nb_args = read_4byte_char(bytecode, i);
        printf("%s(", fun_name);
        if (nb_args)
        {
            for (size_t a = 0; a < nb_args; a++)
            {
                uint8_t arg_mark = read_1byte_char(bytecode, i);
                switch (arg_mark)
                {
                case 1:
                    uint32_t given_n = read_4byte_char(bytecode, i);
                    printf("%d", given_n);
                    break;
                case 2:
                    if (print_hoelang_expr(event, bytecode, i, indentation + 1))
                    {
                        return 1;
                    }

                    break;
                default:
                    printf("%02X (arg)", arg_mark);
                    break;
                }

                if (a != nb_args - 1)
                {
                    printf(", ");
                }
            }
        }
        printf(")");
        in_function = 0;
        break;
    case 5:
        uint32_t number = read_4byte_char(bytecode, i);

        // we ignore this number, it is probably always 1 and it is probably
        // the number of arguments
        read_4byte_char(bytecode, i);

        switch (number)
        {
        case 2:
            printf("sin(");
            break;
        case 9:
            printf("abs(");
            break;
        default:
            printf("some_math_fun(");
            break;
        }

        print_hoelang_expr(event, bytecode, i, indentation);
        printf(")");
        break;
    case 8:
        uint32_t str_len = read_4byte_char(bytecode, i);
        uint8_t str[512] = {0};
        for (size_t a = 0; a < str_len; a++)
        {
            str[a] = bytecode[*i + a];
        }

        *i += str_len;

        printf("string %s", str);
        break;
    case 7:
        printf("return");
        break;
    default:
        printf("??? unknown push %08X", expr_type);
        return 1;
    }

    return 0;
}

int print_hoelang_math_op(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation, char *op)
{
    if (print_hoelang_push(event, bytecode, i, indentation))
    {
        return 1;
    }

    printf (" %s ", op);

    if (print_hoelang_push(event, bytecode, i, indentation))
    {
        return 1;
    }

    return 0;
}

int print_hoelang_add(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation)
{
    return print_hoelang_math_op(event, bytecode, i, indentation, "+");
}

int print_hoelang_sub(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation)
{
    return print_hoelang_math_op(event, bytecode, i, indentation, "-");
}

int print_hoelang_mul(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation)
{
    return print_hoelang_math_op(event, bytecode, i, indentation, "*");
}

int print_hoelang_div(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation)
{
    return print_hoelang_math_op(event, bytecode, i, indentation, "/");
}

int print_hoelang_mod(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation)
{
    return print_hoelang_math_op(event, bytecode, i, indentation, "%");
}

int print_hoelang_expr(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation)
{
    uint32_t expr_op = read_4byte_char(bytecode, i);
    switch (expr_op)
    {
    case 0x65:
        if (print_hoelang_push(event, bytecode, i, indentation))
        {
            return 1;
        }
        break;
    case 0x66:
        if (print_hoelang_add(event, bytecode, i, indentation))
        {
            return 1;
        }
        break;
    case 0x67:
        if (print_hoelang_sub(event, bytecode, i, indentation))
        {
            return 1;
        }
        break;
    case 0x68:
        if (print_hoelang_mul(event, bytecode, i, indentation))
        {
            return 1;
        }
        break;
    case 0x69:
        if (print_hoelang_div(event, bytecode, i, indentation))
        {
            return 1;
        }
        break;
    case 0x6A:
        if (print_hoelang_mod(event, bytecode, i, indentation))
        {
            return 1;
        }
        break;
    default:
        print_indent(indentation);
        printf("??? unknown expr %08X", expr_op);
        return 1;
    }

    return 0;
}

int print_hoelang_assign(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation)
{
    size_t zero_one = read_4byte_char(bytecode, i);
    uint32_t expr_op = read_4byte_char(bytecode, i);
    *i -= 4;
    int j = 0;
    while (expr_op == 0x65
        || expr_op == 0x66
        || expr_op == 0x67
        || expr_op == 0x68
        || expr_op == 0x69
        || expr_op == 0x6A
    )
    {
        if (j)
        {
            if (zero_one)
            {
                // Printing this does not make too much sense, but it probably
                // never happens. If it ever does, we will do more research
                printf(" %d= ", zero_one);
            }
            else
            {
                printf(" = ");
            }
        }
        if (print_hoelang_expr(event, bytecode, i, indentation + 1))
        {
            return 1;
        }
        j++;
        expr_op = read_4byte_char(bytecode, i);
        *i -= 4;
    }

    return 0;
}

// comparison
int print_hoelang_cmp(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation, char *cmp)
{
    size_t zero_one = read_4byte_char(bytecode, i);
    uint32_t expr_op = read_4byte_char(bytecode, i);
    *i -= 4;
    int j = 0;

    // if necessary, we wrap the comparison with the NOT operator
    if (zero_one)
    {
        printf("NOT(");
    }

    while (0
        || expr_op == 0x65
        || expr_op == 0x66
        || expr_op == 0x67
        || expr_op == 0x68
        || expr_op == 0x69
        || expr_op == 0x6A
    )
    {
        if (j)
        {
            printf(" %s ", cmp);
        }
        if (print_hoelang_expr(event, bytecode, i, indentation + 1))
        {
            return 1;
        }

        j++;
        expr_op = read_4byte_char(bytecode, i);
        *i -= 4;
    }

    // end of wrapping
    if (zero_one)
    {
        printf(")");
    }

    return 0;
}

// greater than
int print_hoelang_cmp_gt(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation)
{
    return print_hoelang_cmp(event, bytecode, i, indentation, ">");
}

// greater or equal
int print_hoelang_cmp_ge(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation)
{
    return print_hoelang_cmp(event, bytecode, i, indentation, ">=");
}

// lower than
int print_hoelang_cmp_lt(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation)
{
    return print_hoelang_cmp(event, bytecode, i, indentation, "<");
}

// lower or equal
int print_hoelang_cmp_le(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation)
{
    return print_hoelang_cmp(event, bytecode, i, indentation, "<=");
}

// equal
int print_hoelang_cmp_eq(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation)
{
    return print_hoelang_cmp(event, bytecode, i, indentation, "==");
}

// not equal
int print_hoelang_cmp_ne(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation)
{
    return print_hoelang_cmp(event, bytecode, i, indentation, "!=");
}

// C9, not sure if it is a bool, there might be a better name for this function
int print_hoelang_bool(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation)
{
    size_t zero_one = read_4byte_char(bytecode, i);
    if (zero_one == 1)
    {
        printf("NOT ");
    }
    else if (zero_one)
    {
        printf("C9, %d: ", zero_one);
    }
    if (print_hoelang_expr(event, bytecode, i, indentation + 1))
    {
        return 1;
    }

    return 0;
}

int print_hoelang_block(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation)
{
    uint32_t op = read_4byte_char(bytecode, i);
    uint32_t zero_one;
    switch (op)
    {
    case 0xD0:
        return print_hoelang_assign(event, bytecode, i, indentation);
    case 0xCA:
        return print_hoelang_cmp_eq(event, bytecode, i, indentation);
    case 0xCB:
        return print_hoelang_cmp_ne(event, bytecode, i, indentation);
    case 0xCC:
        return print_hoelang_cmp_gt(event, bytecode, i, indentation);
    case 0xCD:
        return print_hoelang_cmp_lt(event, bytecode, i, indentation);
    case 0xCE:
        return print_hoelang_cmp_ge(event, bytecode, i, indentation);
    case 0xCF:
        return print_hoelang_cmp_le(event, bytecode, i, indentation);
    case 0xC9:
        if (print_hoelang_bool(event, bytecode, i, indentation))
        {
            return 1;
        }

        break;
    default:
        printf("??? unknown block at offset: %zu", *i);
        return 1;
    }

    return 0;
}

int print_hoelang_sub_block(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation);

int print_hoelang_if(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation, int type)
{
    int conditional = 0;
    int extra_indentation = 0;
    uint32_t nb_blocks = read_4byte_char(bytecode, i);
    if (nb_blocks)
        conditional = 1;

    if (conditional)
    {
        extra_indentation += 1;
        printf("if (");

        // start of condition
        for (size_t j = 0; j < nb_blocks; j++)
        {
            if (j)
                printf(" && ");
            if (j == nb_blocks - 1)
            {
                in_if = 1;
            }

            if (print_hoelang_block(event, bytecode, i, indentation + 1))
            {
                return 1;
            }
        }
        printf(")\n");
        print_indent(indentation);
        printf("{");
    }

    in_if = 0;
    nb_blocks = read_4byte_char(bytecode, i);

    // start of if scope
    for (size_t j = 0; j < nb_blocks; j++)
    {
        if (j == nb_blocks - 1)
        {
            in_then = 1;
        }

        if (conditional || j != 0)
        {
            putchar('\n');
            print_indent(indentation + extra_indentation);
        }
        if (type == 1)
        {
            if (print_hoelang_block(event, bytecode, i, indentation + extra_indentation))
                return 1;
        }
        else if (type == 2)
        {
            if (print_hoelang_sub_block(event, bytecode, i, indentation + extra_indentation))
                return 1;
        }
    }

    in_then = 0;
    if (conditional)
    {
        putchar('\n');
        print_indent(indentation);
        printf("}");
    }
}

int print_hoelang_main(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation);

int print_hoelang_sub_block(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation)
{
    uint8_t sub_block_marker = read_1byte_char(bytecode, i);
    switch (sub_block_marker)
    {
    case 1:
        print_hoelang_if(event, bytecode, i, indentation, 1);
        break;
    case 2:
        print_hoelang_if(event, bytecode, i, indentation, 2);
        break;
    default:
        printf("??? unknown %02X sub block", sub_block_marker);
        return 1;
    }

    return 0;
}

void print_hoelang_mask(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation)
{
    print_indent(indentation);
    uint32_t len_str = read_4byte_char(bytecode, i);
    printf("Len string: %d\n", len_str);
    print_indent(indentation);
    for (size_t a = 0; a < len_str; a++)
    {
        uint8_t c = read_1byte_char(bytecode, i);
        putchar(c);
    }

    putchar('\n');
    print_indent(indentation);

    uint32_t nb_uk_ints1 = read_4byte_char(bytecode, i);
    printf("Nb uk ints1: %d\n", nb_uk_ints1);
    for (size_t j = 0; j < nb_uk_ints1; j++)
    {
        print_indent(indentation);
        printf("Uk int: %d\n", read_4byte_char(bytecode, i));
    }

    uint32_t nb_m1 = read_4byte_char(bytecode, i);
    print_indent(indentation);
    printf("Nb m1: %d\n", nb_m1);
    for (size_t j = 0; j < nb_m1; j++)
    {
        print_indent(indentation + 1);
        printf("M1: %d\n", read_4byte_char(bytecode, i));
    }

    // we ignore the next two integers for now
    uint32_t number = read_4byte_char(bytecode, i);
    print_indent(indentation);
    printf("Number 1: %d\n", number);
    number = read_4byte_char(bytecode, i);
    print_indent(indentation);
    printf("Number 2: %d\n", number);

    // we print a block
    print_indent(indentation);
    print_hoelang_block(event, bytecode, i, indentation);
    putchar('\n');

    // we ignore the next integer for now
    number = read_4byte_char(bytecode, i);
    print_indent(indentation);
    printf("Number 3: %d\n", number);

    // we print an expr
    print_indent(indentation);
    print_hoelang_expr(event, bytecode, i, indentation);
    putchar('\n');
}

int print_hoelang_main(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation)
{
    // TODO: rename "block" and "sub_block"
    uint32_t nb_blocks = read_4byte_char(bytecode, i);
    for (size_t j = 0; j < nb_blocks; j++)
    {
        if (*i >= event->len_bytecode)
        {
            return 1;
        }

        print_indent(indentation);
        if (print_hoelang_block(event, bytecode, i, indentation))
        {
            return 1;
        }

        putchar('\n');
    }

    print_indent(indentation);
    uint32_t nb_sub_blocks = read_4byte_char(bytecode, i);
    printf("while (true) {\n", nb_sub_blocks);
    for (size_t j = 0; j < nb_sub_blocks; j++)
    {
        if (*i >= event->len_bytecode)
        {
            return 1;
        }

        print_indent(indentation + 1);
        if (print_hoelang_sub_block(event, bytecode, i, indentation + 1))
            return 1;
        putchar('\n');
    }
    print_indent(indentation);
    printf("}\n");

    return 0;
}