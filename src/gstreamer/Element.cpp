#include "gstreamer/Element.h"

Element::Element(const gchar* name) : Element(gst_element_factory_create(gst_element_factory_find(name), name)) {}

Element::Element(GstElement* element) : m_element(element) {}

Element::~Element()
{
    for (auto prop : m_properties)
        delete prop;

    GstObject* parent;
    if ((parent = gst_element_get_parent(m_element))) {
        gst_bin_remove(GST_BIN(parent), m_element);
    }
}

GParamSpec** Element::getProps(guint* num_props) const
{
    auto props = g_object_class_list_properties(G_OBJECT_GET_CLASS(m_element), num_props);
    return props;
}

std::vector<Element::Property*>& Element::getProperties()
{
    guint n;
    auto props = getProps(&n);
    if (m_properties.size() == n) {
        g_free(props);
        return m_properties;
    }

    for (guint i = 0; i < n; i++) {
        m_properties.push_back(new Property(this, props[i]));
    }

    g_free(props);
    return m_properties;
}

const gchar* Element::getName() const
{
    return gst_element_get_name(m_element);
}

GList* Element::getSinks() const
{
    return GST_ELEMENT_CAST(m_element)->srcpads;
}

GList* Element::getSources() const
{
    return GST_ELEMENT_CAST(m_element)->sinkpads;
}

int Element::isPadLinked(GstPad* pad)
{
    return gst_pad_is_linked(pad);
}

// Property
Element::Property::Property(Element* parent, GParamSpec* spec)
    : parent(parent)
    , name(g_param_spec_get_name(spec))
    , description(g_param_spec_get_blurb(spec))
    , default_value{ toFundamental(g_param_spec_get_default_value(spec)) }
{}

Element::Property::type Element::Property::toFundamental(const GValue* value)
{
    gtype = G_VALUE_TYPE(value);

    if (g_type_is_a(gtype, G_TYPE_STRING)) {
        auto str = g_value_get_string(value);
        if (str)
            return std::string(str);
    } else if (g_type_is_a(gtype, G_TYPE_INT)) {
        return g_value_get_int(value);
    } else if (g_type_is_a(gtype, G_TYPE_INT64)) {
        return g_value_get_int64(value);
    } else if (g_type_is_a(gtype, G_TYPE_UINT)) {
        return g_value_get_uint(value);
    } else if (g_type_is_a(gtype, G_TYPE_UINT64)) {
        return g_value_get_uint64(value);
    } else if (g_type_is_a(gtype, G_TYPE_FLOAT)) {
        return g_value_get_float(value);
    } else if (g_type_is_a(gtype, G_TYPE_DOUBLE)) {
        return g_value_get_double(value);
    } else if (g_type_is_a(gtype, G_TYPE_BOOLEAN)) {
        return g_value_get_boolean(value);
    }

    return "NULL";
}

const std::string Element::Property::getValueAsString(ValueType type) const
{
    return std::visit(
        [](auto var) -> std::string {
            using T = std::decay_t<decltype(var)>;
            if constexpr (!std::is_same_v<T, std::string>)
                return std::to_string(var);
            else
                return var;
            return "";
        },
        type == ValueType::Current ? value : default_value);
}

const std::string Element::Property::getTypeAsString() const
{
    return g_type_name(gtype);
}

/** Compares two c strings, case-insensitive.
 *  Only returns true/false for whether they are equal or not.*/
inline bool compareCaseInsensitive(const char* s1, const char* s2)
{
    int i = 0;
    while (s1[i] && s2[i]) {
        if (tolower(s1[i]) != tolower(s2[i]))
            return false;
    }
    return true;
}

void Element::Property::setValue(std::string&& repr)
{
    if (g_type_is_a(gtype, G_TYPE_STRING)) {
        _setValue<std::string>(repr);
    } else if (g_type_is_a(gtype, G_TYPE_INT)) {
        _setValue<int32_t>(std::stol(repr));
    } else if (g_type_is_a(gtype, G_TYPE_UINT)) {
        _setValue<uint32_t>(std::stoul(repr));
    } else if (g_type_is_a(gtype, G_TYPE_INT64)) {
        _setValue<int64_t>(std::stoll(repr));
    } else if (g_type_is_a(gtype, G_TYPE_UINT64)) {
        _setValue<uint64_t>(std::stoull(repr));
    } else if (g_type_is_a(gtype, G_TYPE_FLOAT)) {
        _setValue<float>(std::stof(repr));
    } else if (g_type_is_a(gtype, G_TYPE_DOUBLE)) {
        _setValue<double>(std::stod(repr));
    } else if (g_type_is_a(gtype, G_TYPE_BOOLEAN)) {
        _setValue<bool>(compareCaseInsensitive(repr.c_str(), "true") || repr != "0");
    } else {
        printf("Name: %s, Type: %s, Repr: %s\n", name.c_str(), g_type_name(gtype), repr.c_str());
    }
}
