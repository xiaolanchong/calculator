#include <iostream>
#include <string>
#include <memory>
#include <istream>
#include <optional>
#include <stdexcept>
#include <assert.h>

#include "LLParser.h"
#include "LRParser.h"

#ifdef DEBUG
#define my_assert assert
#else
void my_assert(bool condition) { if(!condition) throw std::runtime_error("Assertion violation"); }
#endif

void doCmdLineStuff()
{
   std::string expr;
   std::getline(std::cin, expr);
#if 0
   LLParser calc;
#else
   LRParser calc;
#endif
   try
   {
      auto res = calc.ParseAndCalculate(expr);
      if(calc.IsEmpty())
         std::cout << res;
      else
         std::cout << "WRONG";
   }
   catch (const std::exception&)
   {
      std::cout << "WRONG";
   }
}

void test();

int main()
{
#if 0
   doCmdLineStuff();
#endif
   test();
   return 0;
}

template<typename Parser>
void testWithParser()
{
   Parser calc;
   NumberType res = 0;
   res = calc.ParseAndCalculate("6");
   my_assert(res == 6);
   res = calc.ParseAndCalculate("6+5");
   my_assert(res == 11);

   res = calc.ParseAndCalculate("6 + 5 - 3");
   my_assert(res == 8);
   res = calc.ParseAndCalculate("\t6 + 5 \t - 3");
   my_assert(res == 8);

   res = calc.ParseAndCalculate("6 + 5 + 3");
   my_assert(res == 14);

   res = calc.ParseAndCalculate("6 + (5 - 3)");
   my_assert(res == 8);

   res = calc.ParseAndCalculate("2*2 - 3");
   my_assert(res == 1);

   res = calc.ParseAndCalculate("(2*2 - 3)");
   my_assert(res == 1);

   res = calc.ParseAndCalculate("1+(2*2 - 3)");
   my_assert(res == 2);

   res = calc.ParseAndCalculate("88 - 9*(12-4)");
   my_assert(res == 16);

   res = calc.ParseAndCalculate(" 1 + (2 * 2 - 3)");
   my_assert(res == 2);

   res = calc.ParseAndCalculate(" (1) + 2");
   my_assert(res == 3);
  
   res = calc.ParseAndCalculate("-1 + 2");
   my_assert(res == 1);

   res = calc.ParseAndCalculate("+1 + 2");
   my_assert(res == 3);

   res = calc.ParseAndCalculate("- (1 + 2)");
   my_assert(res == -3);

   res = calc.ParseAndCalculate("- (1111111111 + 2)");
   my_assert(res == -1111111113);

   res = calc.ParseAndCalculate("- (11 * 12) * 13");
   my_assert(res == -1716);

   res = calc.ParseAndCalculate("(10 + 11) * (3 + 7)");
   my_assert(res == 210);

   res = calc.ParseAndCalculate("(10 + 11) * (-(3 + 7))");
   my_assert(res == -210);

   res = calc.ParseAndCalculate("1 * 2 * 3 * 4");
   my_assert(res == 24);

   res = calc.ParseAndCalculate("1 * 2 + 4");

   res = calc.ParseAndCalculate("-10 * 5");
   my_assert(res == -50);

   res = calc.ParseAndCalculate("10 * (-5)");
   my_assert(res == -50);

   res = calc.ParseAndCalculate("(-10) * (-5)");
   my_assert(res == 50);

   res = calc.ParseAndCalculate(" 11 222 ");
   my_assert(!calc.IsEmpty());

   res = calc.ParseAndCalculate("(8 - 1 + 3) * 6");
   my_assert(res == 60);

   res = calc.ParseAndCalculate("(8 - 1 + 3) * 6 - ((3 + 7) * 2)");
   my_assert(res == 40);

   res = calc.ParseAndCalculate("-( -( -1)))");
   my_assert(res == -1);

   try
   {
      res = calc.ParseAndCalculate("1 ++ 3)");
   }
   catch(const std::exception&)
   { }
}

void test()
{
   testWithParser<LLParser>();
   testWithParser<LRParser>();
}