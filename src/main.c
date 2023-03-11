#include<Algebra/expressionparser.h>
#define TEST_STR "a = 3\na + 1"
int main(int argc, char* argv)
{
  while(1){
    printf("> ");
    char* str = malloc(1024);
    gets(str);
    if(!strcmp("quit", str))
    {
      return 0;
    }
    Parser* parser = InitializeParser(str);
    Node* n = Parse(parser);
    Evaluator* evaluator = InitializeEvaluator(n);
    PrintTree(n, 0, 1);
    float end = Evaluate(evaluator);
    printf("\n%lf\n",end);
    free(parser);
    free(evaluator);
    if(str != TEST_STR)
      free(str);
  }
}