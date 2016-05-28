#include <stdio.h>
#include "NodeConditionalIs.h"
#include "CompilerState.h"

namespace MFM {

  NodeConditionalIs::NodeConditionalIs(Node * leftNode, NodeTypeDescriptor * classType, CompilerState & state): NodeConditional(leftNode, classType, state) {}

  NodeConditionalIs::NodeConditionalIs(const NodeConditionalIs& ref) : NodeConditional(ref) {}

  NodeConditionalIs::~NodeConditionalIs() {}

  Node * NodeConditionalIs::instantiate()
  {
    return new NodeConditionalIs(*this);
  }

  UTI NodeConditionalIs::checkAndLabelType()
  {
    assert(m_nodeLeft);
    UTI newType = Bool;  //except for 'Has'

    UTI luti = m_nodeLeft->checkAndLabelType();  //side-effect
    if(luti == Nav)
      {
	std::ostringstream msg;
	msg << "Invalid lefthand type of conditional operator '" << getName();
	msg << "'";
	MSG(getNodeLocationAsString().c_str(), msg.str().c_str(), ERR);
	setNodeType(Nav);
	return Nav; //short-circuit
      }

    if(luti == Hzy)
      {
	std::ostringstream msg;
	msg << "Incomplete lefthand type of conditional operator '" << getName();
	msg << "'";
	MSG(getNodeLocationAsString().c_str(), msg.str().c_str(), DEBUG);
	setNodeType(Hzy);
	m_state.setGoAgain();
	return Hzy; //short-circuit
      }

    UlamType * lut = m_state.getUlamTypeByIndex(luti);
    ULAMCLASSTYPE lclasstype = lut->getUlamClassType();
    ULAMTYPE letyp = lut->getUlamTypeEnum();
    if(!((m_state.isAtom(luti) || (letyp == Class)) && lut->isScalar()))
      {
	std::ostringstream msg;
	msg << "Invalid lefthand type of conditional operator '" << getName();
	msg << "'; must be an atom or a class, not type: ";
	msg << lut->getUlamTypeNameBrief().c_str();
	if((lclasstype == UC_UNSEEN) || (luti == Hzy))
	  {
	    MSG(getNodeLocationAsString().c_str(), msg.str().c_str(), DEBUG);
	    newType = Hzy;
	    m_state.setGoAgain();
	  }
	else
	  {
	    MSG(getNodeLocationAsString().c_str(), msg.str().c_str(), ERR);
	    newType = Nav;
	  }
      }

    assert(m_nodeTypeDesc);
    UTI ruti = m_nodeTypeDesc->checkAndLabelType();
    if(m_state.okUTItoContinue(ruti))
      {
	UlamType * rut = m_state.getUlamTypeByIndex(ruti);
	//rhs cannot be a ref type
	if(rut->isReference())
	  {
	    std::ostringstream msg;
	    msg << "Invalid righthand type of conditional operator '" << getName();
	    msg << "'; must be a class type, not a reference: ";
	    msg << rut->getUlamTypeNameBrief().c_str();
	    MSG(getNodeLocationAsString().c_str(), msg.str().c_str(), ERR);
	    newType = Nav;
	  }
	else
	  {
	    //rhs is allowed to be a quark due to inheritance.
	    ULAMTYPE retyp = rut->getUlamTypeEnum();
	    if(!((retyp == Class) && rut->isScalar()))
	      {
		std::ostringstream msg;
		msg << "Invalid righthand type of conditional operator '" << getName();
		msg << "'; must be a class name, not type: ";
		msg << rut->getUlamTypeNameBrief().c_str();
		MSG(getNodeLocationAsString().c_str(), msg.str().c_str(), ERR);
		newType = Nav;
	      }
	  }
      }

    if(m_state.okUTItoContinue(newType) && !m_state.isComplete(ruti))
      {
	std::ostringstream msg;
	msg << "Righthand type of conditional operator '" << getName() << "' ";
	msg << m_state.getUlamTypeNameBriefByIndex(ruti).c_str();
	msg << " is still incomplete while labeling class: ";
	msg << m_state.getUlamTypeNameBriefByIndex(m_state.getCompileThisIdx()).c_str();
	MSG(getNodeLocationAsString().c_str(), msg.str().c_str(), DEBUG);
	newType = Hzy; //goagain set by nodetypedesc
	m_state.setGoAgain();
      }
    setNodeType(newType);
    Node::setStoreIntoAble(TBOOL_FALSE);
    return getNodeType();
  } //checkAndLabelType

  const char * NodeConditionalIs::getName()
  {
    return "is";
  }

  const std::string NodeConditionalIs::prettyNodeName()
  {
    return nodeName(__PRETTY_FUNCTION__);
  }

  const std::string NodeConditionalIs::methodNameForCodeGen()
  {
    assert(m_nodeLeft);
    return  std::string(m_state.getIsMangledFunctionName(m_nodeLeft->getNodeType()));
  }

  EvalStatus  NodeConditionalIs::eval()
  {
    assert(m_nodeLeft);

    UTI nuti = getNodeType();
    if(nuti == Nav)
      return ERROR;

    if(nuti == Hzy)
      return NOTREADY;

    evalNodeProlog(0);   //new current frame pointer

    makeRoomForSlots(1); //always 1 slot for ptr
    EvalStatus evs = m_nodeLeft->evalToStoreInto();
    if(evs != NORMAL)
      {
	evalNodeEpilog();
	return evs;
      }

    UlamValue pluv = m_state.m_nodeEvalStack.loadUlamValuePtrFromSlot(1);

    // DO 'IS':
    UTI luti = pluv.getUlamValueTypeIdx();
    assert(m_state.isPtr(luti));
    luti = pluv.getPtrTargetType();
    UTI ruti = getRightType();

    UTI derefluti = m_state.getUlamTypeAsDeref(luti);
    // was inclusive result for eval purposes (atoms and element types are orthogonal)
    // now optional for debugging
#define _LET_ATOM_IS_ELEMENT
#ifndef _LET_ATOM_IS_ELEMENT
    if(m_state.isAtom(luti))
      {
	evalNodeEpilog();
	return UNEVALUABLE;
      }
    bool isit = ((UlamType::compare(derefluti,ruti,m_state) == UTIC_SAME) || m_state.isClassASubclassOf(derefluti, ruti));

#else

    bool isit = (m_state.isAtom(luti) || (UlamType::compare(derefluti,ruti,m_state) == UTIC_SAME) || m_state.isClassASubclassOf(derefluti, ruti));

#endif

    UlamValue rtnuv = UlamValue::makeImmediate(nuti, (u32) isit, m_state);

    //also copy result UV to stack, -1 relative to current frame pointer
    Node::assignReturnValueToStack(rtnuv);

    evalNodeEpilog();
    return evs;
  } //eval

  void NodeConditionalIs::genCode(File * fp, UVPass& uvpass)
  {
    assert(m_nodeLeft);
    UTI lnuti = m_nodeLeft->getNodeType();
    if(m_state.isAtom(lnuti))
      return genCodeAtomIs(fp, uvpass); //reads into tmpvar
    else if(m_state.isReference(lnuti))
      return genCodeReferenceIs(fp, uvpass); //doesn't read into tmpvar
    //else ClassIs.. reads into tmp var.

    UTI nuti = getNodeType(); //Bool
    UlamType * nut = m_state.getUlamTypeByIndex(nuti);

    UVPass luvpass;
    m_nodeLeft->genCodeToStoreInto(fp, luvpass); //loads lhs into tmp (including array item)
    UTI luti = luvpass.getPassTargetType();

    UTI ruti = getRightType();
    UlamType * rut = m_state.getUlamTypeByIndex(ruti);
    assert(!rut->isReference());

    s32 tmpVarIs = m_state.getNextTmpVarNumber();

    m_state.indentUlamCode(fp);
    fp->write("const ");
    fp->write(nut->getTmpStorageTypeAsString().c_str()); //bool
    fp->write(" ");
    fp->write(m_state.getTmpVarAsString(nuti, tmpVarIs, TMPREGISTER).c_str());
    fp->write(" = ");

    //is a class
    fp->write(m_state.getEffectiveSelfMangledNameByIndex(luti).c_str());
    fp->write(".");
    fp->write(m_state.getIsMangledFunctionName(luti)); //UlamElement IsMethod
    fp->write("(&"); //one arg
    fp->write(m_state.getEffectiveSelfMangledNameByIndex(ruti).c_str());
    fp->write(");\n");

    //update uvpass
    uvpass = UVPass::makePass(tmpVarIs, TMPREGISTER, nuti, m_state.determinePackable(nuti), m_state, 0, 0); //POS 0 rightjustified (atom-based).

    m_state.m_currentObjSymbolsForCodeGen.clear();
  } //genCode

  void NodeConditionalIs::genCodeAtomIs(File * fp, UVPass & uvpass)
  {
    assert(m_nodeLeft);
    UTI nuti = getNodeType();
    UlamType * nut = m_state.getUlamTypeByIndex(nuti);

    UVPass luvpass;
    m_nodeLeft->genCode(fp, luvpass); //loads lhs into tmp (T)

    UTI luti = luvpass.getPassTargetType();
    assert(m_state.isAtom(luti)); //or Atomref

    UTI ruti = getRightType();
    UlamType * rut = m_state.getUlamTypeByIndex(ruti);
    assert(!rut->isReference());

    s32 tmpVarIs = m_state.getNextTmpVarNumber();

    m_state.indentUlamCode(fp);
    fp->write("const ");
    fp->write(nut->getTmpStorageTypeAsString().c_str()); //bool
    fp->write(" ");
    fp->write(m_state.getTmpVarAsString(nuti, tmpVarIs, TMPREGISTER).c_str());
    fp->write(" = ");

    if(rut->getUlamClassType() == UC_ELEMENT)
      {
	//reversed call to rhs' overloaded c-implemented 'Is' method;
	// using lhs' T as argument; required for EMPTY-ELEMENT special case
	fp->write(m_state.getEffectiveSelfMangledNameByIndex(ruti).c_str());
	fp->write(".");
	fp->write(m_state.getIsMangledFunctionName(ruti)); //UlamElement IsMethod
	fp->write("(");
	fp->write(luvpass.getTmpVarAsString(m_state).c_str()); //from tmpvar T or ABS
	fp->write(");\n");
      }
    else
      {
	fp->write(m_state.getIsMangledFunctionName(luti)); //UlamClass IsMethod
	fp->write("(uc, ");
	fp->write(luvpass.getTmpVarAsString(m_state).c_str());
	fp->write(".GetType(), &"); //from tmpvar T or ABS
	fp->write(m_state.getEffectiveSelfMangledNameByIndex(ruti).c_str());
	fp->write(");\n");
      }

    //update uvpass
    uvpass = UVPass::makePass(tmpVarIs, TMPREGISTER, nuti, m_state.determinePackable(nuti), m_state, 0, 0); //POS 0 rightjustified (atom-based).

    m_state.clearCurrentObjSymbolsForCodeGen(); //forgotten
  } //genCodeAtomIs

  void NodeConditionalIs::genCodeReferenceIs(File * fp, UVPass & uvpass)
  {
    assert(m_nodeLeft);
    UTI nuti = getNodeType();
    UlamType * nut = m_state.getUlamTypeByIndex(nuti);

    UVPass luvpass;
    m_nodeLeft->genCodeToStoreInto(fp, luvpass); //loads lhs into tmp (T)
    UTI luti = luvpass.getPassTargetType(); //replace
    assert(m_state.isReference(luti));

    Symbol * stgcos = NULL;
    if(m_state.m_currentObjSymbolsForCodeGen.empty())
      stgcos = m_state.getCurrentSelfSymbolForCodeGen();
    else
      stgcos = m_state.m_currentObjSymbolsForCodeGen.back();

    UTI ruti = getRightType();
    UlamType * rut = m_state.getUlamTypeByIndex(ruti);
    assert(!rut->isReference());

    s32 tmpVarIs = m_state.getNextTmpVarNumber();

    m_state.indentUlamCode(fp);
    fp->write("const ");
    fp->write(nut->getTmpStorageTypeAsString().c_str()); //bool
    fp->write(" ");
    fp->write(m_state.getTmpVarAsString(nuti, tmpVarIs, TMPREGISTER).c_str());
    fp->write(" = ");

    //what if array?
    fp->write(stgcos->getMangledName().c_str());
    fp->write(".GetEffectiveSelf()->");
    fp->write(m_state.getIsMangledFunctionName(luti)); //UlamClass IsMethod
    fp->write("(&");
    fp->write(m_state.getEffectiveSelfMangledNameByIndex(ruti).c_str());
    fp->write(");\n");

    //update uvpass
    uvpass = UVPass::makePass(tmpVarIs, TMPREGISTER, nuti, m_state.determinePackable(nuti), m_state, 0, 0); //POS 0 rightjustified (atom-based).

    m_state.clearCurrentObjSymbolsForCodeGen(); //forgotten (t3741)
  } //genCodeReferenceIs


} //end MFM
