#include "pogXmlWriter.h"

#include "btypeXmlWriter.h"
#include "predWriter.h"

namespace Xml {

void pogXmlWriter::visitPog(const pog::pog& pog) {
  for (unsigned int i = 0; i < pog.typeInfos.size(); i++) {
    m_typeInfos[pog.typeInfos[i]] = i;
  }
  m_printer->OpenElement("Proof_Obligations");
  for (const auto& define : pog.defines) {
    define.accept(*this);
  }
  for (const auto& poGroup : pog.pos) {
    poGroup.accept(*this);
  }
  m_printer->OpenElement("TypeInfos");
  Xml::BTypeWriter typeWriter(m_printer);
  for (unsigned int i = 0; i < pog.typeInfos.size(); i++) {
    m_printer->OpenElement("Type");
    m_printer->PushAttribute("id", std::to_string(i).c_str());
    pog.typeInfos.at(i).accept(typeWriter);
    m_printer->CloseElement();  // Type
  }
  m_printer->CloseElement();  // TypeInfos
  m_printer->CloseElement();  // Binary_Pred
}

void pogXmlWriter::visitDefine(const pog::Define& define) {
  m_printer->OpenElement("Define");
  m_printer->PushAttribute("name", define.name.c_str());
  if (define.hash != 0) {
    m_printer->PushAttribute("hash", std::to_string(define.hash).c_str());
  }
  for (const auto& content : define.contents) {
    if (std::holds_alternative<pog::Set>(content)) {
      std::get<pog::Set>(content).accept(*this);
    } else {
      Xml::writePredicate(*m_printer, m_typeInfos, std::get<Pred>(content));
    }
  }
  m_printer->CloseElement();  // Define
}

void pogXmlWriter::visitPOGroup(const pog::POGroup& poGroup) {
  m_printer->OpenElement("Proof_Obligation");
  if (poGroup.goalHash != 0) {
    m_printer->PushAttribute("goalHash",
                             std::to_string(poGroup.goalHash).c_str());
  }
  m_printer->OpenElement("Tag");
  m_printer->PushText(poGroup.tag.c_str());
  m_printer->CloseElement();  // Tag
  for (const auto& def : poGroup.definitions) {
    m_printer->OpenElement("Definition");
    m_printer->PushAttribute("name", def.c_str());
    m_printer->CloseElement();  // Definition
  }
  for (const auto& hyp : poGroup.hyps) {
    m_printer->OpenElement("Hypothesis");
    Xml::writePredicate(*m_printer, m_typeInfos, hyp);
    m_printer->CloseElement();  // Hypothesis
  }
  for (const auto& localHyp : poGroup.localHyps) {
    m_printer->OpenElement("Local_Hyp");
    Xml::writePredicate(*m_printer, m_typeInfos, localHyp);
    m_printer->CloseElement();  // Local_Hyp
  }
  for (const auto& po : poGroup.simpleGoals) {
    po.accept(*this);
  }
  m_printer->CloseElement();  // Proof_Obligation
}

void pogXmlWriter::visitPO(const pog::PO& po) {
  m_printer->OpenElement("Simple_Goal");
  m_printer->OpenElement("Tag");
  m_printer->PushText(po.tag.c_str());
  m_printer->CloseElement();  // Tag
  for (const auto& ref : po.localHypsRef) {
    m_printer->OpenElement("Ref_Hyp");
    m_printer->PushAttribute("num", std::to_string(ref).c_str());
    m_printer->CloseElement();  // Ref_Hyp
  }
  m_printer->OpenElement("Goal");
  writePredicate(*m_printer, m_typeInfos, po.goal);
  m_printer->CloseElement();  // Goal
  m_printer->CloseElement();  // Simple_Goal
}

void pogXmlWriter::visitSet(const pog::Set& set) {
  m_printer->OpenElement("Set");
  m_printer->OpenElement("Id");
  m_printer->PushAttribute("value", set.setName.name.prefix().c_str());
  m_printer->CloseElement();  // Id
  if (!set.elts.empty()) {
    m_printer->OpenElement("Enumerated_Values");
    for (const auto& elt : set.elts) {
      m_printer->OpenElement("Id");
      m_printer->PushAttribute("value", elt.name.prefix().c_str());
      m_printer->CloseElement();  // Id
    }
    m_printer->CloseElement();  // Enumerated_Values
  }
  m_printer->CloseElement();  // Set
}

}  // namespace Xml