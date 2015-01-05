#include "NodeUnaryOpPlus.h"
#include "CompilerState.h"

namespace MFM {

  NodeUnaryOpPlus::NodeUnaryOpPlus(Node * n, CompilerState & state): NodeUnaryOp(n, state) {}

  NodeUnaryOpPlus::~NodeUnaryOpPlus(){}


  const char * NodeUnaryOpPlus::getName()
  {
    return "+";
  }


  const std::string NodeUnaryOpPlus::prettyNodeName()
  {
    return nodeName(__PRETTY_FUNCTION__);
  }


  UTI NodeUnaryOpPlus::checkAndLabelType()
  {
    assert(m_node);
    UTI ut = m_node->checkAndLabelType();
    UTI newType = ut;         // init to stay the same

    if(!m_state.isScalar(ut)) //array unsupported at this time
      {
	std::ostringstream msg;
	msg << "Incompatible (nonscalar) type: <" << m_state.getUlamTypeNameByIndex(ut).c_str() << "> for unary operator" << getName();
	MSG(getNodeLocationAsString().c_str(), msg.str().c_str(), ERR);
	newType = Nav;
      }
    else
      {
	ULAMTYPE eut = m_state.getUlamTypeByIndex(ut)->getUlamTypeEnum();
	// implicit cast for Bool only
	if(eut == Bool)
	  {
	    newType = Int;
	    m_node = makeCastingNode(m_node, newType);  //insert node/s
	  }
	else if(eut == Bits)
	  {
	    std::ostringstream msg;
	    msg << "Unary operator" << getName() << " applied to type <" << m_state.getUlamTypeNameByIndex(ut).c_str() << "> is not defined";
	    MSG(getNodeLocationAsString().c_str(), msg.str().c_str(), ERR);
	    newType = Nav;
	  }
	//else maintain type for NOOP (e.g. Unary, Unsigned, Int)
      }

    setNodeType(newType);

    setStoreIntoAble(false);

    return newType;
  } //checkAndLabelType


  UlamValue NodeUnaryOpPlus::makeImmediateUnaryOp(UTI type, u32 data, u32 len)
  {
    return UlamValue::makeImmediate(type, data, len); //no change
  }


  void NodeUnaryOpPlus::genCode(File * fp, UlamValue& uvpass)
  {
    assert(m_node);
    m_node->genCode(fp, uvpass);
    //essentially a no-op
    return;
  } //genCode


  void NodeUnaryOpPlus::genCodeToStoreInto(File * fp, UlamValue& uvpass)
  {
    assert(m_node);
    m_node->genCodeToStoreInto(fp, uvpass);
    //essentially a no-op ?
    return;
  } //genCodeToStoreInto

} //end MFM
