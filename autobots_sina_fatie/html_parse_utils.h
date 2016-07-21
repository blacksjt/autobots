#ifndef HTML_PARSE_UTILS_H
#define HTML_PARSE_UTILS_H

#include <string>
#include <gumbo.h>

//html����������
class HtmlParseUtils
{
public:
    HtmlParseUtils(const char* buffer, size_t buffer_length);
    ~HtmlParseUtils();

    GumboNode* GetRootNode() { return output->root;}

public:
    static bool HasAttribute(GumboNode* node, const char* attr_name);

    static bool HasAttributeValue(GumboNode* node, const char* attr_name, const char* attr_value);

    static std::string GetAttributeValue(GumboNode* node, const char* attr_name);

    //�����ƶ�������������ֵ�ýڵ�
    static GumboNode* FirstElementNode(GumboNode* node, const char* attr_name, const char* attr_value);
    GumboNode* FirstElementNode(const char* attr_name, const char* attr_value);

private:
    GumboOutput* output;
};


#endif // HTML_PARSE_UTILS_H