#pragma once
#include <unordered_map>
#include <string>

static std::unordered_map<int, std::string> rule = {

{1, "<Rat21F> ::= <Opt Function Definitions> # <Opt Declaration List> <Statement List> #"},
{2, "<Opt Function Definitions> ::= <Function Definitions> | <Empty>"},
{3, "<Function Definitions> ::= <Function> | <Function> <Function Definitions>"},
{4, "<Function> ::= function <Identifier> ( <Opt Parameter List> ) <Opt Declaration List> <Body>"},
{5, "<Opt Parameter List> ::= <Parameter List> | <Empty>"},
{6, "<Parameter List> ::= <Parameter> | <Parameter> , <Parameter List>"},
{7, "<Parameter> ::= <IDs > <Qualifier>"},
{8, "<Qualifier> ::= integer | boolean | real"},
{9, "<Body> ::= { < Statement List> }"},
{10, "<Opt Declaration List> ::= <Declaration List> | <Empty>"},
{11, "<Declaration List> := <Declaration> ; | <Declaration> ; <Declaration List>"},
{12, "<Declaration> ::= <Qualifier > <IDs>"},
{13, "<IDs> ::= <Identifier> | <Identifier>, <IDs>"},
{14, "<Statement List> ::= <Statement> | <Statement> <Statement List>"},
{15, "<Statement> ::= <Compound> | <Assign> | <If> | <Return> | <Print> | <Scan> | <While>"},
{16, "<Compound> ::= { <Statement List> }"},
{17, "<Assign> ::= <Identifier> = <Expression> ;"},
{18, "<If> ::= if ( <Condition> ) <Statement> endif | if ( <Condition> ) <Statement> else <Statement> endif"},
{19, "<Return> ::= return ; | return <Expression> ;"},
{20, "<Print> ::= put ( <Expression>);"},
{21, "<Scan> ::= get ( <IDs> );"},
{22, "<While> ::= while ( <Condition> ) <Statement>"},
{23, "<Condition> ::= <Expression> <Relop> <Expression>"},
{24, "<Relop> ::= == | != | > | < | <= | =>"},
/*{25, "<Expression> ::= <Expression> + <Term> | <Expression> - <Term> | <Term>"},*/
{25,  "<Expression> ::= <Term> <Expression_Prime>"},
{251, "<Expression_Prime> ::= + <Term> <Expression_Prime> | - <Term> <Expression_Prime> | ε"},
/*{26, "<Term> ::= <Term> * <Factor> | <Term> / <Factor> | <Factor>"},*/
{26, "<Term> ::= <Factor> <Term_Prime>"},
{261, "Term_Prime ::= * <Factor> <Term_Prime> | / <Factor> <Term_Prime> | <Factor>"},
{27, "<Factor> ::= - <Primary> | <Primary>"},
{28, "<Primary> ::= <Identifier> | <Integer> | <Identifier> ( <IDs> ) | ( <Expression> ) | <Real> | true | false"},
{29, "<Empty> ::= "}

};