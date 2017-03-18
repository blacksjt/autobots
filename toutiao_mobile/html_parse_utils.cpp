#include "html_parse_utils.h"

HtmlParseUtils::HtmlParseUtils( const char* buffer, size_t buffer_length )
{
    output = gumbo_parse_with_options(&kGumboDefaultOptions, 
        buffer, buffer_length);
}

HtmlParseUtils::~HtmlParseUtils()
{
    gumbo_destroy_output(&kGumboDefaultOptions, output);
}

bool HtmlParseUtils::HasAttribute( GumboNode* node, const char* attr_name )
{
    GumboAttribute* cls_attr = gumbo_get_attribute(&node->v.element.attributes, attr_name);
    if (cls_attr != NULL)
    {
        return true;
    }

    return false;
}

bool HtmlParseUtils::HasAttributeValue( GumboNode* node, const char* attr_name, const char* attr_value )
{
    GumboAttribute* cls_attr = gumbo_get_attribute(&node->v.element.attributes, attr_name);
    if (cls_attr != NULL && strstr(cls_attr->value, attr_value) != NULL)
    {
        return true;
    }

    return false;
}

std::string HtmlParseUtils::GetAttributeValue( GumboNode* node, const char* attr_name )
{
    GumboAttribute* cls_attr = gumbo_get_attribute(&node->v.element.attributes, attr_name);
    if (cls_attr != NULL)
    {
        return std::string(cls_attr->value);
    }

    return std::string("");
}

GumboNode* HtmlParseUtils::FirstElementNode( GumboNode* node, const char* attr_name, const char* attr_value )
{
    if (node == NULL || node->type != GUMBO_NODE_ELEMENT) 
    {
        return NULL;
    }

    GumboAttribute* cls_attr;
    if (HasAttributeValue(node, attr_name, attr_value))
    {
        return node;
    }

    GumboVector* children = &node->v.element.children;
    for (int i = 0; i < children->length; ++i) 
    {
        GumboNode* child_node = FirstElementNode(static_cast<GumboNode*>(children->data[i]),attr_name,attr_value);
        if (child_node != NULL)
        {
            return child_node;
        }
    }

    return NULL;
}

GumboNode* HtmlParseUtils::FirstElementNode( const char* attr_name, const char* attr_value )
{
    if (output == NULL)
    {
        return NULL;
    }

    return FirstElementNode(output->root,attr_name,attr_value);
}
