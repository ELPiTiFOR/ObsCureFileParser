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

void print_hoelang_expr(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation);

void print_hoelang_push(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation)
{
    print_indent(indentation);

    printf("PUSH ");
    uint32_t expr_type = read_4byte_char(bytecode, i);
    switch (expr_type)
    {
    case 1:
        uint32_t value = read_4byte_char(bytecode, i);
        printf("immediate %d", value);
        break;
    case 2:
        uint32_t hoe_var_index = read_4byte_char(bytecode, i);
        printf("hoe_var index=%d, value=", hoe_var_index);
        print_hoe_event_var(event->hoe_vars + hoe_var_index);
        break;
    case 4:
        in_function = 1;
        uint32_t fun_len = read_4byte_char(bytecode, i);
        uint8_t fun_name[512] = {0};
        for (size_t a = 0; a < fun_len; a++)
        {
            fun_name[a] = bytecode[*i + a];
        }

        *i += fun_len;

        uint32_t nb_args = read_4byte_char(bytecode, i);
        printf("Fun %s(", fun_name);
        if (nb_args)
        {
            for (size_t a = 0; a < nb_args; a++)
            {
                uint8_t arg_mark = read_1byte_char(bytecode, i);
                putchar('\n');
                switch (arg_mark)
                {
                case 1:
                    uint32_t given_n = read_4byte_char(bytecode, i);
                    print_indent(indentation + 1);
                    printf("01(arg): %d", given_n);
                    break;
                case 2:
                    print_hoelang_expr(event, bytecode, i, indentation + 1);
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
            putchar('\n');
            print_indent(indentation);
        }
        printf(")");
        in_function = 0;
        break;
    case 5:
        printf("0x05:\n");
        for (size_t a = 0; a < 5; a++)
        {
            print_indent(indentation + 1);
            uint32_t number = read_4byte_char(bytecode, i);
            printf("%08X", number);
            if (a != 4)
            {
                putchar('\n');
            }
        }
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
        printf("0x07 (RET?)");
        break;
    default:
        printf("??? the fuck is this push %08X", expr_type);
        break;
    }
}

void print_hoelang_expr(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation)
{
    // do not print anything (?)
    uint32_t expr_op = read_4byte_char(bytecode, i);
    switch (expr_op)
    {
    case 0x65:
        print_hoelang_push(event, bytecode, i, indentation);
        break;
    //case 0x66:
    //case 0x67:
    case 0x6A:
        print_indent(indentation);
        printf("0x%02X\n", expr_op);
        for (size_t a = 0; a < 4; a++)
        {
            print_indent(indentation + 1);
            uint32_t number = read_4byte_char(bytecode, i);
            printf("%08X", number);
            if (a != 3)
            {
                putchar('\n');
            }
        }
        uint8_t letter = read_1byte_char(bytecode, i);
        if (!is_lstring_char(letter))
        {
            *i -= 1;
            break;
        }

        printf(" ");
        while (is_lstring_char(letter))
        {
            putchar(letter);
            letter = read_1byte_char(bytecode, i);
        }
        
        *i -= 1;
        break;
    case 0x66:
    case 0x67:
    case 0x68:
        uint32_t first_number_66 = 0;
        uint32_t nb_expr_in_66 = 0;
        print_indent(indentation);
        printf("0x%02X\n", expr_op);
        uint8_t single = read_1byte_char(bytecode, i);
        *i -= 1;
        uint32_t n = read_4byte_char(bytecode, i);
        *i -= 4;
        int j = 0;
        // ho lee shit :|
        while (single
            || (1
            && (n != 0x65 || (expr_op == 0x66 && ((nb_expr_in_66 = nb_expr_in_66 + 1) || first_number_66 == 2) && nb_expr_in_66 != 2))
            && n != 0x66
            && n != 0x67
            && n != 0x68
            && n != 0x6A
            && n != 0xD0
            && n != 0xC9
            && n != 0xCA
            && n != 0xCB
            && n != 0xCC
            && n != 0xCD
            && n != 0xCF
            )
        )
        {
            if (j)
                putchar('\n');

            print_indent(indentation + 1);

            if (is_lstring_char(single))
            {
                *i += 1;
                while (is_lstring_char(single))
                {
                    putchar(single);
                    single = read_1byte_char(bytecode, i);
                }

                *i -= 1;
                n = read_4byte_char(bytecode, i);
                *i -= 4;
                j++;
                continue;
            }
            else if (single)
            {

                if ((single == 2 && in_function && (expr_op != 0x66 || first_number_66 == 1))
                    || (single == 1 && in_then)
                )
                {
                    break;
                }

                printf("%02X", single);
                *i += 1;
                j++;
                uint8_t old_single = single;
                single = read_1byte_char(bytecode, i);
                *i -= 1;
                n = read_4byte_char(bytecode, i);
                *i -= 4;
                if (old_single == 2 && expr_op != 0x66)
                {
                    break;
                }

                continue;
            }

            printf("%08X", n);
            if (j == 1)
            {
                first_number_66 = n;
            }
            *i += 4;
            j++;
            single = read_1byte_char(bytecode, i);
            *i -= 1;
            n = read_4byte_char(bytecode, i);
            *i -= 4;
        }

        if (in_if &&
            (0
            || n == 0xC9
            || n == 0xD0
            || n == 0xC9
            || n == 0xCA
            || n == 0xCB
            || n == 0xCC
            || n == 0xCD
            || n == 0xCF
            )
        )
            *i -= 4;
        else if (nb_expr_in_66 != 0 && in_function)
            *i -= 1;
        break;
    default:
        print_indent(indentation);
        printf("??? the fuck is this expr %08X", expr_op);
        break;
    }
}

void print_hoelang_block(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation)
{
    print_indent(indentation);

    uint32_t op = read_4byte_char(bytecode, i);
    uint32_t zero_one;
    switch (op)
    {
    case 0xD0:
    case 0xCA:
    case 0xCB:
    case 0xCC:
    case 0xCD:
    case 0xCE:
    case 0xCF:
        zero_one = read_4byte_char(bytecode, i);
        printf("%02X, %d:\n", op, zero_one);
        uint32_t expr_op = read_4byte_char(bytecode, i);
        *i -= 4;
        int j = 0;
        while (0
            || expr_op == 0x65
            || expr_op == 0x66
            || expr_op == 0x67
            || expr_op == 0x68
            || expr_op == 0x6A
        )
        {
            if (j)
                putchar('\n');
            print_hoelang_expr(event, bytecode, i, indentation + 1);
            j++;
            expr_op = read_4byte_char(bytecode, i);
            *i -= 4;
        }
        break;
    case 0xC9:
        zero_one = read_4byte_char(bytecode, i);
        printf("C9, %d:\n", zero_one);
        print_hoelang_expr(event, bytecode, i, indentation + 1);
        break;
    default:
        printf("??? offset: %zu", *i);
        break;
    }
}

void print_hoelang_main(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation);

void print_hoelang_sub_block(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation)
{
    print_indent(indentation);
    uint8_t sub_block_marker = read_1byte_char(bytecode, i);
    printf("%02X (sub_block) ", sub_block_marker);
    switch (sub_block_marker)
    {
    case 1:
        printf("if {\n");
        uint32_t nb_blocks = read_4byte_char(bytecode, i);
        for (size_t j = 0; j < nb_blocks; j++)
        {
            if (j == nb_blocks - 1)
            {
                in_if = 1;
            }

            print_hoelang_block(event, bytecode, i, indentation + 1);
            putchar('\n');
        }

        print_indent(indentation);
        in_if = 0;
        printf("} then {\n");
        nb_blocks = read_4byte_char(bytecode, i);
        for (size_t j = 0; j < nb_blocks; j++)
        {
            if (j == nb_blocks - 1)
            {
                in_then = 1;
            }

            print_hoelang_block(event, bytecode, i, indentation + 1);
            putchar('\n');
        }
        in_then = 0;
        print_indent(indentation);
        printf("}");
        break;
    case 2:
        putchar('\n');
        print_hoelang_main(event, bytecode, i, indentation + 1);
        break;
    default:
        printf("??? %02X sub block", sub_block_marker);
        break;
    }
}

void print_hoelang_main(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation)
{
    print_indent(indentation);
    uint32_t nb_blocks = read_4byte_char(bytecode, i);
    printf("nb_blocks = %d\n", nb_blocks);
    for (size_t j = 0; j < nb_blocks; j++)
    {
        print_hoelang_block(event, bytecode, i, indentation + 1);
        putchar('\n');
    }

    print_indent(indentation);
    uint32_t nb_sub_blocks = read_4byte_char(bytecode, i);
    printf("nb_sub_blocks = %d\n", nb_sub_blocks);
    for (size_t j = 0; j < nb_sub_blocks; j++)
    {
        print_hoelang_sub_block(event, bytecode, i, indentation + 1);
        putchar('\n');
    }
}