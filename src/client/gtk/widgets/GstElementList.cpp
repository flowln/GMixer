#include "GstElementList.h"

#include "client/Client.h"

#include "client/common/Storage.h"

#include <gst/gst.h>

#include <glibmm/dispatcher.h>
#include <glibmm/property.h>

#include <giomm/liststore.h>

#include <gtkmm/expression.h>
#include <gtkmm/label.h>
#include <gtkmm/signallistitemfactory.h>
#include <gtkmm/singleselection.h>
#include <gtkmm/sortlistmodel.h>
#include <gtkmm/stringsorter.h>

#include <cassert>
#include <thread>

class ElementListModel : public ElementStorage {
   public:
    struct Element : public Glib::Object {
        Element() : Glib::ObjectBase(typeid(Element)) {}

        std::string name;
        std::string plugin;
        std::string package;
    };

    static ElementListModel* create(ElementType type) { return new ElementListModel(type); }

    [[nodiscard]] unsigned int numEntries() const override
    {
        if (!m_is_populated) {
            Client::logger()->sendWarning("Tried to call `numEntries` before populating GstElementList!");
            return 0;
        }

        return m_elements->get_n_items();
    };
    [[nodiscard]] auto& getSelectionModel() const { return m_wrapper_model; };

    [[nodiscard]] std::string const& getElementName(unsigned int index) const override
    {
        return std::dynamic_pointer_cast<Element>(m_wrapper_model->get_object(index))->name;
    }
    [[nodiscard]] std::string const& getElementPlugin(unsigned int index) const override
    {
        return std::dynamic_pointer_cast<Element>(m_wrapper_model->get_object(index))->plugin;
    }
    [[nodiscard]] std::string const& getElementPackage(unsigned int index) const override
    {
        return std::dynamic_pointer_cast<Element>(m_wrapper_model->get_object(index))->package;
    }

    Glib::RefPtr<Gtk::ListItemFactory> createNameElementFactory()
    {
        auto factory = Gtk::SignalListItemFactory::create();
        factory->signal_setup().connect([](auto& item) {
            auto label = Gtk::make_managed<Gtk::Label>();
            label->set_halign(Gtk::Align::START);
            item->set_child(*label);
        });
        factory->signal_bind().connect([](auto& item) {
            auto element    = std::dynamic_pointer_cast<Element>(item->get_item());
            auto name_label = static_cast<Gtk::Label*>(item->get_child());

            name_label->set_text(element->name);
        });
        return factory;
    }

    Glib::RefPtr<Gtk::ListItemFactory> createPluginElementFactory()
    {
        auto factory = Gtk::SignalListItemFactory::create();
        factory->signal_setup().connect([](auto& item) {
            auto label = Gtk::make_managed<Gtk::Label>();
            label->set_halign(Gtk::Align::START);
            item->set_child(*label);
        });
        factory->signal_bind().connect([](auto& item) {
            auto element      = std::dynamic_pointer_cast<Element>(item->get_item());
            auto plugin_label = static_cast<Gtk::Label*>(item->get_child());

            plugin_label->set_text(element->plugin);
        });
        return factory;
    }

    Glib::RefPtr<Gtk::ListItemFactory> createPackageElementFactory()
    {
        auto factory = Gtk::SignalListItemFactory::create();
        factory->signal_setup().connect([](auto& item) {
            auto label = Gtk::make_managed<Gtk::Label>();
            label->set_halign(Gtk::Align::START);
            item->set_child(*label);
        });
        factory->signal_bind().connect([](auto& item) {
            auto element       = std::dynamic_pointer_cast<Element>(item->get_item());
            auto package_label = static_cast<Gtk::Label*>(item->get_child());

            package_label->set_text(element->package);
        });
        return factory;
    }

    void populate(bool blocking = false) override
    {
        if (m_is_populated)
            return;

        auto _populate = [=] { populateInternal(); };

        if (!blocking) {
            Client::logger()->sendDebug("Starting to populate element model asynchronously!");

            std::thread populate(_populate);
            populate.detach();
        } else {
            Client::logger()->sendDebug("Starting to populate element model synchronously!");

            _populate();
        }
    }

    sigc::signal<void()> finished_populating = {};

   private:
    ElementListModel(ElementType type) : m_type(type)
    {
        m_wrapper_model = Gtk::SingleSelection::create();
        m_wrapper_model->set_autoselect(false);

        m_dispatcher.connect(sigc::mem_fun(*this, &ElementListModel::updateElementStore));
    }

    void updateElementStore()
    {
        m_elements = m_temp_elements;

        m_wrapper_model->set_model(m_elements);

        finished_populating.emit();
        Client::logger()->sendDebug("Finished populating element model!");
    }

    void addElement(GstPlugin* plugin, GstPluginFeature* element)
    {
        auto elem = new Element;

        elem->name    = gst_element_get_name(element);
        elem->plugin  = gst_plugin_get_name(plugin);
        elem->package = gst_plugin_get_package(plugin);

        m_temp_elements->append(Glib::make_refptr_for_instance(elem));
    }

    static bool checkFeatureType(GstPluginFeature* feature, ElementType& type)
    {
        auto f = gst_element_factory_find(gst_plugin_feature_get_name(feature));

        if (!f)
            return false;

        bool result = false;
        switch (type) {
            case ElementType::SOURCE:
                result = gst_element_factory_list_is_type(f, GST_ELEMENT_FACTORY_TYPE_SRC);
                break;
            case ElementType::FILTER:
                result = !(gst_element_factory_list_is_type(f, GST_ELEMENT_FACTORY_TYPE_SRC) ||
                           gst_element_factory_list_is_type(f, GST_ELEMENT_FACTORY_TYPE_SINK));

                break;
            case ElementType::SINK:
                result = gst_element_factory_list_is_type(f, GST_ELEMENT_FACTORY_TYPE_SINK);
                break;
            default:
                break;
        }
        g_object_unref(f);
        return result;
    }

    /* Heavily inspired by the gst-inspect source code
     * https://github.com/GStreamer/gstreamer/blob/master/tools/gst-inspect.c */
    void populateInternal()
    {
        m_temp_elements = Gio::ListStore<Element>::create();

        auto registry = gst_registry_get();

        GList* plugins = gst_registry_get_plugin_list(registry);
        while (plugins) {
            auto plugin = GST_PLUGIN(plugins->data);
            plugins     = g_list_next(plugins);

            if (GST_OBJECT_FLAG_IS_SET(plugin, GST_PLUGIN_FLAG_BLACKLISTED))
                continue;

            GList* features = gst_registry_get_feature_list_by_plugin(registry, gst_plugin_get_name(plugin));
            while (features) {
                auto feature = GST_PLUGIN_FEATURE(features->data);
                features     = g_list_next(features);
                if (!checkFeatureType(feature, m_type))
                    continue;

                addElement(plugin, feature);
            }
            gst_plugin_feature_list_free(features);
        }

        gst_plugin_list_free(plugins);
        m_is_populated = true;

        m_dispatcher.emit();
    }

    bool m_is_populated = false;

    ElementType m_type;

    Glib::Dispatcher m_dispatcher;

    Glib::RefPtr<Gio::ListStore<Element>> m_elements      = nullptr;
    Glib::RefPtr<Gio::ListStore<Element>> m_temp_elements = nullptr;

    Glib::RefPtr<Gtk::SingleSelection> m_wrapper_model = nullptr;

    friend class ElementList;
};

ElementList::ElementList(ElementType type) : Gtk::ScrolledWindow(), m_view(new Gtk::ColumnView())
{
    set_size_request(400, 200);

    m_model = std::unique_ptr<ElementListModel>(ElementListModel::create(type));
    
    auto name_column = Gtk::ColumnViewColumn::create("Element", m_model->createNameElementFactory());
    {
        auto expression = Gtk::ClosureExpression<Glib::ustring>::create([](auto const& item) {
            auto element = std::dynamic_pointer_cast<ElementListModel::Element>(item);
            return element->name;
        });
        name_column->set_sorter(Gtk::StringSorter::create(expression));
    }
    m_view->append_column(name_column);

    auto plugin_column = Gtk::ColumnViewColumn::create("Plugin", m_model->createPluginElementFactory());
    {
        auto expression = Gtk::ClosureExpression<Glib::ustring>::create([](auto const& item) {
            auto element = std::dynamic_pointer_cast<ElementListModel::Element>(item);
            return element->plugin;
        });
        plugin_column->set_sorter(Gtk::StringSorter::create(expression));
    }
    m_view->append_column(plugin_column);

    auto package_column = Gtk::ColumnViewColumn::create("Package", m_model->createPackageElementFactory());
    {
        auto expression = Gtk::ClosureExpression<Glib::ustring>::create([](auto const& item) {
            auto element = std::dynamic_pointer_cast<ElementListModel::Element>(item);
            return element->package;
        });
        package_column->set_sorter(Gtk::StringSorter::create(expression));
    }
    m_view->append_column(package_column);

    name_column->set_expand(true);
    m_view->sort_by_column(name_column, Gtk::SortType::ASCENDING);

    m_view->set_model(m_model->getSelectionModel());
    m_model->finished_populating.connect([this]{ 
        // View -> Selection Model -> Sort List Model -> List Store
        auto sort_list_model = Gtk::SortListModel::create(m_model->m_elements, m_view->get_sorter());
        m_model->getSelectionModel()->set_model(sort_list_model);
    });

    m_view->add_css_class("data-table");
}

std::string ElementList::getCurrentElementName() const
{
    return m_model->getElementName(m_model->getSelectionModel()->get_selected());
}

std::string ElementList::getCurrentElementPluginName() const
{
    return m_model->getElementPlugin(m_model->getSelectionModel()->get_selected());
}

std::string ElementList::getCurrentElementPackage() const
{
    return m_model->getElementPackage(m_model->getSelectionModel()->get_selected());
}

void ElementList::populate()
{
    m_model->populate(false);

    // NOTE: This must not be in the ctor to prevent some crashes. This seems the most appropriate other place.
    set_child(*m_view);
}

void ElementList::when_selection_changed(std::function<void(guint)> cb)
{
    auto real_callback = [this, cb](auto const&, auto const&) { cb(m_model->getSelectionModel()->get_selected()); };
    m_model->getSelectionModel()->signal_selection_changed().connect(real_callback);
}
