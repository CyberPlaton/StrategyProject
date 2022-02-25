#include "XML.h"

bool cherrysoda::XMLUtil::ReadXMLFile(cherrysoda::XML::XMLDocument& doc, const cherrysoda::String& filename)
{
	cherrysoda::XML::XMLError result = doc.LoadFile(filename.c_str());
	if (result != cherrysoda::XML::XMLError::XML_SUCCESS)
	{
		// Clean up.
		doc.Clear();
		return false;
	}

	return true;
}
