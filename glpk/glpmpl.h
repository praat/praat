/* glpmpl.h (GNU MathProg translator) */

/***********************************************************************
*  This code is part of GLPK (GNU Linear Programming Kit).
*
*  Copyright (C) 2000, 01, 02, 03, 04, 05, 06, 07, 08 Andrew Makhorin,
*  Department for Applied Informatics, Moscow Aviation Institute,
*  Moscow, Russia. All rights reserved. E-mail: <mao@mai2.rcnet.ru>.
*
*  GLPK is free software: you can redistribute it and/or modify it
*  under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  GLPK is distributed in the hope that it will be useful, but WITHOUT
*  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
*  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public
*  License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with GLPK. If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/

#ifndef _GLPMPL_H
#define _GLPMPL_H

#include "glpavl.h"
#include "glpdmp.h"
#include "glprng.h"

typedef struct MPL MPL;
#if 0
typedef struct STRING STRING;
#else
typedef char STRING;
#endif
typedef struct SYMBOL SYMBOL;
typedef struct TUPLE TUPLE;
typedef struct ARRAY ELEMSET;
typedef struct ELEMVAR ELEMVAR;
typedef struct FORMULA FORMULA;
typedef struct ELEMCON ELEMCON;
typedef union VALUE VALUE;
typedef struct ARRAY ARRAY;
typedef struct MEMBER MEMBER;
#if 1
/* many C compilers have DOMAIN declared in <math.h> :( */
#undef DOMAIN
#define DOMAIN DOMAIN1
#endif
typedef struct DOMAIN DOMAIN;
typedef struct DOMAIN_BLOCK DOMAIN_BLOCK;
typedef struct DOMAIN_SLOT DOMAIN_SLOT;
typedef struct SET SET;
typedef struct WITHIN WITHIN;
typedef struct PARAMETER PARAMETER;
typedef struct CONDITION CONDITION;
typedef struct VARIABLE VARIABLE;
typedef struct CONSTRAINT CONSTRAINT;
#if 1 /* 11/II-2008 */
typedef struct TABLE TABLE;
typedef struct TABARG TABARG;
typedef struct TABFLD TABFLD;
typedef struct TABIN TABIN;
typedef struct TABOUT TABOUT;
typedef struct TABDCA TABDCA;
#endif
typedef union OPERANDS OPERANDS;
typedef struct ARG_LIST ARG_LIST;
typedef struct CODE CODE;
typedef struct CHECK CHECK;
typedef struct DISPLAY DISPLAY;
typedef struct DISPLAY1 DISPLAY1;
typedef struct PRINTF PRINTF;
typedef struct PRINTF1 PRINTF1;
typedef struct FOR FOR;
typedef struct STATEMENT STATEMENT;
typedef struct TUPLE SLICE;

/**********************************************************************/
/* * *                    TRANSLATOR DATABASE                     * * */
/**********************************************************************/

#define A_BINARY        101   /* something binary */
#define A_CHECK         102   /* check statement */
#define A_CONSTRAINT    103   /* model constraint */
#define A_DISPLAY       104   /* display statement */
#define A_ELEMCON       105   /* elemental constraint/objective */
#define A_ELEMSET       106   /* elemental set */
#define A_ELEMVAR       107   /* elemental variable */
#define A_EXPRESSION    108   /* expression */
#define A_FOR           109   /* for statement */
#define A_FORMULA       110   /* formula */
#define A_INDEX         111   /* dummy index */
#define A_INPUT         112   /* input table */
#define A_INTEGER       113   /* something integer */
#define A_LOGICAL       114   /* something logical */
#define A_MAXIMIZE      115   /* objective has to be maximized */
#define A_MINIMIZE      116   /* objective has to be minimized */
#define A_NONE          117   /* nothing */
#define A_NUMERIC       118   /* something numeric */
#define A_OUTPUT        119   /* output table */
#define A_PARAMETER     120   /* model parameter */
#define A_PRINTF        121   /* printf statement */
#define A_SET           122   /* model set */
#define A_SOLVE         123   /* solve statement */
#define A_SYMBOLIC      124   /* something symbolic */
#define A_TABLE         125   /* data table */
#define A_TUPLE         126   /* n-tuple */
#define A_VARIABLE      127   /* model variable */

#define MAX_LENGTH 100
/* maximal length of any symbolic value (this includes symbolic names,
   numeric and string literals, and all symbolic values that may appear
   during the evaluation phase) */

#define CONTEXT_SIZE 60
/* size of the context queue, in characters */

#define OUTBUF_SIZE 1024
/* size of the output buffer, in characters */

struct MPL
{     /* translator database */
      /*--------------------------------------------------------------*/
      /* scanning segment */
      int line;
      /* number of the current text line */
      int c;
      /* the current character or EOF */
      int token;
      /* the current token: */
#define T_EOF           201   /* end of file */
#define T_NAME          202   /* symbolic name (model section only) */
#define T_SYMBOL        203   /* symbol (data section only) */
#define T_NUMBER        204   /* numeric literal */
#define T_STRING        205   /* string literal */
#define T_AND           206   /* and && */
#define T_BY            207   /* by */
#define T_CROSS         208   /* cross */
#define T_DIFF          209   /* diff */
#define T_DIV           210   /* div */
#define T_ELSE          211   /* else */
#define T_IF            212   /* if */
#define T_IN            213   /* in */
#if 1 /* 21/VII-2006 */
#define T_INFINITY      214   /* Infinity */
#endif
#define T_INTER         215   /* inter */
#define T_LESS          216   /* less */
#define T_MOD           217   /* mod */
#define T_NOT           218   /* not ! */
#define T_OR            219   /* or || */
#define T_SPTP          220   /* s.t. */
#define T_SYMDIFF       221   /* symdiff */
#define T_THEN          222   /* then */
#define T_UNION         223   /* union */
#define T_WITHIN        224   /* within */
#define T_PLUS          225   /* + */
#define T_MINUS         226   /* - */
#define T_ASTERISK      227   /* * */
#define T_SLASH         228   /* / */
#define T_POWER         229   /* ^ ** */
#define T_LT            230   /* <  */
#define T_LE            231   /* <= */
#define T_EQ            232   /* = == */
#define T_GE            233   /* >= */
#define T_GT            234   /* >  */
#define T_NE            235   /* <> != */
#define T_CONCAT        236   /* & */
#define T_BAR           237   /* | */
#define T_POINT         238   /* . */
#define T_COMMA         239   /* , */
#define T_COLON         240   /* : */
#define T_SEMICOLON     241   /* ; */
#define T_ASSIGN        242   /* := */
#define T_DOTS          243   /* .. */
#define T_LEFT          244   /* ( */
#define T_RIGHT         245   /* ) */
#define T_LBRACKET      246   /* [ */
#define T_RBRACKET      247   /* ] */
#define T_LBRACE        248   /* { */
#define T_RBRACE        249   /* } */
#if 1 /* 14/VII-2006 */
#define T_APPEND        250   /* >> */
#endif
#if 1 /* 11/II-2008 */
#define T_TILDE         251   /* ~ */
#define T_INPUT         252   /* <- */
#endif
      int imlen;
      /* length of the current token */
      char *image; /* char image[MAX_LENGTH+1]; */
      /* image of the current token */
      double value;
      /* value of the current token (for T_NUMBER only) */
      int b_token;
      /* the previous token */
      int b_imlen;
      /* length of the previous token */
      char *b_image; /* char b_image[MAX_LENGTH+1]; */
      /* image of the previous token */
      double b_value;
      /* value of the previous token (if token is T_NUMBER) */
      int f_dots;
      /* if this flag is set, the next token should be recognized as
         T_DOTS, not as T_POINT */
      int f_scan;
      /* if this flag is set, the next token is already scanned */
      int f_token;
      /* the next token */
      int f_imlen;
      /* length of the next token */
      char *f_image; /* char f_image[MAX_LENGTH+1]; */
      /* image of the next token */
      double f_value;
      /* value of the next token (if token is T_NUMBER) */
      char *context; /* char context[CONTEXT_SIZE]; */
      /* context circular queue (not null-terminated!) */
      int c_ptr;
      /* pointer to the current position in the context queue */
      int flag_d;
      /* if this flag is set, the data section is being processed */
      /*--------------------------------------------------------------*/
      /* translating segment */
      DMP *pool;
      /* memory pool used to allocate all data instances created during
         the translation phase */
      AVL *tree;
      /* symbolic name table:
         node.type = A_INDEX     => node.link -> DOMAIN_SLOT
         node.type = A_SET       => node.link -> SET
         node.type = A_PARAMETER => node.link -> PARAMETER
         node.type = A_VARIABLE  => node.link -> VARIABLE
         node.type = A_CONSTRANT => node.link -> CONSTRAINT */
      STATEMENT *model;
      /* linked list of model statements in the original order */
      int flag_x;
      /* if this flag is set, the current token being left parenthesis
         begins a slice that allows recognizing any undeclared symbolic
         names as dummy indices; this flag is automatically reset once
         the next token has been scanned */
      int as_within;
      /* the warning "in understood as within" has been issued */
      int as_in;
      /* the warning "within understood as in" has been issued */
      int as_binary;
      /* the warning "logical understood as binary" has been issued */
      int flag_s;
      /* if this flag is set, the solve statement has been parsed */
      /*--------------------------------------------------------------*/
      /* common segment */
      DMP *strings;
      /* memory pool to allocate STRING data structures */
      DMP *symbols;
      /* memory pool to allocate SYMBOL data structures */
      DMP *tuples;
      /* memory pool to allocate TUPLE data structures */
      DMP *arrays;
      /* memory pool to allocate ARRAY data structures */
      DMP *members;
      /* memory pool to allocate MEMBER data structures */
      DMP *elemvars;
      /* memory pool to allocate ELEMVAR data structures */
      DMP *formulae;
      /* memory pool to allocate FORMULA data structures */
      DMP *elemcons;
      /* memory pool to allocate ELEMCON data structures */
      ARRAY *a_list;
      /* linked list of all arrays in the database */
      char *sym_buf; /* char sym_buf[255+1]; */
      /* working buffer used by the routine format_symbol */
      char *tup_buf; /* char tup_buf[255+1]; */
      /* working buffer used by the routine format_tuple */
      /*--------------------------------------------------------------*/
      /* generating/postsolving segment */
      RNG *rand;
      /* pseudo-random number generator */
      int flag_p;
      /* if this flag is set, the postsolving phase is in effect */
      STATEMENT *stmt;
      /* model statement being currently executed */
#if 1 /* 11/II-2008 */
      TABDCA *dca;
      /* pointer to table driver communication area for table statement
         currently executed */
#endif
      int m;
      /* number of rows in the problem, m >= 0 */
      int n;
      /* number of columns in the problem, n >= 0 */
      ELEMCON **row; /* ELEMCON *row[1+m]; */
      /* row[0] is not used;
         row[i] is elemental constraint or objective, which corresponds
         to i-th row of the problem, 1 <= i <= m */
      ELEMVAR **col; /* ELEMVAR *col[1+n]; */
      /* col[0] is not used;
         col[j] is elemental variable, which corresponds to j-th column
         of the problem, 1 <= j <= n */
      /*--------------------------------------------------------------*/
      /* input/output segment */
      FILE *in_fp;
      /* stream assigned to the input text file */
      char *in_file;
      /* name of the input text file */
      FILE *out_fp;
      /* stream assigned to the output text file used to write all data
         produced by display and printf statements; NULL means the data
         should be sent to stdout via the routine print */
      char *out_file;
      /* name of the output text file */
      char *out_buf; /* char out_buf[OUTBUF_SIZE] */
      /* buffer to accumulate output data */
      int out_cnt;
      /* count of data bytes stored in the output buffer */
#if 1 /* 14/VII-2006 */
      FILE *prt_fp;
      /* stream assigned to the print text file; may be NULL */
      char *prt_file;
      /* name of the output print file */
#endif
      /*--------------------------------------------------------------*/
      /* solver interface segment */
      jmp_buf jump;
      /* jump address for non-local go to in case of error */
      int phase;
      /* phase of processing:
         0 - database is being or has been initialized
         1 - model section is being or has been read
         2 - data section is being or has been read
         3 - model is being or has been generated/postsolved
         4 - model processing error has occurred */
      char *mod_file;
      /* name of the input text file, which contains model section */
      char *mpl_buf; /* char mpl_buf[255+1]; */
      /* working buffer used by some interface routines */
};

#define create_slice          _glp_mpl_create_slice
#define expand_slice          _glp_mpl_expand_slice
#define slice_dimen           _glp_mpl_slice_dimen
#define slice_arity           _glp_mpl_slice_arity
#define fake_slice            _glp_mpl_fake_slice
#define delete_slice          _glp_mpl_delete_slice
#define is_number             _glp_mpl_is_number
#define is_symbol             _glp_mpl_is_symbol
#define is_literal            _glp_mpl_is_literal
#define read_number           _glp_mpl_read_number
#define read_symbol           _glp_mpl_read_symbol
#define read_slice            _glp_mpl_read_slice
#define select_set            _glp_mpl_select_set
#define simple_format         _glp_mpl_simple_format
#define matrix_format         _glp_mpl_matrix_format
#define set_data              _glp_mpl_set_data
#define select_parameter      _glp_mpl_select_parameter
#define set_default           _glp_mpl_set_default
#define read_value            _glp_mpl_read_value
#define plain_format          _glp_mpl_plain_format
#define tabular_format        _glp_mpl_tabular_format
#define tabbing_format        _glp_mpl_tabbing_format
#define parameter_data        _glp_mpl_parameter_data
#define data_section          _glp_mpl_data_section

#define fp_add                _glp_mpl_fp_add
#define fp_sub                _glp_mpl_fp_sub
#define fp_less               _glp_mpl_fp_less
#define fp_mul                _glp_mpl_fp_mul
#define fp_div                _glp_mpl_fp_div
#define fp_idiv               _glp_mpl_fp_idiv
#define fp_mod                _glp_mpl_fp_mod
#define fp_power              _glp_mpl_fp_power
#define fp_exp                _glp_mpl_fp_exp
#define fp_log                _glp_mpl_fp_log
#define fp_log10              _glp_mpl_fp_log10
#define fp_sqrt               _glp_mpl_fp_sqrt
#define fp_sin                _glp_mpl_fp_sin
#define fp_cos                _glp_mpl_fp_cos
#define fp_atan               _glp_mpl_fp_atan
#define fp_atan2              _glp_mpl_fp_atan2
#define fp_round              _glp_mpl_fp_round
#define fp_trunc              _glp_mpl_fp_trunc
#define fp_irand224           _glp_mpl_fp_irand224
#define fp_uniform01          _glp_mpl_fp_uniform01
#define fp_uniform            _glp_mpl_uniform
#define fp_normal01           _glp_mpl_fp_normal01
#define fp_normal             _glp_mpl_fp_normal
#define create_string         _glp_mpl_create_string
#define copy_string           _glp_mpl_copy_string
#define compare_strings       _glp_mpl_compare_strings
#define fetch_string          _glp_mpl_fetch_string
#define delete_string         _glp_mpl_delete_string
#define create_symbol_num     _glp_mpl_create_symbol_num
#define create_symbol_str     _glp_mpl_create_symbol_str
#define copy_symbol           _glp_mpl_copy_symbol
#define compare_symbols       _glp_mpl_compare_symbols
#define delete_symbol         _glp_mpl_delete_symbol
#define format_symbol         _glp_mpl_format_symbol
#define concat_symbols        _glp_mpl_concat_symbols
#define create_tuple          _glp_mpl_create_tuple
#define expand_tuple          _glp_mpl_expand_tuple
#define tuple_dimen           _glp_mpl_tuple_dimen
#define copy_tuple            _glp_mpl_copy_tuple
#define compare_tuples        _glp_mpl_compare_tuples
#define build_subtuple        _glp_mpl_build_subtuple
#define delete_tuple          _glp_mpl_delete_tuple
#define format_tuple          _glp_mpl_format_tuple
#define create_elemset        _glp_mpl_create_elemset
#define find_tuple            _glp_mpl_find_tuple
#define add_tuple             _glp_mpl_add_tuple
#define check_then_add        _glp_mpl_check_then_add
#define copy_elemset          _glp_mpl_copy_elemset
#define delete_elemset        _glp_mpl_delete_elemset
#define arelset_size          _glp_mpl_arelset_size
#define arelset_member        _glp_mpl_arelset_member
#define create_arelset        _glp_mpl_create_arelset
#define set_union             _glp_mpl_set_union
#define set_diff              _glp_mpl_set_diff
#define set_symdiff           _glp_mpl_set_symdiff
#define set_inter             _glp_mpl_set_inter
#define set_cross             _glp_mpl_set_cross
#define constant_term         _glp_mpl_constant_term
#define single_variable       _glp_mpl_single_variable
#define copy_formula          _glp_mpl_copy_formula
#define delete_formula        _glp_mpl_delete_formula
#define linear_comb           _glp_mpl_linear_comb
#define remove_constant       _glp_mpl_remove_constant
#define reduce_terms          _glp_mpl_reduce_terms
#define delete_value          _glp_mpl_delete_value
#define create_array          _glp_mpl_create_array
#define find_member           _glp_mpl_find_member
#define add_member            _glp_mpl_add_member
#define delete_array          _glp_mpl_delete_array
#define assign_dummy_index    _glp_mpl_assign_dummy_index
#define update_dummy_indices  _glp_mpl_update_dummy_indices
#define enter_domain_block    _glp_mpl_enter_domain_block
#define eval_within_domain    _glp_mpl_eval_within_domain
#define loop_within_domain    _glp_mpl_loop_within_domain
#define out_of_domain         _glp_mpl_out_of_domain
#define get_domain_tuple      _glp_mpl_get_domain_tuple
#define clean_domain          _glp_mpl_clean_domain
#define check_elem_set        _glp_mpl_check_elem_set
#define take_member_set       _glp_mpl_take_member_set
#define eval_member_set       _glp_mpl_eval_member_set
#define eval_whole_set        _glp_mpl_eval_whole_set
#define clean_set             _glp_mpl_clean_set
#define check_value_num       _glp_mpl_check_value_num
#define take_member_num       _glp_mpl_take_member_num
#define eval_member_num       _glp_mpl_eval_member_num
#define check_value_sym       _glp_mpl_check_value_sym
#define take_member_sym       _glp_mpl_take_member_sym
#define eval_member_sym       _glp_mpl_eval_member_sym
#define eval_whole_par        _glp_mpl_eval_whole_par
#define clean_parameter       _glp_mpl_clean_parameter
#define take_member_var       _glp_mpl_take_member_var
#define eval_member_var       _glp_mpl_eval_member_var
#define eval_whole_var        _glp_mpl_eval_whole_var
#define clean_variable        _glp_mpl_clean_variable
#define take_member_con       _glp_mpl_take_member_con
#define eval_member_con       _glp_mpl_eval_member_con
#define eval_whole_con        _glp_mpl_eval_whole_con
#define clean_constraint      _glp_mpl_clean_constraint
#define eval_numeric          _glp_mpl_eval_numeric
#define eval_symbolic         _glp_mpl_eval_symbolic
#define eval_logical          _glp_mpl_eval_logical
#define eval_tuple            _glp_mpl_eval_tuple
#define eval_elemset          _glp_mpl_eval_elemset
#define is_member             _glp_mpl_is_member
#define eval_formula          _glp_mpl_eval_formula
#define clean_code            _glp_mpl_clean_code
#define execute_check         _glp_mpl_execute_check
#define clean_check           _glp_mpl_clean_check
#define execute_display       _glp_mpl_execute_display
#define clean_display         _glp_mpl_clean_display
#define execute_printf        _glp_mpl_execute_printf
#define clean_printf          _glp_mpl_clean_printf
#define execute_for           _glp_mpl_execute_for
#define clean_for             _glp_mpl_clean_for
#define execute_statement     _glp_mpl_execute_statement
#define clean_statement       _glp_mpl_clean_statement

#define alloc_content         _glp_mpl_alloc_content
#define generate_model        _glp_mpl_generate_model
#define build_problem         _glp_mpl_build_problem
#define postsolve_model       _glp_mpl_postsolve_model
#define clean_model           _glp_mpl_clean_model
#define open_input            _glp_mpl_open_input
#define read_char             _glp_mpl_read_char
#define close_input           _glp_mpl_close_input
#define open_output           _glp_mpl_open_output
#define write_char            _glp_mpl_write_char
#define write_text            _glp_mpl_write_text
#define flush_output          _glp_mpl_flush_output
#define mpl_error             _glp_mpl_error
#define warning               _glp_mpl_warning
#define mpl_initialize        _glp_mpl_initialize
#define mpl_read_model        _glp_mpl_read_model
#define mpl_read_data         _glp_mpl_read_data
#define mpl_generate          _glp_mpl_generate
#define mpl_get_prob_name     _glp_mpl_get_prob_name
#define mpl_get_num_rows      _glp_mpl_get_num_rows
#define mpl_get_num_cols      _glp_mpl_get_num_cols
#define mpl_get_row_name      _glp_mpl_get_row_name
#define mpl_get_row_kind      _glp_mpl_get_row_kind
#define mpl_get_row_bnds      _glp_mpl_get_row_bnds
#define mpl_get_mat_row       _glp_mpl_get_mat_row
#define mpl_get_row_c0        _glp_mpl_get_row_c0
#define mpl_get_col_name      _glp_mpl_get_col_name
#define mpl_get_col_kind      _glp_mpl_get_col_kind
#define mpl_get_col_bnds      _glp_mpl_get_col_bnds
#define mpl_has_solve_stmt    _glp_mpl_has_solve_stmt
#define mpl_put_col_value     _glp_mpl_put_col_value
#define mpl_postsolve         _glp_mpl_postsolve
#define mpl_terminate         _glp_mpl_terminate

/**********************************************************************/
/* * *                  PROCESSING MODEL SECTION                  * * */
/**********************************************************************/

#define alloc(type) ((type *)dmp_get_atomv(mpl->pool, sizeof(type)))
/* allocate atom of given type */

#define enter_context _glp_mpl_enter_context
void enter_context(MPL *mpl);
/* enter current token into context queue */

#define print_context _glp_mpl_print_context
void print_context(MPL *mpl);
/* print current content of context queue */

#define get_char _glp_mpl_get_char
void get_char(MPL *mpl);
/* scan next character from input text file */

#define append_char _glp_mpl_append_char
void append_char(MPL *mpl);
/* append character to current token */

#define get_token _glp_mpl_get_token
void get_token(MPL *mpl);
/* scan next token from input text file */

#define unget_token _glp_mpl_unget_token
void unget_token(MPL *mpl);
/* return current token back to input stream */

#define is_keyword _glp_mpl_is_keyword
int is_keyword(MPL *mpl, char *keyword);
/* check if current token is given non-reserved keyword */

#define is_reserved _glp_mpl_is_reserved
int is_reserved(MPL *mpl);
/* check if current token is reserved keyword */

#define make_code _glp_mpl_make_code
CODE *make_code(MPL *mpl, int op, OPERANDS *arg, int type, int dim);
/* generate pseudo-code (basic routine) */

#define make_unary _glp_mpl_make_unary
CODE *make_unary(MPL *mpl, int op, CODE *x, int type, int dim);
/* generate pseudo-code for unary operation */

#define make_binary _glp_mpl_make_binary
CODE *make_binary(MPL *mpl, int op, CODE *x, CODE *y, int type,
      int dim);
/* generate pseudo-code for binary operation */

#define make_ternary _glp_mpl_make_ternary
CODE *make_ternary(MPL *mpl, int op, CODE *x, CODE *y, CODE *z,
      int type, int dim);
/* generate pseudo-code for ternary operation */

#define numeric_literal _glp_mpl_numeric_literal
CODE *numeric_literal(MPL *mpl);
/* parse reference to numeric literal */

#define string_literal _glp_mpl_string_literal
CODE *string_literal(MPL *mpl);
/* parse reference to string literal */

#define create_arg_list _glp_mpl_create_arg_list
ARG_LIST *create_arg_list(MPL *mpl);
/* create empty operands list */

#define expand_arg_list _glp_mpl_expand_arg_list
ARG_LIST *expand_arg_list(MPL *mpl, ARG_LIST *list, CODE *x);
/* append operand to operands list */

#define arg_list_len _glp_mpl_arg_list_len
int arg_list_len(MPL *mpl, ARG_LIST *list);
/* determine length of operands list */

#define subscript_list _glp_mpl_subscript_list
ARG_LIST *subscript_list(MPL *mpl);
/* parse subscript list */

#define object_reference _glp_mpl_object_reference
CODE *object_reference(MPL *mpl);
/* parse reference to named object */

#define numeric_argument _glp_mpl_numeric_argument
CODE *numeric_argument(MPL *mpl, char *func);
/* parse argument passed to built-in function */

#define symbolic_argument _glp_mpl_symbolic_argument
CODE *symbolic_argument(MPL *mpl, char *func);

#define elemset_argument _glp_mpl_elemset_argument
CODE *elemset_argument(MPL *mpl, char *func);

#define function_reference _glp_mpl_function_reference
CODE *function_reference(MPL *mpl);
/* parse reference to built-in function */

#define create_domain _glp_mpl_create_domain
DOMAIN *create_domain(MPL *mpl);
/* create empty domain */

#define create_block _glp_mpl_create_block
DOMAIN_BLOCK *create_block(MPL *mpl);
/* create empty domain block */

#define append_block _glp_mpl_append_block
void append_block(MPL *mpl, DOMAIN *domain, DOMAIN_BLOCK *block);
/* append domain block to specified domain */

#define append_slot _glp_mpl_append_slot
DOMAIN_SLOT *append_slot(MPL *mpl, DOMAIN_BLOCK *block, char *name,
      CODE *code);
/* create and append new slot to domain block */

#define expression_list _glp_mpl_expression_list
CODE *expression_list(MPL *mpl);
/* parse expression list */

#define literal_set _glp_mpl_literal_set
CODE *literal_set(MPL *mpl, CODE *code);
/* parse literal set */

#define indexing_expression _glp_mpl_indexing_expression
DOMAIN *indexing_expression(MPL *mpl);
/* parse indexing expression */

#define close_scope _glp_mpl_close_scope
void close_scope(MPL *mpl, DOMAIN *domain);
/* close scope of indexing expression */

#define iterated_expression _glp_mpl_iterated_expression
CODE *iterated_expression(MPL *mpl);
/* parse iterated expression */

#define domain_arity _glp_mpl_domain_arity
int domain_arity(MPL *mpl, DOMAIN *domain);
/* determine arity of domain */

#define set_expression _glp_mpl_set_expression
CODE *set_expression(MPL *mpl);
/* parse set expression */

#define branched_expression _glp_mpl_branched_expression
CODE *branched_expression(MPL *mpl);
/* parse conditional expression */

#define primary_expression _glp_mpl_primary_expression
CODE *primary_expression(MPL *mpl);
/* parse primary expression */

#define error_preceding _glp_mpl_error_preceding
void error_preceding(MPL *mpl, char *opstr);
/* raise error if preceding operand has wrong type */

#define error_following _glp_mpl_error_following
void error_following(MPL *mpl, char *opstr);
/* raise error if following operand has wrong type */

#define error_dimension _glp_mpl_error_dimension
void error_dimension(MPL *mpl, char *opstr, int dim1, int dim2);
/* raise error if operands have different dimension */

#define expression_0 _glp_mpl_expression_0
CODE *expression_0(MPL *mpl);
/* parse expression of level 0 */

#define expression_1 _glp_mpl_expression_1
CODE *expression_1(MPL *mpl);
/* parse expression of level 1 */

#define expression_2 _glp_mpl_expression_2
CODE *expression_2(MPL *mpl);
/* parse expression of level 2 */

#define expression_3 _glp_mpl_expression_3
CODE *expression_3(MPL *mpl);
/* parse expression of level 3 */

#define expression_4 _glp_mpl_expression_4
CODE *expression_4(MPL *mpl);
/* parse expression of level 4 */

#define expression_5 _glp_mpl_expression_5
CODE *expression_5(MPL *mpl);
/* parse expression of level 5 */

#define expression_6 _glp_mpl_expression_6
CODE *expression_6(MPL *mpl);
/* parse expression of level 6 */

#define expression_7 _glp_mpl_expression_7
CODE *expression_7(MPL *mpl);
/* parse expression of level 7 */

#define expression_8 _glp_mpl_expression_8
CODE *expression_8(MPL *mpl);
/* parse expression of level 8 */

#define expression_9 _glp_mpl_expression_9
CODE *expression_9(MPL *mpl);
/* parse expression of level 9 */

#define expression_10 _glp_mpl_expression_10
CODE *expression_10(MPL *mpl);
/* parse expression of level 10 */

#define expression_11 _glp_mpl_expression_11
CODE *expression_11(MPL *mpl);
/* parse expression of level 11 */

#define expression_12 _glp_mpl_expression_12
CODE *expression_12(MPL *mpl);
/* parse expression of level 12 */

#define expression_13 _glp_mpl_expression_13
CODE *expression_13(MPL *mpl);
/* parse expression of level 13 */

#define set_statement _glp_mpl_set_statement
SET *set_statement(MPL *mpl);
/* parse set statement */

#define parameter_statement _glp_mpl_parameter_statement
PARAMETER *parameter_statement(MPL *mpl);
/* parse parameter statement */

#define variable_statement _glp_mpl_variable_statement
VARIABLE *variable_statement(MPL *mpl);
/* parse variable statement */

#define constraint_statement _glp_mpl_constraint_statement
CONSTRAINT *constraint_statement(MPL *mpl);
/* parse constraint statement */

#define objective_statement _glp_mpl_objective_statement
CONSTRAINT *objective_statement(MPL *mpl);
/* parse objective statement */

#if 1 /* 11/II-2008 */
#define table_statement _glp_mpl_table_statement
TABLE *table_statement(MPL *mpl);
/* parse table statement */
#endif

#define solve_statement _glp_mpl_solve_statement
void *solve_statement(MPL *mpl);
/* parse solve statement */

#define check_statement _glp_mpl_check_statement
CHECK *check_statement(MPL *mpl);
/* parse check statement */

#define display_statement _glp_mpl_display_statement
DISPLAY *display_statement(MPL *mpl);
/* parse display statement */

#define printf_statement _glp_mpl_printf_statement
PRINTF *printf_statement(MPL *mpl);
/* parse printf statement */

#define for_statement _glp_mpl_for_statement
FOR *for_statement(MPL *mpl);
/* parse for statement */

#define end_statement _glp_mpl_end_statement
void end_statement(MPL *mpl);
/* parse end statement */

#define simple_statement _glp_mpl_simple_statement
STATEMENT *simple_statement(MPL *mpl, int spec);
/* parse simple statement */

#define model_section _glp_mpl_model_section
void model_section(MPL *mpl);
/* parse model section */

/**********************************************************************/
/* * *                  PROCESSING DATA SECTION                   * * */
/**********************************************************************/

#if 2 + 2 == 5
struct SLICE /* see TUPLE */
{     /* component of slice; the slice itself is associated with its
         first component; slices are similar to n-tuples with exception
         that some slice components (which are indicated by asterisks)
         don't refer to any symbols */
      SYMBOL *sym;
      /* symbol, which this component refers to; can be NULL */
      SLICE *next;
      /* the next component of slice */
};
#endif

SLICE *create_slice(MPL *mpl);
/* create slice */

SLICE *expand_slice
(     MPL *mpl,
      SLICE *slice,           /* destroyed */
      SYMBOL *sym             /* destroyed */
);
/* append new component to slice */

int slice_dimen
(     MPL *mpl,
      SLICE *slice            /* not changed */
);
/* determine dimension of slice */

int slice_arity
(     MPL *mpl,
      SLICE *slice            /* not changed */
);
/* determine arity of slice */

SLICE *fake_slice(MPL *mpl, int dim);
/* create fake slice of all asterisks */

void delete_slice
(     MPL *mpl,
      SLICE *slice            /* destroyed */
);
/* delete slice */

int is_number(MPL *mpl);
/* check if current token is number */

int is_symbol(MPL *mpl);
/* check if current token is symbol */

int is_literal(MPL *mpl, char *literal);
/* check if current token is given symbolic literal */

double read_number(MPL *mpl);
/* read number */

SYMBOL *read_symbol(MPL *mpl);
/* read symbol */

SLICE *read_slice
(     MPL *mpl,
      char *name,             /* not changed */
      int dim
);
/* read slice */

SET *select_set
(     MPL *mpl,
      char *name              /* not changed */
);
/* select set to saturate it with elemental sets */

void simple_format
(     MPL *mpl,
      SET *set,               /* not changed */
      MEMBER *memb,           /* modified */
      SLICE *slice            /* not changed */
);
/* read set data block in simple format */

void matrix_format
(     MPL *mpl,
      SET *set,               /* not changed */
      MEMBER *memb,           /* modified */
      SLICE *slice,           /* not changed */
      int tr
);
/* read set data block in matrix format */

void set_data(MPL *mpl);
/* read set data */

PARAMETER *select_parameter
(     MPL *mpl,
      char *name              /* not changed */
);
/* select parameter to saturate it with data */

void set_default
(     MPL *mpl,
      PARAMETER *par,         /* not changed */
      SYMBOL *altval          /* destroyed */
);
/* set default parameter value */

MEMBER *read_value
(     MPL *mpl,
      PARAMETER *par,         /* not changed */
      TUPLE *tuple            /* destroyed */
);
/* read value and assign it to parameter member */

void plain_format
(     MPL *mpl,
      PARAMETER *par,         /* not changed */
      SLICE *slice            /* not changed */
);
/* read parameter data block in plain format */

void tabular_format
(     MPL *mpl,
      PARAMETER *par,         /* not changed */
      SLICE *slice,           /* not changed */
      int tr
);
/* read parameter data block in tabular format */

void tabbing_format
(     MPL *mpl,
      SYMBOL *altval          /* not changed */
);
/* read parameter data block in tabbing format */

void parameter_data(MPL *mpl);
/* read parameter data */

void data_section(MPL *mpl);
/* read data section */

/**********************************************************************/
/* * *                   FLOATING-POINT NUMBERS                   * * */
/**********************************************************************/

double fp_add(MPL *mpl, double x, double y);
/* floating-point addition */

double fp_sub(MPL *mpl, double x, double y);
/* floating-point subtraction */

double fp_less(MPL *mpl, double x, double y);
/* floating-point non-negative subtraction */

double fp_mul(MPL *mpl, double x, double y);
/* floating-point multiplication */

double fp_div(MPL *mpl, double x, double y);
/* floating-point division */

double fp_idiv(MPL *mpl, double x, double y);
/* floating-point quotient of exact division */

double fp_mod(MPL *mpl, double x, double y);
/* floating-point remainder of exact division */

double fp_power(MPL *mpl, double x, double y);
/* floating-point exponentiation (raise to power) */

double fp_exp(MPL *mpl, double x);
/* floating-point base-e exponential */

double fp_log(MPL *mpl, double x);
/* floating-point natural logarithm */

double fp_log10(MPL *mpl, double x);
/* floating-point common (decimal) logarithm */

double fp_sqrt(MPL *mpl, double x);
/* floating-point square root */

double fp_sin(MPL *mpl, double x);
/* floating-point trigonometric sine */

double fp_cos(MPL *mpl, double x);
/* floating-point trigonometric cosine */

double fp_atan(MPL *mpl, double x);
/* floating-point trigonometric arctangent */

double fp_atan2(MPL *mpl, double y, double x);
/* floating-point trigonometric arctangent */

double fp_round(MPL *mpl, double x, double n);
/* round floating-point value to n fractional digits */

double fp_trunc(MPL *mpl, double x, double n);
/* truncate floating-point value to n fractional digits */

/**********************************************************************/
/* * *              PSEUDO-RANDOM NUMBER GENERATORS               * * */
/**********************************************************************/

double fp_irand224(MPL *mpl);
/* pseudo-random integer in the range [0, 2^24) */

double fp_uniform01(MPL *mpl);
/* pseudo-random number in the range [0, 1) */

double fp_uniform(MPL *mpl, double a, double b);
/* pseudo-random number in the range [a, b) */

double fp_normal01(MPL *mpl);
/* Gaussian random variate with mu = 0 and sigma = 1 */

double fp_normal(MPL *mpl, double mu, double sigma);
/* Gaussian random variate with specified mu and sigma */

#if 0
/**********************************************************************/
/* * *                SEGMENTED CHARACTER STRINGS                 * * */
/**********************************************************************/

#define STRSEG_SIZE 12
/* number of characters in one segment of the string */

struct STRING
{     /* segment of character string; the string itself is associated
         with its first segment */
      char seg[STRSEG_SIZE];
      /* up to STRSEG_SIZE characters; the end of string is indicated
         by '\0' as usual; thus, if this segment doesn't contain '\0',
         there must be a next segment */
      STRING *next;
      /* the next segment of string */
};
#else
/**********************************************************************/
/* * *                     CHARACTER STRINGS                      * * */
/**********************************************************************/
#endif

STRING *create_string
(     MPL *mpl,
      char buf[MAX_LENGTH+1]  /* not changed */
);
/* create character string */

STRING *copy_string
(     MPL *mpl,
      STRING *str             /* not changed */
);
/* make copy of character string */

int compare_strings
(     MPL *mpl,
      STRING *str1,           /* not changed */
      STRING *str2            /* not changed */
);
/* compare one character string with another */

char *fetch_string
(     MPL *mpl,
      STRING *str,            /* not changed */
      char buf[MAX_LENGTH+1]  /* modified */
);
/* extract content of character string */

void delete_string
(     MPL *mpl,
      STRING *str             /* destroyed */
);
/* delete character string */

/**********************************************************************/
/* * *                          SYMBOLS                           * * */
/**********************************************************************/

struct SYMBOL
{     /* symbol (numeric or abstract quantity) */
      double num;
      /* numeric value of symbol (used only if str == NULL) */
      STRING *str;
      /* abstract value of symbol (used only if str != NULL) */
};

SYMBOL *create_symbol_num(MPL *mpl, double num);
/* create symbol of numeric type */

SYMBOL *create_symbol_str
(     MPL *mpl,
      STRING *str             /* destroyed */
);
/* create symbol of abstract type */

SYMBOL *copy_symbol
(     MPL *mpl,
      SYMBOL *sym             /* not changed */
);
/* make copy of symbol */

int compare_symbols
(     MPL *mpl,
      SYMBOL *sym1,           /* not changed */
      SYMBOL *sym2            /* not changed */
);
/* compare one symbol with another */

void delete_symbol
(     MPL *mpl,
      SYMBOL *sym             /* destroyed */
);
/* delete symbol */

char *format_symbol
(     MPL *mpl,
      SYMBOL *sym             /* not changed */
);
/* format symbol for displaying or printing */

SYMBOL *concat_symbols
(     MPL *mpl,
      SYMBOL *sym1,           /* destroyed */
      SYMBOL *sym2            /* destroyed */
);
/* concatenate one symbol with another */

/**********************************************************************/
/* * *                          N-TUPLES                          * * */
/**********************************************************************/

struct TUPLE
{     /* component of n-tuple; the n-tuple itself is associated with
         its first component; (note that 0-tuple has no components) */
      SYMBOL *sym;
      /* symbol, which the component refers to; cannot be NULL */
      TUPLE *next;
      /* the next component of n-tuple */
};

TUPLE *create_tuple(MPL *mpl);
/* create n-tuple */

TUPLE *expand_tuple
(     MPL *mpl,
      TUPLE *tuple,           /* destroyed */
      SYMBOL *sym             /* destroyed */
);
/* append symbol to n-tuple */

int tuple_dimen
(     MPL *mpl,
      TUPLE *tuple            /* not changed */
);
/* determine dimension of n-tuple */

TUPLE *copy_tuple
(     MPL *mpl,
      TUPLE *tuple            /* not changed */
);
/* make copy of n-tuple */

int compare_tuples
(     MPL *mpl,
      TUPLE *tuple1,          /* not changed */
      TUPLE *tuple2           /* not changed */
);
/* compare one n-tuple with another */

TUPLE *build_subtuple
(     MPL *mpl,
      TUPLE *tuple,           /* not changed */
      int dim
);
/* build subtuple of given n-tuple */

void delete_tuple
(     MPL *mpl,
      TUPLE *tuple            /* destroyed */
);
/* delete n-tuple */

char *format_tuple
(     MPL *mpl,
      int c,
      TUPLE *tuple            /* not changed */
);
/* format n-tuple for displaying or printing */

/**********************************************************************/
/* * *                       ELEMENTAL SETS                       * * */
/**********************************************************************/

#if 2 + 2 == 5
struct ELEMSET /* see ARRAY */
{     /* elemental set of n-tuples; formally it is a "value" assigned
         to members of model sets (like numbers and symbols, which are
         values assigned to members of model parameters); note that a
         simple model set is not an elemental set, it is 0-dimensional
         array, the only member of which (if it exists) is assigned an
         elemental set */
#endif

ELEMSET *create_elemset(MPL *mpl, int dim);
/* create elemental set */

MEMBER *find_tuple
(     MPL *mpl,
      ELEMSET *set,           /* not changed */
      TUPLE *tuple            /* not changed */
);
/* check if elemental set contains given n-tuple */

MEMBER *add_tuple
(     MPL *mpl,
      ELEMSET *set,           /* modified */
      TUPLE *tuple            /* destroyed */
);
/* add new n-tuple to elemental set */

MEMBER *check_then_add
(     MPL *mpl,
      ELEMSET *set,           /* modified */
      TUPLE *tuple            /* destroyed */
);
/* check and add new n-tuple to elemental set */

ELEMSET *copy_elemset
(     MPL *mpl,
      ELEMSET *set            /* not changed */
);
/* make copy of elemental set */

void delete_elemset
(     MPL *mpl,
      ELEMSET *set            /* destroyed */
);
/* delete elemental set */

int arelset_size(MPL *mpl, double t0, double tf, double dt);
/* compute size of "arithmetic" elemental set */

double arelset_member(MPL *mpl, double t0, double tf, double dt, int j);
/* compute member of "arithmetic" elemental set */

ELEMSET *create_arelset(MPL *mpl, double t0, double tf, double dt);
/* create "arithmetic" elemental set */

ELEMSET *set_union
(     MPL *mpl,
      ELEMSET *X,             /* destroyed */
      ELEMSET *Y              /* destroyed */
);
/* union of two elemental sets */

ELEMSET *set_diff
(     MPL *mpl,
      ELEMSET *X,             /* destroyed */
      ELEMSET *Y              /* destroyed */
);
/* difference between two elemental sets */

ELEMSET *set_symdiff
(     MPL *mpl,
      ELEMSET *X,             /* destroyed */
      ELEMSET *Y              /* destroyed */
);
/* symmetric difference between two elemental sets */

ELEMSET *set_inter
(     MPL *mpl,
      ELEMSET *X,             /* destroyed */
      ELEMSET *Y              /* destroyed */
);
/* intersection of two elemental sets */

ELEMSET *set_cross
(     MPL *mpl,
      ELEMSET *X,             /* destroyed */
      ELEMSET *Y              /* destroyed */
);
/* cross (Cartesian) product of two elemental sets */

/**********************************************************************/
/* * *                    ELEMENTAL VARIABLES                     * * */
/**********************************************************************/

struct ELEMVAR
{     /* elemental variable; formally it is a "value" assigned to
         members of model variables (like numbers and symbols, which
         are values assigned to members of model parameters) */
      int j;
      /* LP column number assigned to this elemental variable */
      VARIABLE *var;
      /* model variable, which contains this elemental variable */
      MEMBER *memb;
      /* array member, which is assigned this elemental variable */
      double lbnd;
      /* lower bound */
      double ubnd;
      /* upper bound */
      double temp;
      /* working quantity used in operations on linear forms; normally
         it contains floating-point zero */
      double value;
      /* value of this elemental variable provided by the solver */
};

/**********************************************************************/
/* * *                        LINEAR FORMS                        * * */
/**********************************************************************/

struct FORMULA
{     /* term of linear form c * x, where c is a coefficient, x is an
         elemental variable; the linear form itself is the sum of terms
         and is associated with its first term; (note that the linear
         form may be empty that means the sum is equal to zero) */
      double coef;
      /* coefficient at elemental variable or constant term */
      ELEMVAR *var;
      /* reference to elemental variable; NULL means constant term */
      FORMULA *next;
      /* the next term of linear form */
};

FORMULA *constant_term(MPL *mpl, double coef);
/* create constant term */

FORMULA *single_variable
(     MPL *mpl,
      ELEMVAR *var            /* referenced */
);
/* create single variable */

FORMULA *copy_formula
(     MPL *mpl,
      FORMULA *form           /* not changed */
);
/* make copy of linear form */

void delete_formula
(     MPL *mpl,
      FORMULA *form           /* destroyed */
);
/* delete linear form */

FORMULA *linear_comb
(     MPL *mpl,
      double a, FORMULA *fx,  /* destroyed */
      double b, FORMULA *fy   /* destroyed */
);
/* linear combination of two linear forms */

FORMULA *remove_constant
(     MPL *mpl,
      FORMULA *form,          /* destroyed */
      double *coef            /* modified */
);
/* remove constant term from linear form */

FORMULA *reduce_terms
(     MPL *mpl,
      FORMULA *form           /* destroyed */
);
/* reduce identical terms in linear form */

/**********************************************************************/
/* * *                   ELEMENTAL CONSTRAINTS                    * * */
/**********************************************************************/

struct ELEMCON
{     /* elemental constraint; formally it is a "value" assigned to
         members of model constraints (like numbers or symbols, which
         are values assigned to members of model parameters) */
      int i;
      /* LP row number assigned to this elemental constraint */
      CONSTRAINT *con;
      /* model constraint, which contains this elemental constraint */
      MEMBER *memb;
      /* array member, which is assigned this elemental constraint */
      FORMULA *form;
      /* linear form */
      double lbnd;
      /* lower bound */
      double ubnd;
      /* upper bound */
};

/**********************************************************************/
/* * *                       GENERIC VALUES                       * * */
/**********************************************************************/

union VALUE
{     /* generic value, which can be assigned to object member or be a
         result of evaluation of expression */
      /* indicator that specifies the particular type of generic value
         is stored in the corresponding array or pseudo-code descriptor
         and can be one of the following:
         A_NONE     - no value
         A_NUMERIC  - floating-point number
         A_SYMBOLIC - symbol
         A_LOGICAL  - logical value
         A_TUPLE    - n-tuple
         A_ELEMSET  - elemental set
         A_ELEMVAR  - elemental variable
         A_FORMULA  - linear form
         A_ELEMCON  - elemental constraint */
      void *none;    /* null */
      double num;    /* value */
      SYMBOL *sym;   /* value */
      int bit;       /* value */
      TUPLE *tuple;  /* value */
      ELEMSET *set;  /* value */
      ELEMVAR *var;  /* reference */
      FORMULA *form; /* value */
      ELEMCON *con;  /* reference */
};

void delete_value
(     MPL *mpl,
      int type,
      VALUE *value            /* content destroyed */
);
/* delete generic value */

/**********************************************************************/
/* * *                SYMBOLICALLY INDEXED ARRAYS                 * * */
/**********************************************************************/

struct ARRAY
{     /* multi-dimensional array, a set of members indexed over simple
         or compound sets of symbols; arrays are used to represent the
         contents of model objects (i.e. sets, parameters, variables,
         constraints, and objectives); arrays also are used as "values"
         that are assigned to members of set objects, in which case the
         array itself represents an elemental set */
      int type;
      /* type of generic values assigned to the array members:
         A_NONE     - none (members have no assigned values)
         A_NUMERIC  - floating-point numbers
         A_SYMBOLIC - symbols
         A_ELEMSET  - elemental sets
         A_ELEMVAR  - elemental variables
         A_ELEMCON  - elemental constraints */
      int dim;
      /* dimension of the array that determines number of components in
         n-tuples for all members of the array, dim >= 0; dim = 0 means
         the array is 0-dimensional */
      int size;
      /* size of the array, i.e. number of its members */
      MEMBER *head;
      /* the first array member; NULL means the array is empty */
      MEMBER *tail;
      /* the last array member; NULL means the array is empty */
      AVL *tree;
      /* the search tree intended to find array members for logarithmic
         time; NULL means the search tree doesn't exist */
      ARRAY *prev;
      /* the previous array in the translator database */
      ARRAY *next;
      /* the next array in the translator database */
};

struct MEMBER
{     /* array member */
      TUPLE *tuple;
      /* n-tuple, which identifies the member; number of its components
         is the same for all members within the array and determined by
         the array dimension; duplicate members are not allowed */
      MEMBER *next;
      /* the next array member */
      VALUE value;
      /* generic value assigned to the member */
};

ARRAY *create_array(MPL *mpl, int type, int dim);
/* create array */

MEMBER *find_member
(     MPL *mpl,
      ARRAY *array,           /* not changed */
      TUPLE *tuple            /* not changed */
);
/* find array member with given n-tuple */

MEMBER *add_member
(     MPL *mpl,
      ARRAY *array,           /* modified */
      TUPLE *tuple            /* destroyed */
);
/* add new member to array */

void delete_array
(     MPL *mpl,
      ARRAY *array            /* destroyed */
);
/* delete array */

/**********************************************************************/
/* * *                 DOMAINS AND DUMMY INDICES                  * * */
/**********************************************************************/

struct DOMAIN
{     /* domain (a simple or compound set); syntactically domain looks
         like '{ i in I, (j,k) in S, t in T : <predicate> }'; domains
         are used to define sets, over which model objects are indexed,
         and also as constituents of iterated operators */
      DOMAIN_BLOCK *list;
      /* linked list of domain blocks (in the example above such blocks
         are 'i in I', '(j,k) in S', and 't in T'); this list cannot be
         empty */
      CODE *code;
      /* pseudo-code for computing the logical predicate, which follows
         the colon; NULL means no predicate is specified */
};

struct DOMAIN_BLOCK
{     /* domain block; syntactically domain blocks look like 'i in I',
         '(j,k) in S', and 't in T' in the example above (in the sequel
         sets like I, S, and T are called basic sets) */
      DOMAIN_SLOT *list;
      /* linked list of domain slots (i.e. indexing positions); number
         of slots in this list is the same as dimension of n-tuples in
         the basic set; this list cannot be empty */
      CODE *code;
      /* pseudo-code for computing basic set; cannot be NULL */
      TUPLE *backup;
      /* if this n-tuple is not empty, current values of dummy indices
         in the domain block are the same as components of this n-tuple
         (note that this n-tuple may have larger dimension than number
         of dummy indices in this block, in which case extra components
         are ignored); this n-tuple is used to restore former values of
         dummy indices, if they were changed due to recursive calls to
         the domain block */
      DOMAIN_BLOCK *next;
      /* the next block in the same domain */
};

struct DOMAIN_SLOT
{     /* domain slot; it specifies an individual indexing position and
         defines the corresponding dummy index */
      char *name;
      /* symbolic name of the dummy index; null pointer means the dummy
         index is not explicitly specified */
      CODE *code;
      /* pseudo-code for computing symbolic value, at which the dummy
         index is bound; NULL means the dummy index is free within the
         domain scope */
      SYMBOL *value;
      /* current value assigned to the dummy index; NULL means no value
         is assigned at the moment */
      CODE *list;
      /* linked list of pseudo-codes with operation O_INDEX referring
         to this slot; this linked list is used to invalidate resultant
         values of the operation, which depend on this dummy index */
      DOMAIN_SLOT *next;
      /* the next slot in the same domain block */
};

void assign_dummy_index
(     MPL *mpl,
      DOMAIN_SLOT *slot,      /* modified */
      SYMBOL *value           /* not changed */
);
/* assign new value to dummy index */

void update_dummy_indices
(     MPL *mpl,
      DOMAIN_BLOCK *block     /* not changed */
);
/* update current values of dummy indices */

int enter_domain_block
(     MPL *mpl,
      DOMAIN_BLOCK *block,    /* not changed */
      TUPLE *tuple,           /* not changed */
      void *info, void (*func)(MPL *mpl, void *info)
);
/* enter domain block */

int eval_within_domain
(     MPL *mpl,
      DOMAIN *domain,         /* not changed */
      TUPLE *tuple,           /* not changed */
      void *info, void (*func)(MPL *mpl, void *info)
);
/* perform evaluation within domain scope */

void loop_within_domain
(     MPL *mpl,
      DOMAIN *domain,         /* not changed */
      void *info, int (*func)(MPL *mpl, void *info)
);
/* perform iterations within domain scope */

void out_of_domain
(     MPL *mpl,
      char *name,             /* not changed */
      TUPLE *tuple            /* not changed */
);
/* raise domain exception */

TUPLE *get_domain_tuple
(     MPL *mpl,
      DOMAIN *domain          /* not changed */
);
/* obtain current n-tuple from domain */

void clean_domain(MPL *mpl, DOMAIN *domain);
/* clean domain */

/**********************************************************************/
/* * *                         MODEL SETS                         * * */
/**********************************************************************/

struct SET
{     /* model set */
      char *name;
      /* symbolic name; cannot be NULL */
      char *alias;
      /* alias; NULL means alias is not specified */
      int dim; /* aka arity */
      /* dimension (number of subscripts); dim = 0 means 0-dimensional
         (unsubscripted) set, dim > 0 means set of sets */
      DOMAIN *domain;
      /* subscript domain; NULL for 0-dimensional set */
      int dimen;
      /* dimension of n-tuples, which members of this set consist of
         (note that the model set itself is an array of elemental sets,
         which are its members; so, don't confuse this dimension with
         dimension of the model set); always non-zero */
      WITHIN *within;
      /* list of supersets, which restrict each member of the set to be
         in every superset from this list; this list can be empty */
      CODE *assign;
      /* pseudo-code for computing assigned value; can be NULL */
      CODE *option;
      /* pseudo-code for computing default value; can be NULL */
      int data;
      /* data status flag:
         0 - no data are provided in the data section
         1 - data are provided, but not checked yet
         2 - data are provided and have been checked */
      ARRAY *array;
      /* array of members, which are assigned elemental sets */
};

struct WITHIN
{     /* restricting superset list entry */
      CODE *code;
      /* pseudo-code for computing the superset; cannot be NULL */
      WITHIN *next;
      /* the next entry for the same set or parameter */
};

void check_elem_set
(     MPL *mpl,
      SET *set,               /* not changed */
      TUPLE *tuple,           /* not changed */
      ELEMSET *refer          /* not changed */
);
/* check elemental set assigned to set member */

ELEMSET *take_member_set      /* returns reference, not value */
(     MPL *mpl,
      SET *set,               /* not changed */
      TUPLE *tuple            /* not changed */
);
/* obtain elemental set assigned to set member */

ELEMSET *eval_member_set      /* returns reference, not value */
(     MPL *mpl,
      SET *set,               /* not changed */
      TUPLE *tuple            /* not changed */
);
/* evaluate elemental set assigned to set member */

void eval_whole_set(MPL *mpl, SET *set);
/* evaluate model set over entire domain */

void clean_set(MPL *mpl, SET *set);
/* clean model set */

/**********************************************************************/
/* * *                      MODEL PARAMETERS                      * * */
/**********************************************************************/

struct PARAMETER
{     /* model parameter */
      char *name;
      /* symbolic name; cannot be NULL */
      char *alias;
      /* alias; NULL means alias is not specified */
      int dim; /* aka arity */
      /* dimension (number of subscripts); dim = 0 means 0-dimensional
         (unsubscripted) parameter */
      DOMAIN *domain;
      /* subscript domain; NULL for 0-dimensional parameter */
      int type;
      /* parameter type:
         A_NUMERIC  - numeric
         A_INTEGER  - integer
         A_BINARY   - binary
         A_SYMBOLIC - symbolic */
      CONDITION *cond;
      /* list of conditions, which restrict each parameter member to
         satisfy to every condition from this list; this list is used
         only for numeric parameters and can be empty */
      WITHIN *in;
      /* list of supersets, which restrict each parameter member to be
         in every superset from this list; this list is used only for
         symbolic parameters and can be empty */
      CODE *assign;
      /* pseudo-code for computing assigned value; can be NULL */
      CODE *option;
      /* pseudo-code for computing default value; can be NULL */
      int data;
      /* data status flag:
         0 - no data are provided in the data section
         1 - data are provided, but not checked yet
         2 - data are provided and have been checked */
      SYMBOL *defval;
      /* default value provided in the data section; can be NULL */
      ARRAY *array;
      /* array of members, which are assigned numbers or symbols */
};

struct CONDITION
{     /* restricting condition list entry */
      int rho;
      /* flag that specifies the form of the condition:
         O_LT - less than
         O_LE - less than or equal to
         O_EQ - equal to
         O_GE - greater than or equal to
         O_GT - greater than
         O_NE - not equal to */
      CODE *code;
      /* pseudo-code for computing the reference value */
      CONDITION *next;
      /* the next entry for the same parameter */
};

void check_value_num
(     MPL *mpl,
      PARAMETER *par,         /* not changed */
      TUPLE *tuple,           /* not changed */
      double value
);
/* check numeric value assigned to parameter member */

double take_member_num
(     MPL *mpl,
      PARAMETER *par,         /* not changed */
      TUPLE *tuple            /* not changed */
);
/* obtain numeric value assigned to parameter member */

double eval_member_num
(     MPL *mpl,
      PARAMETER *par,         /* not changed */
      TUPLE *tuple            /* not changed */
);
/* evaluate numeric value assigned to parameter member */

void check_value_sym
(     MPL *mpl,
      PARAMETER *par,         /* not changed */
      TUPLE *tuple,           /* not changed */
      SYMBOL *value           /* not changed */
);
/* check symbolic value assigned to parameter member */

SYMBOL *take_member_sym       /* returns value, not reference */
(     MPL *mpl,
      PARAMETER *par,         /* not changed */
      TUPLE *tuple            /* not changed */
);
/* obtain symbolic value assigned to parameter member */

SYMBOL *eval_member_sym       /* returns value, not reference */
(     MPL *mpl,
      PARAMETER *par,         /* not changed */
      TUPLE *tuple            /* not changed */
);
/* evaluate symbolic value assigned to parameter member */

void eval_whole_par(MPL *mpl, PARAMETER *par);
/* evaluate model parameter over entire domain */

void clean_parameter(MPL *mpl, PARAMETER *par);
/* clean model parameter */

/**********************************************************************/
/* * *                      MODEL VARIABLES                       * * */
/**********************************************************************/

struct VARIABLE
{     /* model variable */
      char *name;
      /* symbolic name; cannot be NULL */
      char *alias;
      /* alias; NULL means alias is not specified */
      int dim; /* aka arity */
      /* dimension (number of subscripts); dim = 0 means 0-dimensional
         (unsubscripted) variable */
      DOMAIN *domain;
      /* subscript domain; NULL for 0-dimensional variable */
      int type;
      /* variable type:
         A_NUMERIC - continuous
         A_INTEGER - integer
         A_BINARY  - binary */
      CODE *lbnd;
      /* pseudo-code for computing lower bound; NULL means lower bound
         is not specified */
      CODE *ubnd;
      /* pseudo-code for computing upper bound; NULL means upper bound
         is not specified */
      /* if both the pointers lbnd and ubnd refer to the same code, the
         variable is fixed at the corresponding value */
      ARRAY *array;
      /* array of members, which are assigned elemental variables */
};

ELEMVAR *take_member_var      /* returns reference */
(     MPL *mpl,
      VARIABLE *var,          /* not changed */
      TUPLE *tuple            /* not changed */
);
/* obtain reference to elemental variable */

ELEMVAR *eval_member_var      /* returns reference */
(     MPL *mpl,
      VARIABLE *var,          /* not changed */
      TUPLE *tuple            /* not changed */
);
/* evaluate reference to elemental variable */

void eval_whole_var(MPL *mpl, VARIABLE *var);
/* evaluate model variable over entire domain */

void clean_variable(MPL *mpl, VARIABLE *var);
/* clean model variable */

/**********************************************************************/
/* * *              MODEL CONSTRAINTS AND OBJECTIVES              * * */
/**********************************************************************/

struct CONSTRAINT
{     /* model constraint or objective */
      char *name;
      /* symbolic name; cannot be NULL */
      char *alias;
      /* alias; NULL means alias is not specified */
      int dim; /* aka arity */
      /* dimension (number of subscripts); dim = 0 means 0-dimensional
         (unsubscripted) constraint */
      DOMAIN *domain;
      /* subscript domain; NULL for 0-dimensional constraint */
      int type;
      /* constraint type:
         A_CONSTRAINT - constraint
         A_MINIMIZE   - objective (minimization)
         A_MAXIMIZE   - objective (maximization) */
      CODE *code;
      /* pseudo-code for computing main linear form; cannot be NULL */
      CODE *lbnd;
      /* pseudo-code for computing lower bound; NULL means lower bound
         is not specified */
      CODE *ubnd;
      /* pseudo-code for computing upper bound; NULL means upper bound
         is not specified */
      /* if both the pointers lbnd and ubnd refer to the same code, the
         constraint has the form of equation */
      ARRAY *array;
      /* array of members, which are assigned elemental constraints */
};

ELEMCON *take_member_con      /* returns reference */
(     MPL *mpl,
      CONSTRAINT *con,        /* not changed */
      TUPLE *tuple            /* not changed */
);
/* obtain reference to elemental constraint */

ELEMCON *eval_member_con      /* returns reference */
(     MPL *mpl,
      CONSTRAINT *con,        /* not changed */
      TUPLE *tuple            /* not changed */
);
/* evaluate reference to elemental constraint */

void eval_whole_con(MPL *mpl, CONSTRAINT *con);
/* evaluate model constraint over entire domain */

void clean_constraint(MPL *mpl, CONSTRAINT *con);
/* clean model constraint */

#if 1 /* 11/II-2008 */
/**********************************************************************/
/* * *                        DATA TABLES                         * * */
/**********************************************************************/

struct TABLE
{     /* data table */
      char *name;
      /* symbolic name; cannot be NULL */
      char *alias;
      /* alias; NULL means alias is not specified */
      int type;
      /* table type:
         A_INPUT  - input table
         A_OUTPUT - output table */
      TABARG *arg;
      /* argument list; cannot be empty */
      union
      {  struct
         {  SET *set;
            /* input set; NULL means the set is not specified */
            TABFLD *fld;
            /* field list; cannot be empty */
            TABIN *list;
            /* input list; can be empty */
         } in;
         struct
         {  DOMAIN *domain;
            /* subscript domain; cannot be NULL */
            TABOUT *list;
            /* output list; cannot be empty */
         } out;
      } u;
};

struct TABARG
{     /* table argument list entry */
      CODE *code;
      /* pseudo-code for computing the argument */
      TABARG *next;
      /* next entry for the same table */
};

struct TABFLD
{     /* table field list entry */
      char *name;
      /* field name; cannot be NULL */
      TABFLD *next;
      /* next entry for the same table */
};

struct TABIN
{     /* table input list entry */
      PARAMETER *par;
      /* parameter to be read; cannot be NULL */
      char *name;
      /* column name; cannot be NULL */
      TABIN *next;
      /* next entry for the same table */
};

struct TABOUT
{     /* table output list entry */
      CODE *code;
      /* pseudo-code for computing the value to be written */
      char *name;
      /* column name; cannot be NULL */
      TABOUT *next;
      /* next entry for the same table */
};

struct TABDCA
{     /* table driver communication area */
      int id;
      /* driver identifier (set by mpl_tab_drv_open) */
      void *link;
      /* driver link pointer (set by mpl_tab_drv_open) */
      int na;
      /* number of arguments */
      char **arg; /* char *arg[1+ns]; */
      /* arg[k], 1 <= k <= ns, is pointer to k-th argument */
      int nf;
      /* number of fields */
      char **name; /* char *name[1+nc]; */
      /* name[k], 1 <= k <= nc, is name of k-th field */
      int *type; /* int type[1+nc]; */
      /* type[k], 1 <= k <= nc, is type of k-th field:
         '?' - value not assigned
         'N' - number
         'S' - character string */
      double *num; /* double num[1+nc]; */
      /* num[k], 1 <= k <= nc, is numeric value of k-th field */
      char **str;
      /* str[k], 1 <= k <= nc, is string value of k-th field */
};

#define mpl_tab_num_args _glp_mpl_tab_num_args
int mpl_tab_num_args(TABDCA *dca);

#define mpl_tab_get_arg _glp_mpl_tab_get_arg
const char *mpl_tab_get_arg(TABDCA *dca, int k);

#define mpl_tab_num_flds _glp_mpl_tab_num_flds
int mpl_tab_num_flds(TABDCA *dca);

#define mpl_tab_get_name _glp_mpl_tab_get_name
const char *mpl_tab_get_name(TABDCA *dca, int k);

#define mpl_tab_get_type _glp_mpl_tab_get_type
int mpl_tab_get_type(TABDCA *dca, int k);

#define mpl_tab_get_num _glp_mpl_tab_get_num
double mpl_tab_get_num(TABDCA *dca, int k);

#define mpl_tab_get_str _glp_mpl_tab_get_str
const char *mpl_tab_get_str(TABDCA *dca, int k);

#define mpl_tab_set_num _glp_mpl_tab_set_num
void mpl_tab_set_num(TABDCA *dca, int k, double num);

#define mpl_tab_set_str _glp_mpl_tab_set_str
void mpl_tab_set_str(TABDCA *dca, int k, const char *str);

#define mpl_tab_drv_open _glp_mpl_tab_drv_open
void mpl_tab_drv_open(MPL *mpl, int mode);

#define mpl_tab_drv_read _glp_mpl_tab_drv_read
int mpl_tab_drv_read(MPL *mpl);

#define mpl_tab_drv_write _glp_mpl_tab_drv_write
void mpl_tab_drv_write(MPL *mpl);

#define mpl_tab_drv_close _glp_mpl_tab_drv_close
void mpl_tab_drv_close(MPL *mpl);
#endif

/**********************************************************************/
/* * *                        PSEUDO-CODE                         * * */
/**********************************************************************/

union OPERANDS
{     /* operands that participate in pseudo-code operation (choice of
         particular operands depends on the operation code) */
      /*--------------------------------------------------------------*/
      double num;             /* O_NUMBER */
      /* floaing-point number to be taken */
      /*--------------------------------------------------------------*/
      char *str;              /* O_STRING */
      /* character string to be taken */
      /*--------------------------------------------------------------*/
      struct                  /* O_INDEX */
      {  DOMAIN_SLOT *slot;
         /* domain slot, which contains dummy index to be taken */
         CODE *next;
         /* the next pseudo-code with op = O_INDEX, which refers to the
            same slot as this one; pointer to the beginning of this list
            is stored in the corresponding domain slot */
      } index;
      /*--------------------------------------------------------------*/
      struct                  /* O_MEMNUM, O_MEMSYM */
      {  PARAMETER *par;
         /* model parameter, which contains member to be taken */
         ARG_LIST *list;
         /* list of subscripts; NULL for 0-dimensional parameter */
      } par;
      /*--------------------------------------------------------------*/
      struct                  /* O_MEMSET */
      {  SET *set;
         /* model set, which contains member to be taken */
         ARG_LIST *list;
         /* list of subscripts; NULL for 0-dimensional set */
      } set;
      /*--------------------------------------------------------------*/
      struct                  /* O_MEMVAR */
      {  VARIABLE *var;
         /* model variable, which contains member to be taken */
         ARG_LIST *list;
         /* list of subscripts; NULL for 0-dimensional variable */
      } var;
      /*--------------------------------------------------------------*/
      ARG_LIST *list;         /* O_TUPLE, O_MAKE, n-ary operations */
      /* list of operands */
      /*--------------------------------------------------------------*/
      DOMAIN_BLOCK *slice;    /* O_SLICE */
      /* domain block, which specifies slice (i.e. n-tuple that contains
         free dummy indices); this operation is never evaluated */
      /*--------------------------------------------------------------*/
      struct                  /* unary, binary, ternary operations */
      {  CODE *x;
         /* pseudo-code for computing first operand */
         CODE *y;
         /* pseudo-code for computing second operand */
         CODE *z;
         /* pseudo-code for computing third operand */
      } arg;
      /*--------------------------------------------------------------*/
      struct                  /* iterated operations */
      {  DOMAIN *domain;
         /* domain, over which the operation is performed */
         CODE *x;
         /* pseudo-code for computing "integrand" */
      } loop;
      /*--------------------------------------------------------------*/
};

struct ARG_LIST
{     /* operands list entry */
      CODE *x;
      /* pseudo-code for computing operand */
      ARG_LIST *next;
      /* the next operand of the same operation */
};

struct CODE
{     /* pseudo-code (internal form of expressions) */
      int op;
      /* operation code: */
#define O_NUMBER        301   /* take floating-point number */
#define O_STRING        302   /* take character string */
#define O_INDEX         303   /* take dummy index */
#define O_MEMNUM        304   /* take member of numeric parameter */
#define O_MEMSYM        305   /* take member of symbolic parameter */
#define O_MEMSET        306   /* take member of set */
#define O_MEMVAR        307   /* take member of variable */
#define O_TUPLE         308   /* make n-tuple */
#define O_MAKE          309   /* make elemental set of n-tuples */
#define O_SLICE         310   /* define domain block (dummy op) */
                              /* 0-ary operations --------------------*/
#define O_IRAND224      311   /* pseudo-random in [0, 2^24-1] */
#define O_UNIFORM01     312   /* pseudo-random in [0, 1) */
#define O_NORMAL01      313   /* gaussian random, mu = 0, sigma = 1 */
                              /* unary operations --------------------*/
#define O_CVTNUM        314   /* conversion to numeric */
#define O_CVTSYM        315   /* conversion to symbolic */
#define O_CVTLOG        316   /* conversion to logical */
#define O_CVTTUP        317   /* conversion to 1-tuple */
#define O_CVTLFM        318   /* conversion to linear form */
#define O_PLUS          319   /* unary plus */
#define O_MINUS         320   /* unary minus */
#define O_NOT           321   /* negation (logical "not") */
#define O_ABS           322   /* absolute value */
#define O_CEIL          323   /* round upward ("ceiling of x") */
#define O_FLOOR         324   /* round downward ("floor of x") */
#define O_EXP           325   /* base-e exponential */
#define O_LOG           326   /* natural logarithm */
#define O_LOG10         327   /* common (decimal) logarithm */
#define O_SQRT          328   /* square root */
#define O_SIN           329   /* trigonometric sine */
#define O_COS           330   /* trigonometric cosine */
#define O_ATAN          331   /* trigonometric arctangent */
#define O_ROUND         332   /* round to nearest integer */
#define O_TRUNC         333   /* truncate to nearest integer */
#if 1 /* 15/VII-2006 */
#define O_CARD          334   /* cardinality of set */
#define O_LENGTH        335   /* length of symbolic value */
#endif
                              /* binary operations -------------------*/
#define O_ADD           336   /* addition */
#define O_SUB           337   /* subtraction */
#define O_LESS          338   /* non-negative subtraction */
#define O_MUL           339   /* multiplication */
#define O_DIV           340   /* division */
#define O_IDIV          341   /* quotient of exact division */
#define O_MOD           342   /* remainder of exact division */
#define O_POWER         343   /* exponentiation (raise to power) */
#define O_ATAN2         344   /* trigonometric arctangent */
#define O_ROUND2        345   /* round to n fractional digits */
#define O_TRUNC2        346   /* truncate to n fractional digits */
#define O_UNIFORM       347   /* pseudo-random in [a, b) */
#define O_NORMAL        348   /* gaussian random, given mu and sigma */
#define O_CONCAT        349   /* concatenation */
#define O_LT            350   /* comparison on 'less than' */
#define O_LE            351   /* comparison on 'not greater than' */
#define O_EQ            352   /* comparison on 'equal to' */
#define O_GE            353   /* comparison on 'not less than' */
#define O_GT            354   /* comparison on 'greater than' */
#define O_NE            355   /* comparison on 'not equal to' */
#define O_AND           356   /* conjunction (logical "and") */
#define O_OR            357   /* disjunction (logical "or") */
#define O_UNION         358   /* union */
#define O_DIFF          359   /* difference */
#define O_SYMDIFF       360   /* symmetric difference */
#define O_INTER         361   /* intersection */
#define O_CROSS         362   /* cross (Cartesian) product */
#define O_IN            363   /* test on 'x in Y' */
#define O_NOTIN         364   /* test on 'x not in Y' */
#define O_WITHIN        365   /* test on 'X within Y' */
#define O_NOTWITHIN     366   /* test on 'X not within Y' */
#if 1 /* 15/VII-2006 */
#define O_SUBSTR        367   /* substring */
#endif
                              /* ternary operations ------------------*/
#define O_DOTS          368   /* build "arithmetic" set */
#define O_FORK          369   /* if-then-else */
#if 1 /* 15/VII-2006 */
#define O_SUBSTR3       370   /* substring */
#endif
                              /* n-ary operations --------------------*/
#define O_MIN           371   /* minimal value (n-ary) */
#define O_MAX           372   /* maximal value (n-ary) */
                              /* iterated operations -----------------*/
#define O_SUM           373   /* summation */
#define O_PROD          374   /* multiplication */
#define O_MINIMUM       375   /* minimum */
#define O_MAXIMUM       376   /* maximum */
#define O_FORALL        377   /* conjunction (A-quantification) */
#define O_EXISTS        378   /* disjunction (E-quantification) */
#define O_SETOF         379   /* compute elemental set */
#define O_BUILD         380   /* build elemental set */
      OPERANDS arg;
      /* operands that participate in the operation */
      int type;
      /* type of the resultant value:
         A_NUMERIC  - numeric
         A_SYMBOLIC - symbolic
         A_LOGICAL  - logical
         A_TUPLE    - n-tuple
         A_ELEMSET  - elemental set
         A_FORMULA  - linear form */
      int dim;
      /* dimension of the resultant value; for A_TUPLE and A_ELEMSET it
         is the dimension of the corresponding n-tuple(s) and cannot be
         zero; for other resultant types it is always zero */
      CODE *up;
      /* parent pseudo-code, which refers to this pseudo-code as to its
         operand; NULL means this pseudo-code has no parent and defines
         an expression, which is not contained in another expression */
      int vflag;
      /* volatile flag; being set this flag means that this operation
         has a side effect; for primary expressions this flag is set
         directly by corresponding parsing routines (for example, if
         primary expression is a reference to a function that generates
         pseudo-random numbers); in other cases this flag is inherited
         from operands */
      int valid;
      /* if this flag is set, the resultant value, which is a temporary
         result of evaluating this operation on particular values of
         operands, is valid; if this flag is clear, the resultant value
         doesn't exist and therefore not valid; having been evaluated
         the resultant value is stored here and not destroyed until the
         dummy indices, which this value depends on, have been changed
         (and if it doesn't depend on dummy indices at all, it is never
         destroyed); thus, if the resultant value is valid, evaluating
         routine can immediately take its copy not computing the result
         from scratch; this mechanism is similar to moving invariants
         out of loops and allows improving efficiency at the expense of
         some extra memory needed to keep temporary results */
      /* however, if the volatile flag (see above) is set, even if the
         resultant value is valid, evaluating routine computes it as if
         it were not valid, i.e. caching is not used in this case */
      VALUE value;
      /* resultant value in generic format */
};

double eval_numeric(MPL *mpl, CODE *code);
/* evaluate pseudo-code to determine numeric value */

SYMBOL *eval_symbolic(MPL *mpl, CODE *code);
/* evaluate pseudo-code to determine symbolic value */

int eval_logical(MPL *mpl, CODE *code);
/* evaluate pseudo-code to determine logical value */

TUPLE *eval_tuple(MPL *mpl, CODE *code);
/* evaluate pseudo-code to construct n-tuple */

ELEMSET *eval_elemset(MPL *mpl, CODE *code);
/* evaluate pseudo-code to construct elemental set */

int is_member(MPL *mpl, CODE *code, TUPLE *tuple);
/* check if n-tuple is in set specified by pseudo-code */

FORMULA *eval_formula(MPL *mpl, CODE *code);
/* evaluate pseudo-code to construct linear form */

void clean_code(MPL *mpl, CODE *code);
/* clean pseudo-code */

/**********************************************************************/
/* * *                      MODEL STATEMENTS                      * * */
/**********************************************************************/

struct CHECK
{     /* check statement */
      DOMAIN *domain;
      /* subscript domain; NULL means domain is not used */
      CODE *code;
      /* code for computing the predicate to be checked */
};

struct DISPLAY
{     /* display statement */
      DOMAIN *domain;
      /* subscript domain; NULL means domain is not used */
      DISPLAY1 *list;
      /* display list; cannot be empty */
};

struct DISPLAY1
{     /* display list entry */
      int type;
      /* item type:
         A_INDEX      - dummy index
         A_SET        - model set
         A_PARAMETER  - model parameter
         A_VARIABLE   - model variable
         A_CONSTRAINT - model constraint/objective
         A_EXPRESSION - expression */
      union
      {  DOMAIN_SLOT *slot;
         SET *set;
         PARAMETER *par;
         VARIABLE *var;
         CONSTRAINT *con;
         CODE *code;
      } u;
      /* item to be displayed */
      ARG_LIST *list;
      /* optional subscript list (for constraint/objective only) */
      DISPLAY1 *next;
      /* the next entry for the same statement */
};

struct PRINTF
{     /* printf statement */
      DOMAIN *domain;
      /* subscript domain; NULL means domain is not used */
      CODE *fmt;
      /* pseudo-code for computing format string */
      PRINTF1 *list;
      /* printf list; can be empty */
#if 1 /* 14/VII-2006 */
      CODE *fname;
      /* pseudo-code for computing filename to redirect the output;
         NULL means the output goes to stdout */
      int app;
      /* if this flag is set, the output is appended */
#endif
};

struct PRINTF1
{     /* printf list entry */
      CODE *code;
      /* pseudo-code for computing value to be printed */
      PRINTF1 *next;
      /* the next entry for the same statement */
};

struct FOR
{     /* for statement */
      DOMAIN *domain;
      /* subscript domain; cannot be NULL */
      STATEMENT *list;
      /* linked list of model statements within this for statement in
         the original order */
};

struct STATEMENT
{     /* model statement */
      int line;
      /* number of source text line, where statement begins */
      int type;
      /* statement type:
         A_SET        - set statement
         A_PARAMETER  - parameter statement
         A_VARIABLE   - variable statement
         A_CONSTRAINT - constraint/objective statement
         A_TABLE      - table statement
         A_SOLVE      - solve statement
         A_CHECK      - check statement
         A_DISPLAY    - display statement
         A_PRINTF     - printf statement
         A_FOR        - for statement */
      union
      {  SET *set;
         PARAMETER *par;
         VARIABLE *var;
         CONSTRAINT *con;
#if 1 /* 11/II-2008 */
         TABLE *tab;
#endif
         void *slv; /* currently not used (set to NULL) */
         CHECK *chk;
         DISPLAY *dpy;
         PRINTF *prt;
         FOR *fur;
      } u;
      /* specific part of statement */
      STATEMENT *next;
      /* the next statement; in this list statements follow in the same
         order as they appear in the model section */
};

#if 1 /* 11/II-2008 */
#define execute_table _glp_mpl_execute_table
void execute_table(MPL *mpl, TABLE *tab);
/* execute table statement */

#define free_dca _glp_mpl_free_dca
void free_dca(MPL *mpl);
/* free table driver communucation area */

#define clean_table _glp_mpl_clean_table
void clean_table(MPL *mpl, TABLE *tab);
/* clean table statement */
#endif

void execute_check(MPL *mpl, CHECK *chk);
/* execute check statement */

void clean_check(MPL *mpl, CHECK *chk);
/* clean check statement */

void execute_display(MPL *mpl, DISPLAY *dpy);
/* execute display statement */

void clean_display(MPL *mpl, DISPLAY *dpy);
/* clean display statement */

void execute_printf(MPL *mpl, PRINTF *prt);
/* execute printf statement */

void clean_printf(MPL *mpl, PRINTF *prt);
/* clean printf statement */

void execute_for(MPL *mpl, FOR *fur);
/* execute for statement */

void clean_for(MPL *mpl, FOR *fur);
/* clean for statement */

void execute_statement(MPL *mpl, STATEMENT *stmt);
/* execute specified model statement */

void clean_statement(MPL *mpl, STATEMENT *stmt);
/* clean specified model statement */

/**********************************************************************/
/* * *              GENERATING AND POSTSOLVING MODEL              * * */
/**********************************************************************/

void alloc_content(MPL *mpl);
/* allocate content arrays for all model objects */

void generate_model(MPL *mpl);
/* generate model */

void build_problem(MPL *mpl);
/* build problem instance */

void postsolve_model(MPL *mpl);
/* postsolve model */

void clean_model(MPL *mpl);
/* clean model content */

/**********************************************************************/
/* * *                        INPUT/OUTPUT                        * * */
/**********************************************************************/

void open_input(MPL *mpl, char *file);
/* open input text file */

int read_char(MPL *mpl);
/* read next character from input text file */

void close_input(MPL *mpl);
/* close input text file */

void open_output(MPL *mpl, char *file);
/* open output text file */

void write_char(MPL *mpl, int c);
/* write next character to output text file */

void write_text(MPL *mpl, char *fmt, ...);
/* format and write text to output text file */

void flush_output(MPL *mpl);
/* finalize writing data to output text file */

/**********************************************************************/
/* * *                      SOLVER INTERFACE                      * * */
/**********************************************************************/

#define MPL_FR          401   /* free (unbounded) */
#define MPL_LO          402   /* lower bound */
#define MPL_UP          403   /* upper bound */
#define MPL_DB          404   /* both lower and upper bounds */
#define MPL_FX          405   /* fixed */

#define MPL_ST          411   /* constraint */
#define MPL_MIN         412   /* objective (minimization) */
#define MPL_MAX         413   /* objective (maximization) */

#define MPL_NUM         421   /* continuous */
#define MPL_INT         422   /* integer */
#define MPL_BIN         423   /* binary */

void mpl_error(MPL *mpl, char *fmt, ...);
/* print error message and terminate model processing */

void warning(MPL *mpl, char *fmt, ...);
/* print warning message and continue model processing */

MPL *mpl_initialize(void);
/* create and initialize translator database */

int mpl_read_model(MPL *mpl, char *file, int skip_data);
/* read model section and optional data section */

int mpl_read_data(MPL *mpl, char *file);
/* read data section */

int mpl_generate(MPL *mpl, char *file);
/* generate model */

char *mpl_get_prob_name(MPL *mpl);
/* obtain problem (model) name */

int mpl_get_num_rows(MPL *mpl);
/* determine number of rows */

int mpl_get_num_cols(MPL *mpl);
/* determine number of columns */

char *mpl_get_row_name(MPL *mpl, int i);
/* obtain row name */

int mpl_get_row_kind(MPL *mpl, int i);
/* determine row kind */

int mpl_get_row_bnds(MPL *mpl, int i, double *lb, double *ub);
/* obtain row bounds */

int mpl_get_mat_row(MPL *mpl, int i, int ndx[], double val[]);
/* obtain row of the constraint matrix */

double mpl_get_row_c0(MPL *mpl, int i);
/* obtain constant term of free row */

char *mpl_get_col_name(MPL *mpl, int j);
/* obtain column name */

int mpl_get_col_kind(MPL *mpl, int j);
/* determine column kind */

int mpl_get_col_bnds(MPL *mpl, int j, double *lb, double *ub);
/* obtain column bounds */

int mpl_has_solve_stmt(MPL *mpl);
/* check if model has solve statement */

void mpl_put_col_value(MPL *mpl, int j, double val);
/* store column value */

int mpl_postsolve(MPL *mpl);
/* postsolve model */

void mpl_terminate(MPL *mpl);
/* free all resources used by translator */

#endif

/* eof */
