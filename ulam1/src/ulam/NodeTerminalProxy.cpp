#include <stdlib.h>
#include "NodeTerminalProxy.h"
#include "CompilerState.h"

namespace MFM {

  NodeTerminalProxy::NodeTerminalProxy(UTI memberType, Token funcTok, CompilerState & state) : NodeTerminal(UNKNOWNSIZE, state), m_uti(memberType), m_funcTok(funcTok)
  {
    Node::setNodeLocation(funcTok.m_locator);
  }

  NodeTerminalProxy::~NodeTerminalProxy() {}

  const std::string NodeTerminalProxy::prettyNodeName()
  {
    return nodeName(__PRETTY_FUNCTION__);
  }


  UTI NodeTerminalProxy::checkAndLabelType()
  {
    if(m_constant.sval == UNKNOWNSIZE)
      {
	m_state.constantFoldIncompleteUTI(m_uti); //update if possible
      }

    if(!m_state.isComplete(m_uti))
      {
	std::ostringstream msg;
	msg << "Proxy Type: " << m_state.getUlamTypeNameByIndex(m_uti).c_str() << " is still incomplete and unknown";
	MSG(getNodeLocationAsString().c_str(), msg.str().c_str(), INFO);
      }
    else
      {
	//depending on the of-func, update our constant
	if(!setConstantValue(m_funcTok))
	  {
	    std::ostringstream msg;
	    msg << "Unsupported request: '" << m_state.getTokenDataAsString(&m_funcTok).c_str() << "' of type: " << m_state.getUlamTypeNameByIndex(m_uti).c_str();
	    MSG(&m_funcTok, msg.str().c_str(), ERR);
	    setNodeType(Nav);
	  }
      }

    setConstantTypeForNode(m_funcTok); //enough info to set this constant node's type

    return getNodeType(); //updated to Unsigned, hopefully
  }  //checkandLabelType


    EvalStatus NodeTerminalProxy::eval()
  {
    EvalStatus evs = NORMAL; //init ok
    evalNodeProlog(0); //new current frame pointer

    UlamType * cut = m_state.getUlamTypeByIndex(m_uti);
    if(m_constant.sval == UNKNOWNSIZE)
      {
	if(!cut->isComplete())
	  m_state.constantFoldIncompleteUTI(m_uti); //update if possible

	if(m_constant.sval != UNKNOWNSIZE)
	  setConstantValue(m_funcTok);
      }

    UlamValue rtnUV;
    evs = NodeTerminal::makeTerminalValue(rtnUV);

    //copy result UV to stack, -1 relative to current frame pointer
    if(evs == NORMAL)
      Node::assignReturnValueToStack(rtnUV);

    evalNodeEpilog();
    return evs;
  } //eval


  bool NodeTerminalProxy::setConstantValue(Token tok)
  {
    bool rtnB = false;
    UlamType * cut = m_state.getUlamTypeByIndex(m_uti);
    assert(cut->isComplete());

    if(tok.m_dataindex == m_state.m_pool.getIndexForDataString("sizeof"))
      {
	m_constant.uval =  cut->getTotalBitSize(); //unsigned

	rtnB = true;
      }
    else if(tok.m_dataindex == m_state.m_pool.getIndexForDataString("maxof"))
      {
	if(cut->isMinMaxAllowed())
	  {
	    m_constant.uval = cut->getMax();
	    rtnB = true;
	  }
      }
    else if (tok.m_dataindex == m_state.m_pool.getIndexForDataString("minof"))
      {
	if(cut->isMinMaxAllowed())
	  {
	    m_constant.sval = cut->getMin();
	    rtnB = true;
	  }
      }
    return rtnB;
  } //setConstantValue


  //minof, maxof should be the same type as the original member token, unsigned for sizeof
  UTI NodeTerminalProxy::setConstantTypeForNode(Token tok)
  {
    UTI newType = Nav;  //init
    if(tok.m_dataindex == m_state.m_pool.getIndexForDataString("sizeof"))
	newType = m_state.getUlamTypeOfConstant(Unsigned);
    else
      {
	// not a class! since sizeof is only func that applies to classes
	ULAMTYPE etype = m_state.getUlamTypeByIndex(m_uti)->getUlamTypeEnum();
	newType = m_state.getUlamTypeOfConstant(etype);
      }

    setNodeType(newType);
    setStoreIntoAble(false);
    return newType;
  } //setConstantTypeForNode

} //end MFM
