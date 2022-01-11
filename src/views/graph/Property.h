#pragma once

#include <sigc++/signal.h>

#include <glibmm/ustring.h>
#include <glibmm/value.h>

#include <forward_list>
#include <unordered_map>

namespace GMixer {

class Property {
    using field = std::pair<GType, std::string>;
    using map = std::unordered_map<std::string, field>;
    friend class PropertyList;

    /* Human-readable name of the property */
    Glib::ustring name;

    /* Maps keys (property name) to values (properties) */
    map properties;

    /* If not dependent, will outlive any container containing it (that is, it is persistent) */
    bool dependent;

    /* Callback for when someone wants to update a field of the property 
     * Receives the information concerning the change (field name and values),
     * and returns whether it should update the field in the property DS. */
    sigc::signal<bool(Property*, const std::string&, const std::string&)> signal_property_update;

public:
    
    Property(Glib::ustring name, bool dependent = false)
    : name(name), dependent(dependent)
    {}

    void connect(sigc::slot<bool(Property*, const std::string&, const std::string&)> slot)
    {
        signal_property_update.connect(slot);
    }

    void addField(const std::string& name, const std::string& property, GType type);
    void updateField(const std::string& name, const std::string& new_value);
    GType getType(const std::string& name) const;
    const std::string getField(const std::string& name) const;
    const Glib::ustring getName() const { return name; };
};

class PropertyList {
    using list = std::forward_list<Property*>;

    public:
        PropertyList() = default;
        void destroy();
        void add(Property* val);

        list::const_iterator begin() const { return m_list.begin(); }
        list::const_iterator end() const   { return m_list.end(); }

    private:
        ~PropertyList() = default;
        list m_list;
};
}
