#ifndef _CHERRYSODA_UTIL_XML_H_
#define _CHERRYSODA_UTIL_XML_H_
#pragma once

#include <CherrySoda/Util/String.h>

#include <tinyxml2.h>

namespace cherrysoda
{
	namespace XML
	{
		using namespace tinyxml2;

	} // namespace tinyxml2


	class XMLUtil
	{
	public:
		static bool ReadXMLFile(cherrysoda::XML::XMLDocument& doc, const String& filename);
	};

} // namespace cherrysoda


#endif // _CHERRYSODA_UTIL_XML_H_
