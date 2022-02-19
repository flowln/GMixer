#pragma once

#include <gst/gst.h>

#include <sigc++/slot.h>

#include <string>
#include <variant>

class Element {
   public:
    // Every GST element has zero or more properties,
    // each of which have different information on it,
    // such as the name of the property, it's default value, etc.
    // Those will be called fields.
    class Property {
       private:
        const Element* parent;

        /* Available types */
        using type = std::variant<std::string, int32_t, int64_t, uint32_t, uint64_t, float, double, bool>;

        const std::string name;        /* Name of the property (e.g. bitrate, volume, qol, etc). */
        const std::string description; /* Its description. */
        GType gtype;                   /* The GType associated with the property */

        const type default_value; /* The default value given by GST to the property */
        type value;               /* The currently set value of the property. */

        /* As a side-effect, this changes the type_name */
        type toFundamental(const GValue* value);

        template <typename T>
        constexpr T getDefaultValue() const noexcept(false)
        {
            return std::get<T>(default_value);
        }

        template <typename T>
        constexpr T getValue() const noexcept(false)
        {
            return std::get<T>(value);
        }

        template <typename T>
        void _setValue(T val)
        {
            value = val;

            // Send property update to gst
            g_object_set(parent->getBase(), getName().c_str(), getValue<T>(), NULL);
        }

       public:
        Property(Element*, GParamSpec*);

        const std::string& getName() const { return name; }
        const std::string& getDescription() const { return description; }

        enum class ValueType { Current, Default };
        const std::string getValueAsString(ValueType type = ValueType::Current) const;
        const std::string getTypeAsString() const;

        void setValue(std::string&& repr);
    };

    Element(const gchar* name);
    Element(GstElement*);
    ~Element();

    bool operator==(Element* elem) const { return m_element == elem->getBase(); };
    bool operator==(GstElement* elem) const { return m_element == elem; };

    const gchar* getName() const;
    GstElement* getBase() const { return m_element; }
    GstElement* getPeer() const { return GST_ELEMENT(gst_pad_get_parent(GST_PAD_PEER(m_element))); }

    std::vector<Property*>& getProperties();

    const GList* getSinks() const;
    const GList* getSources() const;
    const GList* getPadTemplates() const;

    static int isPadLinked(GstPad*);

   private:
    GstElement* m_element;

    GParamSpec** getProps(guint*) const;
    std::vector<Property*> m_properties;
};
