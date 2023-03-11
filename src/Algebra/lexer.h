#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
enum TokenTypes
{
    END,
    CONSTANT,
    VARIABLE,
    SYMBOL,
    BRACKET,
    BAD
};

char* GetTypeName(int t)
{
  switch(t)
  {
    case END:
    {
      return "END";
    }
    case CONSTANT:
    {
      return "CONSTANT";
    }
    case VARIABLE:
    {
      return "VARIABLE";
    }
    case SYMBOL:
    {
      return "SYMBOL";
    }
    case BRACKET:
    {
      return "BRACKET";
    }
  }
  return "UNKNOWN";
}

typedef struct
{
    int Position;
    int Type;
    char* Text;
    double Value;
} Token;

Token* InitializeToken(int Pos, int Type, char* Text, double Value)
{
    Token* t = malloc(sizeof(Token));
    *t = (Token)
    {
      Pos, 
      Type, 
      Text, 
      Value
    };
    if(Type == BAD)
    {
      printf("UNEXPECTED CHAR '%c'\n", t->Text[0]);
      exit(-1);
    }
    return t;
}

typedef struct
{
    int Position;
    char* Text;
} Lexer;

Lexer* InitializeLexer(char* Text)
{
    Lexer* lexer = malloc(sizeof(Lexer));
    *lexer = (Lexer){0, Text};
    return lexer;
}
int IsDigit(char c)
{
  return c >= '0' && c <= '9';
}
int IsAlphabet(char c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
Token* LexSingleChar(Lexer* lexer, int Type)
{
  char* c = malloc(2);
  c[0] = lexer->Text[lexer->Position];
  c[1] = 0;
  return InitializeToken(lexer->Position++, Type, c, 0);
}
Token* LexConstant(Lexer* lexer);
Token* Lex(Lexer* lexer)
{
    if(IsDigit(lexer->Text[lexer->Position]) || lexer->Text[lexer->Position] == '-')
    {
      return LexConstant(lexer);
    }
    if(IsAlphabet(lexer->Text[lexer->Position]))
    {
      if(IsDigit(lexer->Text[lexer->Position + 1]))
      {
        char* c = malloc(3);
        c[0] = lexer->Text[lexer->Position];
        c[1] = lexer->Text[lexer->Position + 1];
        c[2] = 0;
        lexer->Position += 2;
        return InitializeToken(lexer->Position - 2, VARIABLE, c, 0);
      }
      return LexSingleChar(lexer, VARIABLE);
    }
    switch(lexer->Text[lexer->Position])
    {
      case 0:
      {
        return InitializeToken(0,0,"", 0);
      }
      case ' ':
      {
        while(lexer->Text[lexer->Position] == ' ')
        {
          ++lexer->Position;
        }
        return Lex(lexer);
      }
      case '(':
      case ')':
      {
        return LexSingleChar(lexer, BRACKET);
      }
      case '\n':
      case '^':
      case '-':
      case '/':
      case '*':
      case '+':
      case '=':
      {
        return LexSingleChar(lexer, SYMBOL);
      }
    }
    return LexSingleChar(lexer, BAD);
}


Token* LexConstant(Lexer* lexer)
{
    int HasDecimal = 0;
    int Start = lexer->Position;
    ++lexer->Position;
    while
    (
      IsDigit(lexer->Text[lexer->Position]) 
      || (!HasDecimal && lexer->Text[lexer->Position] == '.')
    )
    {
      if(lexer->Text[lexer->Position] == '.')
          HasDecimal = 1;
      ++lexer->Position;
    }
    char* num = malloc(1 + (lexer->Position - Start));
    lexer->Position = Start + 1;
    num[0] = lexer->Text[Start];
    while
    (
      IsDigit(lexer->Text[lexer->Position]) 
      || (!HasDecimal && lexer->Text[lexer->Position] == '.')
    )
    {
      if(lexer->Text[lexer->Position] == '.')
          HasDecimal = 1;
      num[lexer->Position - Start] = lexer->Text[lexer->Position];
      ++lexer->Position;
    }
    num[lexer->Position - Start] = 0;
    double number;
    sscanf(num, "%lf", &number);
    return InitializeToken
    (
      lexer->Position, 
      CONSTANT,
      num, 
      number
    );
}
