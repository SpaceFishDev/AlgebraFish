#include<algebra/lexer.h>

enum NodeTypes
{
  CONSTANT_NODE,
  BINEXPR_NODE,   
  PARENEXPR_NODE,
  VARIABLE_NODE,
};

char* NodeGetTypeName(int type)
{
  switch(type)
  {
    case CONSTANT_NODE:
    return "CONSTANT_NODE";
    case BINEXPR_NODE:
    return "BINEXPR_NODE";
    case PARENEXPR_NODE:
    return "PARENEXPR_NODE";
    case VARIABLE_NODE:
    return "VARIABLE_NODE";
  }
  return "UNKNOWN_NODE";
}

typedef struct Node
{
  int Type;
  Token* nodeToken;
  struct Node** Children;
  int NChild;
} Node;

Node* InitializeRootNode()
{
  Node* n = malloc(sizeof(Node));
  *n = (Node){0};
  return n;
}

void addChild(Node* n, Node* src)
{
  realloc(n->Children, sizeof(Node*) * n->NChild + 1);
  n->Children[n->NChild] = src;
  ++n->NChild;
}

Node* InititalizeNumberNode(Token* Number)
{
  Node* n = malloc(sizeof(Node));
  *n = (Node){CONSTANT_NODE, Number, 0, 0};
  return n;
}

Node* InititalizeBinaryExpression(Node* Left, Token* Operator, Node* Right)
{
  Node* n = malloc(sizeof(Node));
  n->Children = malloc(sizeof(Node*)*2);
  *n->Children = malloc(sizeof(Node)*2);
  n->Children[0] = Left;
  n->Children[1] = Right;
  n->nodeToken = Operator;
  n->Type = BINEXPR_NODE;
  n->NChild = 2;
  return n;
}

Node* InitializeVarNode(Token* token)
{
  Node* n = malloc(sizeof(Node));
  n->nodeToken = token;
  n->Type = VARIABLE_NODE;
  n->NChild = 0;
  n->Children = 0;
  return n;
}

Node* InitializeParenthesizedExpression(Token* open, Node* expression, Token* close)
{
  Node* n = malloc(sizeof(Node));
  Node** Children = malloc(sizeof(Node*));
  *Children = expression;
  *n = (Node){PARENEXPR_NODE, open, Children, 1};
  return n;
}

typedef struct
{
  int Position;
  Token** Tokens;
  int NToken;
} Parser;

Parser* InitializeParser(char* Text)
{
  Lexer* lexer = InitializeLexer(Text);
  Parser* p = malloc(sizeof(Parser));
  int len = 0;
  while(1)
  {
    Token* t = Lex(lexer);
    ++len;
    if(t->Type == END)
    {
        break;
    }
  }
  free(lexer);
  p->Tokens = malloc(sizeof(Token*) * len);
  lexer = InitializeLexer(Text);
  for(int i = 0; i != len; ++i)
  {
    p->Tokens[i] = Lex(lexer);
    printf
    (
      "TOKEN(%s, %s)\n", 
      GetTypeName(p->Tokens[i]->Type), 
      p->Tokens[i]->Text
    );
  }
  p->Position = 0;
  p->NToken = len;
  return p;
}
#define Peek(i) parser->Tokens[parser->Position + i]
#define CurrentToken Peek(0)
#define NextToken parser->Tokens[parser->Position++]

Token* Match(Parser* parser, int type)
{
  if(CurrentToken->Type == type)
  {
    return CurrentToken;
  }
  return InitializeToken(parser->Position, type, "", 0);
}

Node* ParsePrimaryExpression(Parser* parser);
Node* ParseTerm(Parser* parser);

Node* Parse(Parser* parser, Node* root)
{
  Node* n = ParseTerm(parser);
  addChild();
}
Node* ParseTerm(Parser* parser);
Node* ParseFactor(Parser* parser);
Node* ParseExpression(Parser* parser)
{
  return ParseTerm(parser);
}
Node* ParseTerm(Parser* parser)
{
  if(CurrentToken->Text[0] == '\n')
  {
    ++parser->Position;
    return Parse(parser);
  }
  Node* left = ParseFactor(parser);
  ++parser->Position;
  if(CurrentToken->Text[0] == '=')
  {
    Token* operatorToken = NextToken;
    Node* right = ParseFactor(parser);
    ++parser->Position;
    left = InititalizeBinaryExpression(left,operatorToken, right);
    return left;
  }
  while
  (
    CurrentToken && CurrentToken->Text[0] == '+'
    || CurrentToken->Text[0] == '-'
  )
  {
    Token* operatorToken = NextToken;
    Node* right = ParseFactor(parser);
    ++parser->Position;
    left = InititalizeBinaryExpression(left,operatorToken, right);
  }
  return left;
}

Node* ParseFactor(Parser* parser)
{
  Node* left = ParsePrimaryExpression(parser);
  if(left->Type == VARIABLE_NODE && parser->Tokens[parser->Position + 1]->Text[0] != '=')
  {
    // --parser->Position;
    return left;
  }
  ++parser->Position;
  if(parser->Position < parser->NToken && CurrentToken->Text[0 ]!= '*' && CurrentToken->Text[0] != '/' )
  {
    --parser->Position;
    return left;
  }
  Token* operatorToken = NextToken;
  Node* right = ParseFactor(parser);
  left = InititalizeBinaryExpression(left,operatorToken, right);
  return left;
}

Node* ParsePrimaryExpression(Parser* parser)
{
  if(CurrentToken->Type == VARIABLE)
  {
    Token* v = parser->Tokens[parser->Position];
    return InitializeVarNode(v);
  }
  if(CurrentToken->Type == BRACKET)
  {
    Token* left = NextToken;
    Node* expression = ParseExpression(parser);
    Token* right = Match(parser, BRACKET);
    // ++parser->Position;
    return InitializeParenthesizedExpression(left, expression, right);
  }

  Token* Number = Match(parser, CONSTANT);
  if(Number->Text)
    return InititalizeNumberNode(Number);
}



void PrintTree(Node* node, int depth, int isLastChild) { 
    for (int i = 0; i < depth; i++) {
        if (i == depth - 1) {
            if (isLastChild) {
                printf("\\--");
            } else {
                printf("|--");
            }
        } else {
            printf("|   ");
        }
    }

    printf("node( %s, %s )\n", node->nodeToken->Text, NodeGetTypeName(node->Type));

    for (int i = 0; i < node->NChild; i++) {
        if (node->Children[i] != NULL) {
            int childIsLast = (i == node->NChild - 1) ? 1 : 0;
            PrintTree(node->Children[i], depth+1, childIsLast);
        }
    }
}

typedef struct Variable
{
  char* name;
  Node* Value;
} Variable;

typedef struct Evaluator
{
  Node* root;
  Variable* Variables;
  int NVar;
} Evaluator;

void AddVar(Evaluator** ev, Variable v)
{
    (*ev)->NVar++;
    (*ev)->Variables = realloc((*ev)->Variables, (*ev)->NVar * sizeof(Variable));

    (*ev)->Variables[(*ev)->NVar - 1] = v;
}

Evaluator* InitializeEvaluator(Node* root)
{
  Evaluator* ev = malloc(sizeof(Evaluator));
  *ev = (Evaluator){root};
  return ev;
}
double EvaluateExpression(Node* root, Evaluator* evaluator);
double Evaluate(Evaluator* evaluator)
{
  return EvaluateExpression(evaluator->root, evaluator);
}

volatile double EvaluateExpression(Node* root, Evaluator* evaluator)
{
  if(!root)
  {
    return 0;
  }
  if(root->Type == VARIABLE_NODE)
  {
    Variable v = (Variable){0};
    for(int i = 0; i != evaluator->NVar; ++i)
    {
      if(root->nodeToken->Text == evaluator->Variables[i].name)
      {
        v = evaluator->Variables[i];
        break;
      }
    }
    return EvaluateExpression(v.Value, evaluator);
  }
  if(root->Type == CONSTANT_NODE)
    return root->nodeToken->Value;
  if(root->Type == BINEXPR_NODE)
  {
    if(root->nodeToken->Text[0] == '=')
    {
      Variable v = (Variable){root->Children[0]->nodeToken->Text, root->Children[1]};
      AddVar(&evaluator, v);
      return EvaluateExpression(root->Children[0], evaluator);
    }
    double left = EvaluateExpression(root->Children[0], evaluator);
    double right = EvaluateExpression(root->Children[1], evaluator);

    if(root->nodeToken->Text[0] == '+')
      return left + right;
    if(root->nodeToken->Text[0] == '-')
      return left - right;
    if(root->nodeToken->Text[0] == '/')
      return left / right;
    if(root->nodeToken->Text[0] == '*')
      return left * right;
  } 
  if(root->Type == PARENEXPR_NODE)
  {
    return EvaluateExpression(root->Children[0], evaluator);
  }
  // return 0; 
}