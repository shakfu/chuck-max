/**
 * @file Chuck grammar for tree-sitter
 * @author Ben Rosen <benjamin.j.rosen@gmail.com>
 * @license MIT
 */

/// <reference types="tree-sitter-cli/dsl" />
// @ts-check

module.exports = grammar({
  extras: ($) => [/\s/, $.block_comment, $.line_comment],
  name: "chuck",

  rules: {
    source_file: ($) =>
      repeat(
        choice(
          $.block,
          $.class_definition,
          $.conditional,
          $._control_structure,
          $.function_definition,
          $.import_expression,
          $.overload_definition,
          $.statement,
        ),
      ),

    array: ($) => seq("[", $._expression_list, "]"),

    array_identifier: ($) =>
      prec(
        1,
        seq(
          choice(
            $.class_identifier,
            $.function_call,
            $._function_call_chain,
            $.member_identifier,
            $.reference_type,
            $.variable_identifier,
          ),
          repeat1(seq("[", optional($._expression), "]")),
        ),
      ),

    binary_expression: ($) =>
      prec.left(seq($._expression, $.operator, $._expression)),

    block: ($) =>
      seq(
        "{",
        repeat(
          choice(
            $.class_definition,
            $.conditional,
            $._control_structure,
            $.function_definition,
            $.method_definition,
            $.overload_definition,
            $.statement,
          ),
        ),
        "}",
      ),

    block_comment: () => token(seq("/*", /[^*]*\*+([^/*][^*]*\*+)*/, "/")),
    boolean_literal_value: () => choice("false", "true"),

    cast: ($) =>
      seq($._expression, "$", choice($.primitive_type, $.reference_type)),

    character: ($) => seq("'", choice(/[^\s]/, $.escape_character), "'"),
    _chuck_keyword: () => choice("const", "function", "global", "spork"),

    chuck_operation: ($) =>
      choice(
        seq(
          choice($.chuck_operation, $._expression),
          $.chuck_operator,
          choice(
            $._declaration,
            $.function_call,
            $._identifier,
            $.keyword,
            $.member_identifier,
          ),
        ),
        seq("(", $.chuck_operation, ")"),
      ),

    chuck_operator: () =>
      choice(
        "%=>",
        "&=>",
        "*=>",
        "+=>",
        "-->",
        "-=>",
        "/=>",
        "=<",
        "=>",
        "=^",
        "@=>",
        "^=>",
        "|=>",
      ),

    class_definition: ($) =>
      seq(
        optional($.doc_comment),
        optional("public"),
        "class",
        choice($.class_identifier, $.variable_identifier),
        optional(seq("extends", $.class_identifier)),
        $.block,
      ),

    class_identifier: () => /_?[A-Z][a-zA-Z0-9_]*/,

    class_instantiation: ($) =>
      seq(
        "new",
        choice($.function_call, $._function_call_chain, $._identifier),
      ),

    _class_keyword: () =>
      choice(
        "extends",
        "implements",
        "interface",
        "private",
        "protected",
        "pure",
        "static",
        "super",
        "this",
      ),

    complex: ($) => seq("#(", $._expression, ",", $._expression, ")"),

    conditional: ($) =>
      prec(
        1,
        seq(
          "if",
          "(",
          $._expression,
          ")",
          $._control_structure_body,
          optional(
            seq("else", choice($.conditional, $._control_structure_body)),
          ),
        ),
      ),

    _control_structure: ($) =>
      choice($.do_loop, $.for_loop, $.for_each_loop, $.loop),

    control_structure_keyword: () => choice("break", "continue", "switch"),
    _control_structure_body: ($) => choice($.block, $.statement, ";"),
    debug_print: ($) => seq("<<<", $._expression_list, ">>>"),

    _declaration: ($) =>
      choice($.reference_declaration, $.variable_declaration),

    doc_comment: ($) => seq("@doc", $.string),

    documented_expression: ($) =>
      seq($.doc_comment, choice($.chuck_operation, $._expression)),

    do_loop: ($) =>
      seq("do", $._control_structure_body, $._until_while_expression),

    duration_identifier: () =>
      choice("day", "hour", "minute", "ms", "samp", "second", "week"),

    dur: ($) =>
      seq(
        $._expression,
        "::",
        choice(
          $.class_identifier,
          $.duration_identifier,
          $.variable_identifier,
        ),
      ),

    escape_character: () =>
      token(seq("\\", choice(/[0-1][0-7][0-7]/, "0", "a", "n", "t", '"'))),

    _expression: ($) =>
      prec(
        2,
        choice(
          $.array,
          $.binary_expression,
          $.cast,
          $.character,
          $.class_instantiation,
          $.conditional,
          $._control_structure,
          $.debug_print,
          $._declaration,
          $.expression_group,
          $.function_call,
          $._function_call_chain,
          $._identifier,
          $._increment_expression,
          $.keyword,
          $.member_identifier,
          $.negation_expression,
          $.negative_expression,
          $._number,
          $.reference_values,
          $.spork_expression,
          $.string,
          $.ternary_expression,
        ),
      ),

    expression_group: ($) =>
      prec(
        1,
        seq("(", optional(choice($._expression, $._expression_list)), ")"),
      ),

    _expression_list: ($) => {
      const expression = choice($.chuck_operation, $._expression);
      return prec.left(seq(expression, repeat(seq(",", expression))));
    },

    float: () => /\d+[eE][+-]?\d+|(\d*\.\d+|\d+\.)([eE][+-]?\d+)?/,

    for_loop: ($) =>
      seq(
        "for",
        "(",
        optional(
          choice($.chuck_operation, $._identifier, $.variable_declaration),
        ),
        ";",
        choice($.binary_expression, $.boolean_literal_value),
        ";",
        choice($.chuck_operation, $._expression),
        ")",
        $._control_structure_body,
      ),

    for_each_loop: ($) =>
      prec(
        1,
        seq(
          "for",
          "(",
          seq($.variable_declaration, ":", $._expression),
          ")",
          $._control_structure_body,
        ),
      ),

    function_call: ($) =>
      seq(
        choice($._identifier, $.member_identifier),
        seq("(", field("argument", optional($._expression_list)), ")"),
      ),

    _function_call_chain: ($) =>
      seq($.function_call, repeat1(prec.left(seq(".", $.function_call)))),

    function_definition: ($) =>
      seq(
        optional($.doc_comment),
        $._function_keyword,
        $._function_name_and_body,
      ),

    _function_keyword: () => choice("fun", "function"),

    _function_name_and_body: ($) =>
      seq(
        optional($._type),
        field("name", choice($.class_identifier, $.variable_identifier)),
        $._function_parameters,
        $.block,
      ),

    _function_parameters: ($) =>
      seq(
        "(",
        field(
          "parameter",
          optional(seq($._declaration, repeat(seq(",", $._declaration)))),
        ),
        ")",
      ),

    global_unit_generator: () => choice("adc", "blackhole", "dac"),
    hexidecimal: () => token(seq("0", /x/i, /[\da-fA-F](_?[\da-fA-F])*/)),
    post_increment_expression: ($) => seq($._expression, choice("++", "--")),

    pre_increment_expression: ($) =>
      prec(1, seq(choice("++", "--"), $._expression)),

    _increment_expression: ($) =>
      choice($.post_increment_expression, $.pre_increment_expression),

    _identifier: ($) =>
      prec(
        1,
        choice(
          $.array_identifier,
          $.class_identifier,
          $.reference_type,
          $.variable_identifier,
        ),
      ),

    import_expression: ($) =>
      seq(
        "@import",
        choice($.string, seq("{", $.string, repeat(seq(",", $.string)), "}")),
        optional(";"),
      ),

    int: () => /\d+/,

    keyword: ($) =>
      choice(
        $._chuck_keyword,
        $._class_keyword,
        $.control_structure_keyword,
        $.duration_identifier,
        $.global_unit_generator,
        $.primitive_type,
        $._literal_value,
      ),

    line_comment: () => token(seq("//", /[^\r\n\u2028\u2029]*/)),

    _literal_value: ($) =>
      choice($.boolean_literal_value, $.special_literal_value),

    loop: ($) =>
      seq(
        choice(
          seq("repeat", "(", $._expression, ")"),
          $._until_while_expression,
        ),
        $._control_structure_body,
      ),

    member_identifier: ($) =>
      seq(
        choice(
          "me",
          "this",
          "super",
          $.complex,
          $.expression_group,
          $.global_unit_generator,
          $._identifier,
          $.polar,
          $.string,
        ),
        repeat1(seq(".", choice($.class_identifier, $.variable_identifier))),
      ),

    method_definition: ($) =>
      seq(
        optional($.doc_comment),
        optional("public"),
        choice(
          seq(
            $._function_keyword,
            choice("@construct", "@destruct"),
            $._function_parameters,
            $.block,
          ),
          $._function_name_and_body,
        ),
      ),

    negation_expression: ($) => prec.left(seq(choice("!", "~"), $._expression)),
    negative_expression: ($) => prec.left(seq("-", $._expression)),

    _number: ($) =>
      choice($.complex, $.dur, $.float, $.hexidecimal, $.int, $.polar),

    operator: () =>
      choice(
        "!=",
        "%",
        "&",
        "&&",
        "*",
        "+",
        "-",
        "/",
        "<",
        "<<",
        "<=",
        "==",
        ">",
        ">=",
        ">>",
        "^",
        "|",
        "||",
      ),

    overload_definition: ($) => {
      const operator = choice("!", "~", "++", "--", $.chuck_operator, $.operator);

      const parameters = seq(
        "(",
        field(
          "parameter",
          optional(seq($._declaration, repeat(seq(",", $._declaration)))),
        ),
        ")",
      );

      return seq(
        choice("private", "public", $._function_keyword),
        $._type,
        "@operator",
        choice(seq(operator, parameters), seq(parameters, operator)),
        $.block,
      );
    },

    polar: ($) => seq("%(", $._expression, ",", $._expression, ")"),

    primitive_type: () =>
      choice(
        "auto",
        "complex",
        "dur",
        "float",
        "int",
        "polar",
        "same",
        "time",
        "vec2",
        "vec3",
        "vec4",
        "void",
      ),

    reference_declaration: ($) => seq($._type, "@", $._identifier),
    reference_type: () => choice("Event", "Object", "UGen", "array", "string"),
    reference_values: ($) => seq("@(", $._expression_list, ")"),

    special_literal_value: () =>
      choice("NULL", "maybe", "me", "now", "null", "pi"),

    spork_expression: ($) =>
      seq("spork", "~", choice($.function_call, $._function_call_chain)),

    statement: ($) =>
      seq(
        choice(
          "return",
          seq(
            optional("return"),
            choice(
              $.chuck_operation,
              $.documented_expression,
              $._expression,
              $._expression_list,
              $.function_definition,
              $.overload_definition,
            ),
          ),
        ),
        ";",
      ),

    string: () => {
      const delimeter = '"';
      return token(choice(
        seq(delimeter, /(\\.|[^"\\])*/, delimeter),
        seq("`", /[^`]*/, "`"),
      ));
    },

    ternary_expression: ($) =>
      prec.left(seq($._expression, "?", $._expression, ":", $._expression)),

    _type: ($) =>
      seq(
        optional("static"),
        choice(
          $.variable_identifier,
          seq(
            choice($.class_identifier, $.primitive_type, $.reference_type),
            optional("[]"),
          ),
        ),
      ),

    _until_while_expression: ($) =>
      seq(
        choice("until", "while"),
        "(",
        choice($.chuck_operation, $._expression),
        ")",
      ),

    variable_identifier: () => /_?[a-z][a-zA-Z0-9_]*/,

    variable_declaration: ($) => {
      const identifier = choice(
        $.function_call,
        $._function_call_chain,
        $._identifier,
      );

      return prec.right(
        seq(
          optional("global"),
          $._type,
          seq(identifier, repeat(seq(",", optional($._type), identifier))),
        ),
      );
    },
  },

  word: ($) => $.variable_identifier,
});
