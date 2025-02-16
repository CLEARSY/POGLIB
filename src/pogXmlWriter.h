#ifndef POGWRITER_H
#define POGWRITER_H

#include <iostream>
#include <map>

#include "pog.h"
#include "predWriter.h"
#include "tinyxml2.h"

namespace Xml {

class pogXmlWriter : public pog::pogVisitor {
 private:
  tinyxml2::XMLPrinter* m_printer;
  std::map<BType, unsigned int> m_typeInfos;

 public:
  explicit pogXmlWriter(tinyxml2::XMLPrinter* printer)
      : m_printer(printer), m_typeInfos() {}

  void visitPog(const pog::pog& pog) override;
  void visitDefine(const pog::Define& define) override;
  void visitPOGroup(const pog::POGroup& poGroup) override;
  void visitPO(const pog::PO& po) override;
  void visitSet(const pog::Set& set) override;
};

}  // namespace Xml

#endif  // POGWRITER_H
