#include <stdio.h>
#include "NodeBinaryOpLogical.h"
#include "CompilerState.h"

namespace MFM {

  NodeBinaryOpLogical::NodeBinaryOpLogical(Node * left, Node * right, CompilerState & state) : NodeBinaryOp(left, right, state) {}

  NodeBinaryOpLogical::NodeBinaryOpLogical(const NodeBinaryOpLogical& ref) : NodeBinaryOp(ref) {}

  NodeBinaryOpLogical::~NodeBinaryOpLogical()  {}

  // not used for logical op
  void NodeBinaryOpLogical::doBinaryOperation(s32 lslot, s32 rslot, u32 slots)
  {
    assert(0);
  } //dobinaryoperation

  UTI NodeBinaryOpLogical::calcNodeType(UTI lt, UTI rt)  //logical
  {
    if(lt == Nav || rt == Nav)
      {
	return Nav;
      }

    UTI newType = Nav; //init
    // all logical operations are performed as Bool.BITSPERBOOL.-1
    if(NodeBinaryOp::checkScalarTypesOnly(lt, rt))
      {
	s32 maxbs = 1;
	//check for Bool, or safe Non-Bool to Bool casting cases:
	if(!(checkNonBoolToBoolCastAndMaxsize(lt, maxbs) && checkNonBoolToBoolCastAndMaxsize(rt, maxbs)))
	  {
	    std::ostringstream msg;
	    msg << "Bool is the supported type for logical operator";
	    msg << getName() << "; Suggest casting ";
	    msg << m_state.getUlamTypeNameByIndex(lt).c_str() << " and ";
	    msg << m_state.getUlamTypeNameByIndex(rt).c_str();
	    msg << " to Bool";
	    if(maxbs > 1)
	      msg << "(" << maxbs << ")";
	    MSG(getNodeLocationAsString().c_str(), msg.str().c_str(), ERR);
	  }
	else
	  {
	    //both bool. ok to cast. use larger bool bitsize.
	    UlamKeyTypeSignature newkey(m_state.m_pool.getIndexForDataString("Bool"), maxbs);
	    newType = m_state.makeUlamType(newkey, Bool);
	  }
      } //both scalars
    return newType;
  } //calcNodeType

  bool NodeBinaryOpLogical::checkNonBoolToBoolCastAndMaxsize(UTI uti, s32& maxbitsize)
  {
    bool rtnOK = false;
    ULAMTYPE typEnum = m_state.getUlamTypeByIndex(uti)->getUlamTypeEnum();
    s32 bs = m_state.getBitSize(uti);
    if((typEnum == Bool) || ((bs == 1) && (typEnum == Unsigned || typEnum == Unary)))
      {
	rtnOK = true;
	maxbitsize = (bs > maxbitsize ? bs : maxbitsize);
      }
    return rtnOK;
  } //checkNonBoolToBoolCastAndMaxsize

  const std::string NodeBinaryOpLogical::methodNameForCodeGen()
  {
    assert(0);
    return "notapplicable_logicalops";
  } // methodNameForCodeGen

} //end MFM
