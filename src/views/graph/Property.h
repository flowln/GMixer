#pragma once

#include <glibmm/ustring.h>
#include <glibmm/value.h>

#include <forward_list>
#include <unordered_map>

namespace GMixer {
struct Property {
    using field = std::pair<GType, std::string>;
    using map = std::unordered_map<std::string, field>;

    /* Human-readable name of the property */
    Glib::ustring name;

    /* Maps keys (property name) to values (properties) */
    map properties;

    /* If not dependent, will outlive any container containing it (that is, it is persistent) */
    bool          dependent;

    Property(Glib::ustring name, bool dependent = false)
    : name(name), dependent(dependent)
    {}

    void addField(const std::string& name, const std::string& property, GType type){
        field pair = {type, property};
        
        properties.insert({name, pair});
    }

    GType getType(const std::string& name){
        try{
            return properties.at(name).first;
        } catch(const std::out_of_range& oor){
            return {};
        }
    }

    const std::string getField(const std::string& name){
        try{
            return properties.at(name).second;
        } catch(const std::out_of_range& oor){
            return "";
        }
    }

};

class PropertyList {
    using list = std::forward_list<Property*>;

    public:
        PropertyList() = default;
        ~PropertyList() { for(auto val : m_list) if(val->dependent) delete val; }
        void add(Property* val){
            m_list.push_front(val);
        }

        list::const_iterator begin() const { return m_list.begin(); }
        list::const_iterator end() const   { return m_list.end(); }

    private:
        list m_list;
};
}
