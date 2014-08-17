#include <sstream>
#include <string.h>
#include "CompilerState.h"
#include "Symbol.h"

namespace MFM {

  Symbol::Symbol(u32 id, UlamType * utype) : m_id(id), m_utype(utype), m_dataMember(false){}
  Symbol::~Symbol(){}

  u32 Symbol::getId()
  {
    return m_id;
  }


  UlamType * Symbol::getUlamType()
  {
    return m_utype;
  }


  bool Symbol::isFunction()
  {
    return false;
  }


  bool Symbol::isTypedef()
  {
    return false;
  }


  void Symbol::setDataMember()
  {
    m_dataMember = true;
  }


  bool Symbol::isDataMember()
  {
    return m_dataMember;
  }


  const std::string Symbol::getMangledName(CompilerState& state)
  {
       std::ostringstream mangled;
       mangled << getMangledPrefix() << state.m_pool.getDataAsString(getId()).c_str();
       return mangled.str();
  }

} //end MFM
