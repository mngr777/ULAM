#include "TestCase_EndToEndParser.h"

namespace MFM {

  BEGINTESTCASEPARSER(t211_test_parser_leftassoc)
  {
    std::string GetAnswerKey()
    {
      return std::string(" { Int a(1);  Int test() {  a 1 1 -b 1 +b = a return } }\n");
    }
    
    std::string PresetTest(FileManagerString * fms)
    {
      bool rtn1 = fms->add("a.ulam","ulam { Int a; Int test() { a = 1 - 1 + 1; return a; } }"); // we want 1, not -1
      
      if(rtn1)
	return std::string("a.ulam");
      
      return std::string("");
    }      
  }
  
  ENDTESTCASEPARSER(t211_test_parser_leftassoc)
  
} //end MFM


