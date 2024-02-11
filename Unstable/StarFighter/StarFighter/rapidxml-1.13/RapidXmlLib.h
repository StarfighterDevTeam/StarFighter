#pragma once
#ifndef _RAPID_XML_LIB_H
#define _RAPID_XML_LIB_H

#include "rapidxml.hpp"

// Predeclaration needed since C++20
namespace rapidxml {
	namespace internal {
		template<class OutIt, class Ch>
        inline OutIt print_data_node(OutIt out, const xml_node<Ch> *node, int flags, int indent);

        template<class OutIt, class Ch>
        inline OutIt print_cdata_node(OutIt out, const xml_node<Ch> *node, int flags, int indent);

		template<class OutIt, class Ch>
        inline OutIt print_children(OutIt out, const xml_node<Ch> *node, int flags, int indent);

		template<class OutIt, class Ch>
        inline OutIt print_element_node(OutIt out, const xml_node<Ch> *node, int flags, int indent);

		template<class OutIt, class Ch>
        inline OutIt print_declaration_node(OutIt out, const xml_node<Ch> *node, int flags, int indent);

		template<class OutIt, class Ch>
        inline OutIt print_comment_node(OutIt out, const xml_node<Ch> *node, int flags, int indent);

		template<class OutIt, class Ch>
        inline OutIt print_doctype_node(OutIt out, const xml_node<Ch> *node, int flags, int indent);

		template<class OutIt, class Ch>
        inline OutIt print_pi_node(OutIt out, const xml_node<Ch> *node, int flags, int indent);
	}

	static inline void appendXmlAttribute(xml_document<>& doc, xml_node<>*pNode, const char* attrName, int attrValue)
	{
		pNode->append_attribute(doc.allocate_attribute(attrName, doc.allocate_string(std::to_string(attrValue).c_str())));
	};

	static inline void appendXmlAttribute_Str(xml_document<>& doc, xml_node<>*pNode, const char* attrName, const char* attrValue)
	{
		pNode->append_attribute(doc.allocate_attribute(attrName, doc.allocate_string(attrValue)));
	};

	static inline void writeIntValue(xml_document<>&doc, xml_node<>* parentNode, const char* nodeName, int attrValue)
	{
		xml_node<>* pNode = doc.allocate_node(node_element, nodeName);
		parentNode->append_node(pNode);
		appendXmlAttribute(doc, pNode, "value", attrValue);
	};

	static inline void writeBoolValue(xml_document<>& doc, xml_node<>* parentNode, const char* nodeName, bool attrValue)
	{
		xml_node<>* pNode = doc.allocate_node(node_element, nodeName);
		parentNode->append_node(pNode);
		appendXmlAttribute(doc, pNode, "value", (int)attrValue);
	};

	static inline void writeFloatValue(xml_document<>& doc, xml_node<>* parentNode, const char* nodeName, float attrValue)
	{
		xml_node<>* pNode = doc.allocate_node(node_element, nodeName);
		parentNode->append_node(pNode);
		appendXmlAttribute(doc, pNode, "value", attrValue);
	};

	static inline void readIntValue(xml_document<>& doc, const xml_node<>* parentNode, const char* nodeName, int& outAttrValue)
	{
		xml_node<>* pNode = parentNode->first_node(nodeName);
		if (pNode)
			outAttrValue = std::stoi(pNode->first_attribute("value")->value());
	};

	static inline void readBoolValue(xml_document<>& doc, const xml_node<>* parentNode, const char* nodeName, bool& outAttrValue)
	{
		xml_node<>* pNode = parentNode->first_node(nodeName);
		if (pNode)
			outAttrValue = (0 != std::stoi(pNode->first_attribute("value")->value()));
	};

	static inline void readFloatValue(xml_document<>& doc, const xml_node<>* parentNode, const char* nodeName, float& outAttrValue)
	{
		xml_node<>* pNode = parentNode->first_node(nodeName);
		if (pNode)
			outAttrValue = std::stof(pNode->first_attribute("value")->value());
	};
}

#include "rapidxml_print.hpp"

#endif // _RAPID_XML_LIB_H
