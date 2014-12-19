#include "TestCase_EndToEndCompiler.h"

namespace MFM {

  BEGINTESTCASECOMPILER(t3256_test_compiler_funccalltypedefarg_issue)
  {
    std::string GetAnswerKey()
    {
      //Ue_TypedefIssue { typedef Int(3) Symmetry;  Int(32) test() {  TypedefIssue t;  t ( 0 cast )set . 0 cast return } }\nExit status: 0

      return std::string("Ue_TypedefIssue { typedef Int(3) Symmetry;  Int(3) m(0);  Int(32) test() {  TypedefIssue t;  t ( 0 cast )set . t ( 1 cast )set . t m . cast return } }\nExit status: 1");
    }

    std::string PresetTest(FileManagerString * fms)
    {

      /*
      ulam 1;
      use Empty;

      element TypedefIssue {
	// Types
	typedef Int(3) Symmetry;

	Void set(Symmetry vector) { }
	Int test() {
	  TypedefIssue t;
	  t.set((Symmetry) 0);  // line 11
	  return 0;
	}
      }
*/

      // leak parseRestOfMemberSelectExpr ?? error using 'Symmetry' before defined.
      //bool rtn1 = fms->add("TypedefIssue.ulam","ulam 1;\n element TypedefIssue {\nSymmetry m;\n  // Types\n typedef Int(3) Symmetry;\nVoid set(Symmetry vector) {m=vector;\n }\nInt test() {\nTypedefIssue t;\n t.set((Symmetry) 0);  // line 11\n t.set((Int(3)) 1); return t.m;\n}\n}\n");

      // Int(3) version:
      //bool rtn1 = fms->add("TypedefIssue.ulam","ulam 1;\n element TypedefIssue {\n // Types\n typedef Int(3) Symmetry;\n Symmetry m;\n Void set(Symmetry vector) {m=vector;\n }\nInt test() {\nTypedefIssue t;\n t.set((Symmetry) 0);  // line 11\n t.set((Int(3)) 1); return t.m;\n}\n}\n");

      // Unsigned(3) version:
      bool rtn1 = fms->add("TypedefIssue.ulam","ulam 1;\n element TypedefIssue {\n // Types\n typedef Unsigned(3) Symmetry;\n Symmetry m;\n Void set(Symmetry vector) {m=vector;\n }\nInt test() {\nTypedefIssue t;\n t.set((Symmetry) 0);  // line 11\n t.set((Unsigned(3)) 1); return t.m;\n}\n}\n");

      if(rtn1)
	return std::string("TypedefIssue.ulam");

      return std::string("");
    }
  }

  ENDTESTCASECOMPILER(t3256_test_compiler_funccalltypedefarg_issue)

} //end MFM


